use std::sync::Arc;

use indexmap::IndexMap;
use tauri::async_runtime::RwLock;

#[derive(serde::Serialize)]
pub struct Album {
  url: String,
  cover_url: String,
  title: String,
}

pub struct Spotify {
  search_result: RwLock<IndexMap<String, Album>>,
  token: RwLock<String>,
}

impl Spotify {
  pub fn new() -> Self {
    Self {
      search_result: RwLock::new(IndexMap::new()),
      token: RwLock::new("".into()),
    }
  }

  pub async fn init(self: &Arc<Self>) -> anyhow::Result<()> {
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
          url: format!("https://open.spotify.com/album/{}", id),
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
}
