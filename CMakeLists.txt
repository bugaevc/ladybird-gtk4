cmake_minimum_required(VERSION 3.23)
project(ladybird-gtk4
  VERSION 1.0.0
  LANGUAGES C CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(PkgConfig)
pkg_check_modules(gtk4 REQUIRED IMPORTED_TARGET gtk4>=4.11.3)
pkg_check_modules(libadwaita REQUIRED IMPORTED_TARGET libadwaita-1>=1.4)

find_program(GLIB_COMPILE_RESOURCES NAMES glib-compile-resources REQUIRED)

find_program(BLUEPRINT_COMPILER NAMES blueprint-compiler REQUIRED)

find_package(Threads REQUIRED)
find_package(Lagom CONFIG REQUIRED)
find_package(ladybird CONFIG REQUIRED)

add_compile_definitions(AK_DONT_REPLACE_STD)

set(RESOURCE_FILES)
function(add_blueprint source_file ui_file)
    add_custom_command(
        OUTPUT "${ui_file}"
        COMMAND "${BLUEPRINT_COMPILER}"
        ARGS
            compile
            --output "${CMAKE_CURRENT_BINARY_DIR}/${ui_file}"
            "${CMAKE_CURRENT_SOURCE_DIR}/${source_file}"
        DEPENDS "${source_file}"
    )
    list(APPEND RESOURCE_FILES "${ui_file}")
    set(RESOURCE_FILES ${RESOURCE_FILES} PARENT_SCOPE)
endfunction()

add_blueprint(window.blp window.ui)

set(SOURCE_GRESOURCE_FILE "${CMAKE_CURRENT_SOURCE_DIR}/gresources.xml")
set(GENERATED_GRESOURCE_FILE "${CMAKE_CURRENT_BINARY_DIR}/ladybird-resources.c")

add_custom_command(
    OUTPUT "${GENERATED_GRESOURCE_FILE}"
    COMMAND "${GLIB_COMPILE_RESOURCES}"
    ARGS
        --target "${GENERATED_GRESOURCE_FILE}"
        --generate-source
        --sourcedir "${CMAKE_CURRENT_SOURCE_DIR}"
        --sourcedir "${CMAKE_CURRENT_BINARY_DIR}"
        "${SOURCE_GRESOURCE_FILE}"
    DEPENDS "${SOURCE_GRESOURCE_FILE}" ${RESOURCE_FILES}
)
add_library(resources OBJECT "${GENERATED_GRESOURCE_FILE}")
target_link_libraries(resources PRIVATE PkgConfig::gtk4)
set_target_properties(resources PROPERTIES COMPILE_OPTIONS "")

set(SOURCES
    main.cpp
    Window.cpp
    WebView.cpp
    ViewImpl.cpp
    EventLoopImplementationGLib.cpp
    $<TARGET_OBJECTS:resources>
)

add_executable(ladybird ${SOURCES})
target_link_libraries(ladybird PRIVATE PkgConfig::gtk4 PkgConfig::libadwaita ladybird::ladybird Lagom::Core Lagom::WebView Lagom::Gfx Lagom::Web Lagom::IPC Lagom::FileSystem Lagom::SQL Lagom::Protocol)

if (NOT CMAKE_SKIP_INSTALL_RULES)
  include(cmake/InstallRules.cmake)
endif()
