use serde::{Deserialize, Serialize};
use tokio::{io::AsyncWriteExt, fs::OpenOptions};

#[derive(Serialize, Deserialize, Debug, Default, Clone)]
pub struct QobuzConfig {
    pub email: String,
    pub password: String,
    pub app_id: String,
    pub app_secret: String,
}
#[derive(Serialize, Deserialize, Debug, Default, Clone)]
pub struct TidalConfig {
    pub token: String,
    pub country: String,
    pub app_id: String,
    pub app_secret: String,
}
#[derive(Serialize, Deserialize, Debug, Default, Clone)]
pub struct SpotifyConfig {
    pub email: String,
    pub password: String,
    pub app_id: String,
    pub app_secret: String,
}
#[derive(Serialize, Deserialize, Debug, Default, Clone)]
pub struct Config {
    pub bind_address: String,
    pub mpd_address: String,
    pub qobuz: QobuzConfig,
    pub tidal: TidalConfig,
    pub spotify: SpotifyConfig,
}

pub fn load_config(j: &str) -> Result<Config, std::io::Error> {
    let c: Config = toml::from_str(&j).unwrap();
    Ok(c)
}

pub async fn write_config(c: &Config) -> anyhow::Result<()> {
    let proj_dirs = directories::ProjectDirs::from("com", "THMonster", "prelude").unwrap();
    let d = proj_dirs.config_dir();
    let _ = std::fs::create_dir_all(&d);
    let config_path = d.join("config.toml");
    if !config_path.exists() {
        let _ = std::fs::File::create(&config_path);
    }
    let mut f = OpenOptions::new()
        .write(true)
        .truncate(true)
        .open(config_path)
        .await?;
    f.write_all(toml::to_string_pretty(c).unwrap().as_bytes()).await?;
    f.sync_all().await?;
    Ok(())
}
