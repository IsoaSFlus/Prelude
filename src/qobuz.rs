use crate::{mpd::MPD, Album, Track};
use anyhow::Result;
use std::sync::Arc;

pub struct Qobuz {
    base_api_url: String,
    app_id: String,
    app_secret: String,
    user_auth_token: String,
    pub ua: String,
    pub mpd: Arc<MPD>,
}

impl Qobuz {
    pub async fn new(email: &str, pwd: &str, app_id: &str, app_secret: &str, mpd: Arc<MPD>) -> Self {
        let base_api_url = "https://www.qobuz.com/api.json/0.2/".into();
        let mut r = Self {
            base_api_url,
            ua: "Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:94.0) Gecko/20100101 Firefox/94.0".into(),
            app_id: app_id.into(),
            app_secret: app_secret.into(),
            user_auth_token: "".into(),
            mpd,
        };
        r.user_auth_token = r.auth(email, pwd).await.unwrap();
        r
    }

    pub async fn auth(&self, email: &str, pwd: &str) -> Result<String> {
        let client = reqwest::Client::builder().user_agent(&self.ua).connect_timeout(tokio::time::Duration::from_secs(10)).build()?;
        let j = client
            .get(format!("{}user/login", self.base_api_url))
            .header("X-App-Id", self.app_id.as_str())
            .query(&[("email", email), ("password", pwd), ("app_id", self.app_id.as_str())])
            .send()
            .await?
            .json::<serde_json::Value>()
            .await?;
        println!("{:?}", &j.to_string());
        Ok(j.pointer("/user_auth_token").unwrap().as_str().unwrap().into())
    }

    pub async fn get_tacks_file(&self, track_id: &str) -> Result<String> {
        let client = reqwest::Client::builder().user_agent(&self.ua).connect_timeout(tokio::time::Duration::from_secs(10)).build()?;

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
            .header("X-User-Auth-Token", self.user_auth_token.as_str())
            .query(q)
            .send()
            .await?
            .json::<serde_json::Value>()
            .await?;
        println!("{}", j.to_string());
        Ok(j.pointer("/url").unwrap().as_str().unwrap().into())
    }

    pub async fn add_album(&self, album_id: &str) -> Result<()> {
        let client = reqwest::Client::builder().user_agent(&self.ua).connect_timeout(tokio::time::Duration::from_secs(10)).build()?;
        let j = client
            .get(format!("{}album/get", self.base_api_url))
            .header("X-App-Id", self.app_id.as_str())
            .header("X-User-Auth-Token", self.user_auth_token.as_str())
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
                id: t.pointer("/id").unwrap().as_i64().unwrap(),
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
        self.mpd.add_album_to_mpd(&album, "qobuz").await?;
        Ok(())
    }
}
