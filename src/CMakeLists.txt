pkg_check_modules(gtk4 REQUIRED IMPORTED_TARGET gtk4>=4.11.3)
pkg_check_modules(libadwaita REQUIRED IMPORTED_TARGET libadwaita-1>=1.4)
pkg_check_modules(libsoup IMPORTED_TARGET libsoup-3.0)


if (libsoup_FOUND)
    add_compile_definitions(HAVE_LIBSOUP)
endif ()

set(SOURCES
    main.cpp
    Application.cpp
    Window.cpp
    WebView.cpp
    Tab.cpp
    BitmapPaintable.cpp
    LocationEntry.cpp
    JSConsole.cpp
    ViewImpl.cpp
    EventLoopImplementationGLib.cpp
    "${SERENITY_SOURCE_DIR}/Ladybird/Utilities.cpp"
    "${SERENITY_SOURCE_DIR}/Ladybird/HelperProcess.cpp"
    $<TARGET_OBJECTS:resources>
)

add_executable(ladybird "${SOURCES}")
target_link_libraries(ladybird PRIVATE
    PkgConfig::gtk4
    PkgConfig::libadwaita
    Lagom::Core
    Lagom::WebView
    Lagom::Gfx
    Lagom::Web
    Lagom::IPC
    Lagom::FileSystem
    Lagom::Protocol
)

if (libsoup_FOUND)
    target_link_libraries(ladybird PRIVATE PkgConfig::libsoup)
elseif(ENABLE_PUBLIC_SUFFIX_DOWNLOAD)
    target_compile_definitions(ladybird PRIVATE ENABLE_PUBLIC_SUFFIX_DOWNLOAD)
    target_link_libraries(ladybird PRIVATE Lagom::PublicSuffix)
endif()

set(WEBCONTENT_SOURCE_DIR "${SERENITY_SOURCE_DIR}/Userland/Services/WebContent")
set(WEBCONTENT_SOURCES
    "${WEBCONTENT_SOURCE_DIR}/ConnectionFromClient.cpp"
    "${WEBCONTENT_SOURCE_DIR}/ConsoleGlobalEnvironmentExtensions.cpp"
    "${WEBCONTENT_SOURCE_DIR}/PageHost.cpp"
    "${WEBCONTENT_SOURCE_DIR}/WebContentConsoleClient.cpp"
    "${WEBCONTENT_SOURCE_DIR}/WebDriverConnection.cpp"
    "${SERENITY_SOURCE_DIR}/Ladybird/FontPlugin.cpp"
    "${SERENITY_SOURCE_DIR}/Ladybird/HelperProcess.cpp"
    "${SERENITY_SOURCE_DIR}/Ladybird/ImageCodecPlugin.cpp"
    "${SERENITY_SOURCE_DIR}/Ladybird/Utilities.cpp"
    WebContentMain.cpp
)
if (libsoup_FOUND)
    set(WEBCONTENT_SOURCES
        RequestManagerSoup.cpp
        EventLoopImplementationGLib.cpp
        "${WEBCONTENT_SOURCES}"
    )
endif()

add_executable(WebContent "${WEBCONTENT_SOURCES}")
target_link_libraries(WebContent PRIVATE
    Lagom::Audio
    Lagom::Core
    Lagom::FileSystem
    Lagom::Gfx
    Lagom::IPC
    Lagom::JS
    Lagom::Main
    Lagom::Web
    Lagom::WebSocket
    Lagom::WebView
    Lagom::Protocol
)
if (libsoup_FOUND)
    target_link_libraries(WebContent PRIVATE PkgConfig::libsoup)
endif()
