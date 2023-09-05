find_program(GLIB_COMPILE_RESOURCES NAMES glib-compile-resources REQUIRED)
find_program(BLUEPRINT_COMPILER NAMES blueprint-compiler REQUIRED)

pkg_check_modules(gio REQUIRED IMPORTED_TARGET gio-2.0)

set(RESOURCE_FILES style.css)
function(add_blueprint source_file ui_file)
    add_custom_command(
        OUTPUT "${ui_file}"
        COMMAND "${BLUEPRINT_COMPILER}"
        ARGS
            compile
            --output "${CMAKE_CURRENT_BINARY_DIR}/${ui_file}"
            "${CMAKE_CURRENT_SOURCE_DIR}/${source_file}"
        DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/${source_file}"
    )
    list(APPEND RESOURCE_FILES "${ui_file}")
    set(RESOURCE_FILES "${RESOURCE_FILES}" PARENT_SCOPE)
endfunction()

add_blueprint(window.blp window.ui)
add_blueprint(shortcuts-dialog.blp shortcuts-dialog.ui)
add_blueprint(tab.blp tab.ui)
add_blueprint(location-entry.blp location-entry.ui)
add_blueprint(js-console.blp js-console.ui)

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
    DEPENDS "${SOURCE_GRESOURCE_FILE}" "${RESOURCE_FILES}"
)
add_library(resources OBJECT "${GENERATED_GRESOURCE_FILE}")
target_link_libraries(resources PRIVATE PkgConfig::gio)
set_target_properties(resources PROPERTIES COMPILE_OPTIONS "")
