use crate::server::{Album, Track};
use anyhow::{anyhow, Result};
use std::sync::Arc;
use tokio::{process::Command, sync::RwLock};

struct TidalKey {
    device_code: String,
    country_code: String,
    access_token: String,
}

pub struct Tidal {
    base_api_url: String,
    auth_url: String,
    app_id: String,
    app_secret: String,
    key: RwLock<Option<TidalKey>>,
    pub ua: String,
}

impl Tidal {
    pub fn new(c: &crate::config::Config) -> Self {
        let key = if c.tidal.token.is_empty() {
            RwLock::new(None)
        } else {
            RwLock::new(Some(TidalKey {
                device_code: "".into(),
                country_code: c.tidal.country.clone(),
                access_token: c.tidal.token.clone(),
            }))
        };
        Self {
            base_api_url: "https://api.tidal.com/v1".into(),
            app_id: c.tidal.app_id.clone(),
            app_secret: c.tidal.app_secret.clone(),
            key,
            ua: "Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:94.0) Gecko/20100101 Firefox/94.0"
                .into(),
            auth_url: "https://auth.tidal.com/v1/oauth2".into(),
        }
    }

    pub async fn auth(&self) -> Result<()> {
        if self.key.read().await.is_some() {
            return Ok(());
        }
        let client = reqwest::Client::builder()
            .user_agent(&self.ua)
            .connect_timeout(tokio::time::Duration::from_secs(10))
            .build()?;
        let payload = [
            ("client_id", self.app_id.as_str()),
            ("scope", "r_usr+w_usr+w_sub"),
        ];
        let resp = client
            .post(format!("{}/device_authorization", &self.auth_url))
            .form(&payload)
            .send()
            .await?
            .json::<serde_json::Value>()
            .await?;
        println!("{:?}", &resp);
        let interval = resp
            .pointer("/interval")
            .ok_or(anyhow!("err 1"))?
            .as_u64()
            .unwrap();
        let device_code: String = resp
            .pointer("/deviceCode")
            .ok_or(anyhow!("err 2"))?
            .as_str()
            .unwrap()
            .into();
        let mut child = Command::new("xdg-open")
            .arg(format!(
                "https://{}",
                resp.pointer("/verificationUriComplete")
                    .ok_or(anyhow!("err 6"))?
                    .as_str()
                    .unwrap()
            ))
            .spawn()
            .expect("failed to spawn");
        let _ = child.wait().await;
        let payload = [
            ("client_id", self.app_id.as_str()),
            ("device_code", device_code.as_str()),
            ("grant_type", "urn:ietf:params:oauth:grant-type:device_code"),
            ("scope", "r_usr+w_usr+w_sub"),
        ];
        loop {
            let resp = client
                .post(format!("{}/token", &self.auth_url))
                .basic_auth(self.app_id.as_str(), Some(self.app_secret.as_str()))
                .form(&payload)
                .send()
                .await?
                .json::<serde_json::Value>()
                .await?;
            println!("{:?}", &resp);
            match resp.pointer("/status") {
                Some(_) => {
                    tokio::time::sleep(std::time::Duration::from_secs(interval + 1)).await;
                    continue;
                }
                None => {}
            }
            let country_code = resp
                .pointer("/user/countryCode")
                .ok_or(anyhow!("err 3"))?
                .as_str()
                .unwrap()
                .into();
            let access_token = resp
                .pointer("/access_token")
                .ok_or(anyhow!("err 4"))?
                .as_str()
                .unwrap()
                .into();
            *self.key.write().await = Some(TidalKey {
                device_code,
                country_code,
                access_token,
            });
            break;
        }
        Ok(())
    }

    pub async fn add_album(&self, album_id: &str) -> Result<()> {
        let client = reqwest::Client::builder()
            .user_agent(&self.ua)
            .connect_timeout(tokio::time::Duration::from_secs(10))
            .build()?;
        let j = client
            .get(format!("{}/albums/{}/items", self.base_api_url, album_id))
            .header(
                "authorization",
                format!(
                    "Bearer {}",
                    self.key
                        .read()
                        .await
                        .as_ref()
                        .unwrap()
                        .access_token
                        .as_str()
                ),
            )
            .query(&[
                (
                    "countryCode",
                    self.key
                        .read()
                        .await
                        .as_ref()
                        .unwrap()
                        .country_code
                        .as_str(),
                ),
                ("limit", "100"),
            ])
            .send()
            .await?
            .json::<serde_json::Value>()
            .await?;
        // println!("{}", j.to_string());
        let mut tracks = Vec::new();
        for t in j.pointer("/items").unwrap().as_array().unwrap() {
            let title = t.pointer("/item/title").unwrap().as_str().unwrap().into();
            let track = Track {
                id: t.pointer("/item/id").unwrap().as_i64().unwrap().to_string(),
                performers: t
                    .pointer("/item/artists/0/name")
                    .unwrap()
                    .as_str()
                    .unwrap()
                    .into(),
                title,
            };
            tracks.push(track);
        }
        let album = Album {
            img: j
                .pointer("/items/0/item/album/cover")
                .unwrap()
                .as_str()
                .unwrap()
                .into(),
            title: j
                .pointer("/items/0/item/album/title")
                .unwrap()
                .as_str()
                .unwrap()
                .into(),
            tracks,
        };
        println!("{:?}", album);
        // self.mpd.add_album_to_mpd(&album, "tidal").await?;
        Ok(())
    }

    pub async fn get_tacks_file(&self, track_id: &str) -> Result<String> {
        let client = reqwest::Client::builder()
            .user_agent(&self.ua)
            .connect_timeout(tokio::time::Duration::from_secs(10))
            .build()?;
        let j = client
            .get(format!(
                "{}/tracks/{}/playbackinfopostpaywall",
                self.base_api_url, track_id
            ))
            .header(
                "authorization",
                format!(
                    "Bearer {}",
                    self.key
                        .read()
                        .await
                        .as_ref()
                        .unwrap()
                        .access_token
                        .as_str()
                ),
            )
            .query(&[
                (
                    "countryCode",
                    self.key
                        .read()
                        .await
                        .as_ref()
                        .unwrap()
                        .country_code
                        .as_str(),
                ),
                ("audioquality", "HI_RES"),
                ("playbackmode", "STREAM"),
                ("assetpresentation", "FULL"),
            ])
            .send()
            .await?
            .json::<serde_json::Value>()
            .await?;
        println!("{}", j.to_string());
        let mf = j
            .pointer("/manifest")
            .ok_or(anyhow!("err 2"))?
            .as_str()
            .unwrap();
        let mf = base64::decode(mf)?;
        let u = if j
            .pointer("/manifestMimeType")
            .ok_or(anyhow!("err 3"))?
            .as_str()
            .unwrap()
            .eq("application/vnd.tidal.bts")
        {
            let j: serde_json::Value = serde_json::from_str(std::str::from_utf8(&mf)?)?;
            j.pointer("/urls/0")
                .ok_or(anyhow!("err 1"))?
                .as_str()
                .unwrap()
                .into()
        } else {
            println!("{}", std::str::from_utf8(&mf)?);
            let re = regex::Regex::new(r#"media="([^"]+)""#).unwrap();
            match re.captures(std::str::from_utf8(&mf)?) {
                Some(it) => it[1].replace("&amp;", "&").into(),
                None => "".into(),
            }
        };
        println!("{}", &u);
        Ok(u)
    }
}
