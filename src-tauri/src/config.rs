use serde::{Deserialize, Serialize};

#[derive(Serialize, Deserialize, Debug, Default)]
pub struct QobuzConfig {
    pub email: String,
    pub password: String,
    pub app_id: String,
    pub app_secret: String,
}
#[derive(Serialize, Deserialize, Debug, Default)]
pub struct TidalConfig {
    pub token: String,
    pub country: String,
    pub app_id: String,
    pub app_secret: String,
}
#[derive(Serialize, Deserialize, Debug, Default)]
pub struct SpotifyConfig {
    pub email: String,
    pub password: String,
    pub app_id: String,
    pub app_secret: String,
}
#[derive(Serialize, Deserialize, Debug, Default)]
pub struct Config {
    pub qobuz: QobuzConfig,
    pub tidal: TidalConfig,
    pub spotify: SpotifyConfig,
    pub bind_address: String,
    pub mpd_address: String,
}

pub fn load_config(j: &str) -> Result<Config, std::io::Error> {
    let c: Config = toml::from_str(&j).unwrap();
    Ok(c)
}
