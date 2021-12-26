use crate::{server, spotify};
use std::{ops::Deref, sync::Arc};

#[tauri::command]
pub async fn init(server: tauri::State<'_, Arc<crate::server::Server>>) -> Result<(), String> {
    let s: Arc<crate::server::Server> = server.deref().clone();
    tokio::spawn(async move { s.init().await });
    Ok(())
}

#[tauri::command]
pub async fn search_spotify(keywords: String, server: tauri::State<'_, Arc<crate::server::Server>>) -> Result<(), String> {
    let _ = server.state.spotify.search(&keywords).await;
    Ok(())
}

#[tauri::command]
pub async fn get_spotify_search_result(server: tauri::State<'_, Arc<crate::server::Server>>) -> Result<Vec<spotify::Album>, String> {
    Ok(server.state.spotify.fetch_result().await)
}

#[tauri::command]
pub async fn get_spotify_album_tracks(id: String, server: tauri::State<'_, Arc<crate::server::Server>>) -> Result<Vec<spotify::Track>, String> {
    if let Ok(tracks) = server.state.spotify.get_album_tracks(&id).await {
        Ok(tracks)
    } else {
        if let Ok(tracks) = server.state.spotify.get_playlist_tracks(&id).await {
            Ok(tracks)
        } else {
            Err("get spotify album tracks error".into())
        }
    }
}

#[tauri::command]
pub async fn add_spotify_tracks_to_mpd(album: server::Album, server: tauri::State<'_, Arc<crate::server::Server>>) -> Result<(), String> {
    server
        .state
        .mpd
        .add_album_to_mpd(&album, "spotify")
        .await
        .map_err(|_| "add spotify tracks to mpd error".to_string())?;
    Ok(())
}
