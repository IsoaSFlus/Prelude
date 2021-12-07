mod mpd;
mod qobuz;
mod tidal;

use axum::{
    body::StreamBody,
    extract::{Extension, Path, Query},
    response::IntoResponse,
    routing::get,
    AddExtensionLayer, Router,
};
use clap::Arg;
use hyper::StatusCode;
use mpd::MPD;
use qobuz::Qobuz;
use serde::{Deserialize, Serialize};
use std::{collections::HashMap, sync::Arc};
use tidal::Tidal;

#[derive(Serialize, Deserialize, Debug, Default)]
pub struct QobuzConfig {
    pub email: String,
    pub password: String,
    pub app_id: String,
    pub app_secret: String,
}
#[derive(Serialize, Deserialize, Debug, Default)]
pub struct TidalConfig {
    pub email: String,
    pub password: String,
    pub app_id: String,
    pub app_secret: String,
}
#[derive(Serialize, Deserialize, Debug, Default)]
pub struct Config {
    pub qobuz: QobuzConfig,
    pub tidal: TidalConfig,
}
pub fn load_config(j: &str) -> Result<Config, std::io::Error> {
    let c: Config = toml::from_str(&j).unwrap();
    Ok(c)
}

#[derive(Debug, Default)]
pub struct Track {
    pub id: i64,
    pub performers: String,
    pub title: String,
    // work: String,
}
#[derive(Debug, Default)]
pub struct Album {
    pub img: String,
    pub title: String,
    pub tracks: Vec<Track>,
}

pub struct State {
    qobuz: Qobuz,
    tidal: Tidal,
}

fn main() {
    let ma = clap::App::new("archlinux-repo")
        .author("THMonster <lltiueob@live.com>")
        .about("Qobuz for MPD")
        .arg(
            Arg::with_name("bind-address")
                .short("b")
                .long("bind-address")
                .value_name("ADDRESS")
                .help("the address to bind")
                .takes_value(true)
                .required(true),
        )
        .get_matches();
    let address = ma.value_of("bind-address").unwrap().to_string();

    tokio::runtime::Builder::new_multi_thread().enable_all().build().unwrap().block_on(async move {
        let proj_dirs = directories::ProjectDirs::from("com", "THMonster", "prelude").unwrap();
        let d = proj_dirs.config_dir();
        let _ = tokio::fs::create_dir_all(&d).await;
        let config_path = d.join("config.toml");
        if !config_path.exists() {
            let _ = tokio::fs::File::create(&config_path).await;
        }
        let c = std::fs::read(config_path).unwrap();
        let c = String::from_utf8_lossy(&c);
        let c = load_config(&c).unwrap();
        tokio::spawn(async move {
            loop {
                tokio::time::sleep(std::time::Duration::new(30, 0)).await;
            }
        });
        let mpd = Arc::new(MPD::new("127.0.0.1:6600", &address));

        let state = Arc::new(State {
            qobuz: Qobuz::new(
                &c.qobuz.email,
                &c.qobuz.password,
                &c.qobuz.app_id,
                &c.qobuz.app_secret,
                mpd.clone(),
            )
            .await,
            tidal: Tidal::new(
                &c.tidal.email,
                &c.tidal.password,
                &c.tidal.app_id,
                &c.tidal.app_secret,
                mpd.clone(),
            )
            .await,
        });
        match state.tidal.auth().await {
            Ok(_) => {}
            Err(e) => {
                println!("{}", e)
            }
        };
        // build our application with a single route
        let app = Router::new()
            .route("/track/:id", get(get_track))
            .route("/add_qobuz/:album_id", get(add_album_qo))
            .route("/add_tidal/:album_id", get(add_album_ti))
            .layer(AddExtensionLayer::new(state));

        // run it with hyper on localhost:3000
        axum::Server::bind(&address.parse().unwrap()).serve(app.into_make_service()).await.unwrap();
    });
}

async fn get_track(
    Extension(state): Extension<Arc<State>>,
    Path(id): Path<i64>,
    Query(params): Query<HashMap<String, String>>,
) -> impl IntoResponse {
    let u = if params.get("t").eq(&Some(&"tidal".into())) {
        state.tidal.get_tacks_url(id.to_string().as_str()).await.unwrap()
    } else {
        state.qobuz.get_tacks_file(id.to_string().as_str()).await.unwrap()
    };
    let client = reqwest::Client::builder()
        .user_agent(&state.qobuz.ua)
        .connect_timeout(tokio::time::Duration::from_secs(10))
        .build()
        .unwrap();
    let stream = async_stream::stream! {
        if u.contains(r#"$Number$.mp4"#) {
            let mut i = 0usize;
            while let Ok(mut resp) = client.get(u.replace("$Number$", i.to_string().as_str())).send().await {
                if resp.status() != 200 {
                    break;
                }
                i += 1usize;
                while let Ok(Some(chunk)) = resp.chunk().await {
                    yield anyhow::Ok(chunk);
                }
            }
        } else {
            if let Ok(mut resp) = client.get(&u).send().await {
                while let Ok(Some(chunk)) = resp.chunk().await {
                    yield anyhow::Ok(chunk);
                }
            }
        }
    };
    StreamBody::new(stream)
}

async fn add_album_qo(Extension(state): Extension<Arc<State>>, Path(album_id): Path<String>) -> impl IntoResponse {
    let _ = state.qobuz.add_album(&album_id).await;
    StatusCode::OK
}

async fn add_album_ti(Extension(state): Extension<Arc<State>>, Path(album_id): Path<String>) -> impl IntoResponse {
    let _ = state.tidal.add_album(&album_id).await;
    StatusCode::OK
}
