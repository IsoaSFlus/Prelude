use crate::{mpd::MPD, qobuz::Qobuz, tidal::Tidal};
use axum::{
    body::StreamBody,
    extract::{Extension, Path, Query},
    response::IntoResponse,
    routing::get,
    AddExtensionLayer, Router,
};
use bytes::Bytes;
use hyper::StatusCode;
use std::{collections::HashMap, sync::Arc};

use crate::spotify::Spotify;

#[derive(serde::Deserialize, Debug, Default)]
pub struct Track {
    pub id: String,
    pub performers: String,
    pub title: String,
    // work: String,
}
#[derive(serde::Deserialize, Debug, Default)]
pub struct Album {
    pub img: String,
    pub title: String,
    pub tracks: Vec<Track>,
}

pub struct State {
    pub qobuz: Qobuz,
    pub tidal: Tidal,
    pub spotify: Spotify,
    pub mpd: MPD,
}

pub struct Server {
    pub state: Arc<State>,
    config: crate::config::Config,
}

impl Server {
    pub fn new() -> Self {
        let proj_dirs = directories::ProjectDirs::from("com", "THMonster", "prelude").unwrap();
        let d = proj_dirs.config_dir();
        let _ = std::fs::create_dir_all(&d);
        let config_path = d.join("config.toml");
        if !config_path.exists() {
            let _ = std::fs::File::create(&config_path);
        }
        let c = std::fs::read(config_path).unwrap();
        let c = String::from_utf8_lossy(&c);
        let c = crate::config::load_config(&c).unwrap();
        let state = Arc::new(State {
            qobuz: Qobuz::new(&c),
            tidal: Tidal::new(&c),
            spotify: Spotify::new(&c),
            mpd: MPD::new(&c.mpd_address, &c.bind_address),
        });
        Self { state, config: c }
    }

    pub async fn init(&self) {
        tokio::spawn(async move {
            loop {
                tokio::time::sleep(std::time::Duration::new(30, 0)).await;
            }
        });

        match self.state.tidal.auth().await {
            Ok(_) => {}
            Err(e) => {
                println!("{}", e)
            }
        };
        match self
            .state
            .qobuz
            .auth(
                self.config.qobuz.email.as_str(),
                self.config.qobuz.password.as_str(),
            )
            .await
        {
            Ok(_) => {}
            Err(e) => {
                println!("{}", e)
            }
        };
        match self.state.spotify.init().await {
            Ok(_) => {}
            Err(e) => {
                println!("{}", e)
            }
        }
        // build our application with a single route
        let app = Router::new()
            .route("/track/:id", get(get_track))
            .route("/add_qobuz/:album_id", get(add_album_qo))
            .route("/add_tidal/:album_id", get(add_album_ti))
            .layer(AddExtensionLayer::new(self.state.clone()));

        // run it with hyper on localhost:3000
        axum::Server::bind(&self.config.bind_address.parse().unwrap())
            .serve(app.into_make_service())
            .await
            .unwrap();
    }
}

async fn get_track(
    Extension(state): Extension<Arc<State>>,
    Path(id): Path<String>,
    Query(params): Query<HashMap<String, String>>,
) -> impl IntoResponse {
    let mut rx = if params.get("t").eq(&Some(&"tidal".into())) {
        let u = state
            .tidal
            .get_tacks_file(id.as_str())
            .await
            .map_err(|_| "err")?;
        url_to_stream(u, &state.qobuz.ua).await.map_err(|_| "err")?
    } else if params.get("t").eq(&Some(&"qobuz".into())) {
        let u = state
            .qobuz
            .get_tacks_file(id.as_str())
            .await
            .map_err(|_| "err")?;
        url_to_stream(u, &state.qobuz.ua).await.map_err(|_| "err")?
    } else {
        state
            .spotify
            .get_track_file(id.as_str())
            .await
            .map_err(|_| "err")?
    };

    let stream = async_stream::stream! {
        while let Some(chunk) = rx.recv().await {
            yield anyhow::Ok(chunk);
        }
    };
    Ok::<_, &str>(StreamBody::new(stream))
}

async fn add_album_qo(
    Extension(state): Extension<Arc<State>>,
    Path(album_id): Path<String>,
) -> impl IntoResponse {
    let _ = state.qobuz.add_album(&album_id).await;
    StatusCode::OK
}

async fn add_album_ti(
    Extension(state): Extension<Arc<State>>,
    Path(album_id): Path<String>,
) -> impl IntoResponse {
    let _ = state.tidal.add_album(&album_id).await;
    StatusCode::OK
}

async fn url_to_stream(
    u: String,
    ua: &str,
) -> anyhow::Result<tokio::sync::mpsc::UnboundedReceiver<Bytes>> {
    let client = reqwest::Client::builder()
        .user_agent(ua)
        .connect_timeout(tokio::time::Duration::from_secs(10))
        .build()
        .unwrap();
    let client = Arc::new(client);
    let (tx, rx) = tokio::sync::mpsc::unbounded_channel();
    tokio::spawn(async move {
        if u.contains(r#"$Number$.mp4"#) {
            let mut i = 0usize;
            while let Ok(mut resp) = client
                .get(u.replace("$Number$", i.to_string().as_str()))
                .send()
                .await
            {
                if resp.status() != 200 {
                    break;
                }
                i += 1usize;
                while let Ok(Some(chunk)) = resp.chunk().await {
                    if tx.send(chunk).is_err() {
                        break;
                    }
                }
            }
        } else {
            if let Ok(mut resp) = client.get(&u).send().await {
                while let Ok(Some(chunk)) = resp.chunk().await {
                    if tx.send(chunk).is_err() {
                        break;
                    }
                }
            }
        }
    });
    Ok(rx)
}
