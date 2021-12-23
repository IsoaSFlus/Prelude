#![cfg_attr(
  all(not(debug_assertions), target_os = "windows"),
  windows_subsystem = "windows"
)]

mod cmd;
mod spotify;

use std::sync::Arc;

fn main() {
  let sp = Arc::new(spotify::Spotify::new());
  tauri::Builder::default()
    .manage(sp)
    .invoke_handler(tauri::generate_handler![
      cmd::init,
      cmd::search_spotify,
      cmd::get_spotify_search_result
    ])
    .run(tauri::generate_context!())
    .expect("error while running tauri application");
}
