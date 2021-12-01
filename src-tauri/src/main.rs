#![cfg_attr(all(not(debug_assertions), target_os = "windows"), windows_subsystem = "windows")]

mod cmd;
mod config;
mod mpd;
mod qobuz;
mod server;
mod spotify;
mod tidal;

use std::sync::Arc;

fn main() {
    let server = Arc::new(server::Server::new());
    tauri::Builder::default()
        .manage(server)
        .invoke_handler(tauri::generate_handler![
            cmd::init,
            cmd::search_spotify,
            cmd::get_spotify_search_result,
            cmd::get_spotify_album_tracks,
            cmd::add_spotify_tracks_to_mpd,
            cmd::search_in_tidal,
            cmd::add_album_to_mpd
        ])
        .run(tauri::generate_context!())
        .expect("error while running tauri application");
}
