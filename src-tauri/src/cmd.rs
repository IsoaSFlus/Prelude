use std::sync::Arc;

use crate::spotify;

#[tauri::command]
pub async fn init(sp: tauri::State<'_, Arc<crate::spotify::Spotify>>) -> Result<(), String> {
    match sp.init().await {
        Ok(_) => {}
        Err(_) => {
            return Err("spotify init error".into());
        }
    }
    Ok(())
}

#[tauri::command]
pub async fn search_spotify(
    keywords: String,
    sp: tauri::State<'_, Arc<crate::spotify::Spotify>>,
) -> Result<(), String> {
    let _ = sp.search(&keywords).await;
    Ok(())
}

#[tauri::command]
pub async fn get_spotify_search_result(
    sp: tauri::State<'_, Arc<crate::spotify::Spotify>>,
) -> Result<Vec<spotify::Album>, String> {
    Ok(sp.fetch_result().await)
}

#[tauri::command]
pub async fn get_spotify_album_tracks(
    id: String,
    sp: tauri::State<'_, Arc<crate::spotify::Spotify>>,
) -> Result<Vec<spotify::Track>, String> {
    if let Ok(tracks) = sp.get_album_tracks(&id).await {
        Ok(tracks)
    } else {
        Err("get spotify album tracks error".into())
    }
}
