use std::{any, io::Read, slice::SliceIndex, sync::Arc};

use bytes::Bytes;
use futures::Stream;
use indexmap::IndexMap;
use librespot::{
    audio::{AudioDecrypt, AudioFile},
    core::{session::Session, spotify_id::SpotifyId},
};
use tauri::async_runtime::RwLock;

#[derive(serde::Serialize)]
pub struct Track {
    id: String,
    duration: i64,
    performers: String,
    title: String,
}
#[derive(serde::Serialize)]
pub struct Album {
    id: String,
    cover_url: String,
    title: String,
}

pub struct Spotify {
    email: String,
    password: String,
    search_result: RwLock<IndexMap<String, Album>>,
    token: RwLock<String>,
    session: RwLock<Option<Session>>,
}

impl Spotify {
    pub fn new(c: &crate::config::Config) -> Self {
        Self {
            search_result: RwLock::new(IndexMap::new()),
            token: RwLock::new("".into()),
            email: c.spotify.email.clone(),
            password: c.spotify.password.clone(),
            session: RwLock::new(None),
        }
    }

    pub async fn init(&self) -> anyhow::Result<()> {
        let credentials =
            librespot::discovery::Credentials::with_password(&self.email, &self.password);
        let session = librespot::core::session::Session::connect(
            librespot::core::config::SessionConfig::default(),
            credentials,
            None,
        )
        .await?;
        *self.session.write().await = Some(session);

        let client = reqwest::Client::builder()
            .connect_timeout(std::time::Duration::from_secs(10))
            .build()?;
        let payload = [("grant_type", "client_credentials")];
        let j = client
      .post("https://accounts.spotify.com/api/token")
      .header("authorization", r#"Basic Y2E1YTU4Zjk4ZTBkNGJjNmE2ZThhMmZkNWIzZTg5NWQ6YWJjZTQzZDllNmNkNDZhOTgyNDFlZTI1NDFhZWZjMWU="#)
      .form(&payload)
      .send()
      .await?
      .json::<serde_json::Value>()
      .await?;
        let t = j
            .pointer("/access_token")
            .ok_or(anyhow::anyhow!("spotify init err 1"))?
            .as_str()
            .unwrap();
        let mut token = self.token.write().await;
        token.clear();
        token.push_str("Bearer ");
        token.push_str(t);
        Ok(())
    }

    pub async fn search(&self, keywords: &str) -> anyhow::Result<()> {
        let client = reqwest::Client::builder()
            .connect_timeout(std::time::Duration::from_secs(10))
            .build()?;
        let j = client
            .get("https://api.spotify.com/v1/search")
            .header("authorization", self.token.read().await.as_str())
            .query(&[
                ("q", keywords),
                ("type", "album"),
                ("market", "JP"),
                ("limit", "50"),
            ])
            .send()
            .await?
            .json::<serde_json::Value>()
            .await?;
        // println!("{:?}", &j);
        let items = j
            .pointer("/albums/items")
            .ok_or(anyhow::anyhow!("spotify search err 1"))?
            .as_array()
            .unwrap();
        let mut re = self.search_result.write().await;
        re.clear();
        for item in items {
            let id = item
                .pointer("/id")
                .ok_or(anyhow::anyhow!("spotify search err 2"))?
                .as_str()
                .unwrap();
            let cover_url = item
                .pointer("/images/0/url")
                .ok_or(anyhow::anyhow!("spotify search err 3"))?
                .as_str()
                .unwrap();
            let title = item
                .pointer("/name")
                .ok_or(anyhow::anyhow!("spotify search err 4"))?
                .as_str()
                .unwrap();
            re.insert(
                id.into(),
                Album {
                    id: format!("{}", id),
                    cover_url: cover_url.into(),
                    title: title.into(),
                },
            );
        }
        re.reverse();
        Ok(())
    }

    pub async fn fetch_result(&self) -> Vec<Album> {
        let mut ret = Vec::new();
        let mut re = self.search_result.write().await;
        loop {
            match re.pop() {
                Some((_, v)) => {
                    ret.push(v);
                    if ret.len() > 9 {
                        break;
                    }
                }
                None => {
                    break;
                }
            };
        }
        ret
    }

    pub async fn get_album_tracks(&self, id: &str) -> anyhow::Result<Vec<Track>> {
        let mut ret = Vec::new();
        let client = reqwest::Client::builder()
            .connect_timeout(std::time::Duration::from_secs(10))
            .build()?;
        let j = client
            .get(format!("https://api.spotify.com/v1/albums/{}/tracks", &id))
            .header("authorization", self.token.read().await.as_str())
            .query(&[("market", "JP"), ("limit", "50")])
            .send()
            .await?
            .json::<serde_json::Value>()
            .await?;
        println!("{:?}", &j.to_string());
        let items = j
            .pointer("/items")
            .ok_or(anyhow::anyhow!("gat err 1"))?
            .as_array()
            .unwrap();
        for item in items {
            if item
                .pointer("/is_playable")
                .ok_or(anyhow::anyhow!("gat err 5"))?
                .as_bool()
                .unwrap()
                == false
            {
                continue;
            };
            let id = item
                .pointer("/id")
                .ok_or(anyhow::anyhow!("gat err 2"))?
                .as_str()
                .unwrap();
            let duration = item
                .pointer("/duration_ms")
                .ok_or(anyhow::anyhow!("gat err 3"))?
                .as_i64()
                .unwrap();
            let title = item
                .pointer("/name")
                .ok_or(anyhow::anyhow!("gat err 4"))?
                .as_str()
                .unwrap();
            let artists = item
                .pointer("/artists")
                .ok_or(anyhow::anyhow!("gat err 6"))?
                .as_array()
                .unwrap();
            let mut performers: String = "".into();
            for artist in artists {
                if !performers.is_empty() {
                    performers.push_str(", ");
                }
                performers.push_str(
                    artist
                        .pointer("/name")
                        .ok_or(anyhow::anyhow!("gat err 7"))?
                        .as_str()
                        .unwrap(),
                );
            }
            ret.push(Track {
                id: format!("{}", id),
                duration: duration / 1000,
                title: title.into(),
                performers,
            });
        }
        Ok(ret)
    }
    pub async fn get_track_file(
        &self,
        track_id: &str,
    ) -> anyhow::Result<tokio::sync::mpsc::UnboundedReceiver<Bytes>> {
        use librespot::metadata::{FileFormat, Metadata, Track};
        let session = self.session.read().await;
        let session = session.as_ref().unwrap();
        let id = match SpotifyId::from_base62(track_id) {
            Ok(id) => id,
            Err(_) => return Err(anyhow::anyhow!("gtf err 1")),
        };
        let track = match Track::get(session, id).await {
            Ok(it) => it,
            Err(_) => return Err(anyhow::anyhow!("gtf err 2")),
        };
        println!("{:?}", &track.files);
        // let mut file_id = None;
        let file_id = if let Some(f) = track.files.get(&FileFormat::OGG_VORBIS_160) {
            f
        } else {
            return Err(anyhow::anyhow!("gtf err 3"));
        };

        let key = match session.audio_key().request(track.id, *file_id).await {
            Ok(it) => it,
            Err(_) => return Err(anyhow::anyhow!("gtf err 4")),
        };
        let encrypted = match AudioFile::open(session, *file_id, 1024 * 1024, true).await {
            Ok(it) => it,
            Err(_) => return Err(anyhow::anyhow!("gtf err 5")),
        };
        let size = encrypted.get_stream_loader_controller().len();
        let mut decrypted = AudioDecrypt::new(key, encrypted);
        // skip
        let mut skip: [u8; 0xa7] = [0; 0xa7];
        let mut decrypted =
            tokio::task::spawn_blocking(move || match decrypted.read_exact(&mut skip) {
                Ok(_) => Ok(decrypted),
                Err(e) => Err(e),
            })
            .await??;
        let (tx, rx) = tokio::sync::mpsc::unbounded_channel();
        tokio::spawn(async move {
            loop {
                // Blocking reader
                let (d, read, buf) = tokio::task::spawn_blocking(move || {
                    let mut buf = vec![0; 1024 * 64];
                    match decrypted.read(&mut buf) {
                        Ok(r) => {
                            buf.truncate(r);
                            Ok((decrypted, r, buf))
                        }
                        Err(e) => Err(e),
                    }
                })
                .await??;
                decrypted = d;
                if read == 0 {
                    break;
                }
                if tx.send(Bytes::from(buf)).is_err() {
                    break;
                }
            }
            anyhow::Ok(())
        });
        Ok(rx)
    }
}
