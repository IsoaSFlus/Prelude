set(qlp_file_SRCS
    main.cpp
    mainwindow.cpp
    album.cpp
    album_model.cpp
    search_engine.cpp
    tidal/tidal_core.cpp
    spotify/spotify_core.cpp
    qobuz/qobuz_core.cpp
    json/CJsonObject.cpp
    json/cJSON.c
    mpd/mpd_client.cpp
    addtracktompd.cpp
)

qt5_add_resources(qlp_file_SRCS resource.qrc)
add_executable(prelude ${qlp_file_SRCS})
target_compile_definitions(prelude PRIVATE -DPROJECT_VERSION="${PROJECT_VERSION}")
target_link_libraries(prelude
  Qt5::Core
  Qt5::Network
  Qt5::Gui
  Qt5::Widgets
  Qt5::QuickWidgets
  Qt5::QuickControls2
  -lmpdclient
)


install(TARGETS prelude DESTINATION bin)
