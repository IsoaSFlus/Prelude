use crate::server::{Album, Track, self};
use anyhow::Result;
use tauri::async_runtime::RwLock;

pub struct Qobuz {
    base_api_url: String,
    app_id: String,
    app_secret: String,
    user_auth_token: RwLock<String>,
    pub ua: String,
}

impl Qobuz {
    pub fn new(c: &crate::config::Config) -> Self {
        let base_api_url = "https://www.qobuz.com/api.json/0.2/".into();
        Self {
            base_api_url,
            ua: "Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:94.0) Gecko/20100101 Firefox/94.0"
                .into(),
            app_id: c.qobuz.app_id.clone(),
            app_secret: c.qobuz.app_secret.clone(),
            user_auth_token: RwLock::new("".into()),
        }
    }

    pub async fn auth(&self, email: &str, pwd: &str) -> Result<()> {
        let client = reqwest::Client::builder()
            .user_agent(&self.ua)
            .connect_timeout(tokio::time::Duration::from_secs(10))
            .build()?;
        let j = client
            .get(format!("{}user/login", self.base_api_url))
            .header("X-App-Id", self.app_id.as_str())
            .query(&[
                ("email", email),
                ("password", pwd),
                ("app_id", self.app_id.as_str()),
            ])
            .send()
            .await?
            .json::<serde_json::Value>()
            .await?;
        println!("{:?}", &j.to_string());
        self.user_auth_token
            .write()
            .await
            .push_str(j.pointer("/user_auth_token").unwrap().as_str().unwrap());
        Ok(())
    }

    pub async fn get_tacks_file(&self, track_id: &str) -> Result<String> {
        let client = reqwest::Client::builder()
            .user_agent(&self.ua)
            .connect_timeout(tokio::time::Duration::from_secs(10))
            .build()?;

        let ts = chrono::Utc::now().timestamp().to_string();
        let sig = format!(
            "trackgetFileUrlformat_id{}intentstreamtrack_id{}{}{}",
            "27", track_id, &ts, &self.app_secret
        );
        let sig = md5::compute(sig.as_bytes());
        let sig = format!("{:032x}", sig);
        let q = &[
            ("request_ts", ts.as_str()),
            ("request_sig", sig.as_str()),
            ("track_id", track_id),
            ("format_id", "27"),
            ("intent", "stream"),
        ];
        let j = client
            .get(format!("{}track/getFileUrl", self.base_api_url))
            .header("X-App-Id", self.app_id.as_str())
            .header(
                "X-User-Auth-Token",
                self.user_auth_token.read().await.as_str(),
            )
            .query(q)
            .send()
            .await?
            .json::<serde_json::Value>()
            .await?;
        println!("{}", j.to_string());
        Ok(j.pointer("/url").unwrap().as_str().unwrap().into())
    }

    pub async fn get_album(&self, album_id: &str) -> Result<server::Album> {
        let client = reqwest::Client::builder()
            .user_agent(&self.ua)
            .connect_timeout(tokio::time::Duration::from_secs(10))
            .build()?;
        let j = client
            .get(format!("{}album/get", self.base_api_url))
            .header("X-App-Id", self.app_id.as_str())
            .header(
                "X-User-Auth-Token",
                self.user_auth_token.read().await.as_str(),
            )
            .query(&[("album_id", album_id)])
            .send()
            .await?
            .json::<serde_json::Value>()
            .await?;
        // println!("{:?}", &j);
        let mut tracks = Vec::new();
        for t in j.pointer("/tracks/items").unwrap().as_array().unwrap() {
            let title = t.pointer("/title").unwrap().as_str().unwrap().into();
            let title = match t.pointer("/work") {
                Some(it) => match it.as_str() {
                    Some(it) => {
                        format!("{}: {}", it, title)
                    }
                    None => title,
                },
                None => title,
            };
            let track = Track {
                id: t.pointer("/id").unwrap().as_i64().unwrap().to_string(),
                performers: t.pointer("/performers").unwrap().as_str().unwrap().into(),
                title,
            };
            tracks.push(track);
        }
        let album = Album {
            img: j.pointer("/image/large").unwrap().as_str().unwrap().into(),
            title: j.pointer("/title").unwrap().as_str().unwrap().into(),
            tracks,
        };
        println!("{:?}", album);
        Ok(album)
    }
}
