project(
    'ladybird-gtk',
    'c',
    'cpp',
    license: 'BSD-2-Clause',
    version: '1.0-dev',
    default_options: [
        'warning_level=1',
        'buildtype=debugoptimized',
        'cpp_std=c++20',
    ],
)

gnome = import('gnome')

gtk_min_version = '>= 4.11.3'
libadwaita_min_version = '>=1.4'

gtk_dep = dependency(
    'gtk4',
    version: gtk_min_version,
    default_options: ['build-tests=false', 'build-testsuite=false'],
)
libadwaita_dep = dependency(
    'libadwaita-1',
    version: libadwaita_min_version,
    fallback: ['adwaita', 'libadwaita_dep'],
)

lagom_dep = dependency(
    'Lagom',
    method: 'cmake',
    modules: [
        'Lagom::Core',
        'Lagom::FileSystem',
        'Lagom::Gfx',
        'Lagom::IPC',
        'Lagom::Main',
        'Lagom::Web',
        'Lagom::WebView',
        'Lagom::SQL',
        'Lagom::Protocol',
    ],
)
ladybird_dep = dependency(
    'ladybird',
    method: 'cmake',
    modules: ['ladybird::ladybird'],
)

add_project_arguments(
    [
        '-DAK_DONT_REPLACE_STD',
        '-Wno-literal-suffix',
    ],
    language: 'cpp',
)


libadwaita_dep_type = libadwaita_dep.type_name()
if libadwaita_dep_type == 'pkgconfig'
    libadwaita_typelib_path = (
        libadwaita_dep.get_pkgconfig_variable('libdir') / 'girepository-1.0'
    )
else
    # FIXME: How to get the build dir of the subproject properly?
    libadwaita_typelib_path = (
        meson.current_build_dir() / 'subprojects/adwaita/src'
    )
endif

blueprints = custom_target(
    'blueprints',
    input: files('window.blp'),
    env: {
        'GI_TYPELIB_PATH': libadwaita_typelib_path,
    },
    output: 'window.ui',
    command: [
        find_program('blueprint-compiler'),
        'batch-compile',
        '@OUTDIR@',
        '@CURRENT_SOURCE_DIR@',
        '@INPUT@',
    ],
)

resources = gnome.compile_resources(
    'ladybird-resources',
    'gresources.xml',
    c_name: 'ladybird_resources',
    source_dir: '.',
    dependencies: [blueprints],
)

ladybird = executable(
    'ladybird',
    dependencies: [
        lagom_dep,
        gtk_dep,
        libadwaita_dep,
        ladybird_dep,
        declare_dependency(sources: [resources]),
    ],
    sources: [
        'main.cpp',
        'Window.cpp',
        'WebView.cpp',
        'ViewImpl.cpp',
        'EventLoopImplementationGLib.cpp',
    ],
)
