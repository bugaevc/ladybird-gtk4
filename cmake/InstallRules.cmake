include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

set(package ladybird-gtk4)

install(TARGETS ladybird
  EXPORT ladybirdTargets
  RUNTIME
    COMPONENT ladybird_Runtime
    DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY
    COMPONENT ladybird_Runtime
    NAMELINK_COMPONENT ladybird_Development
    DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

write_basic_package_version_file(
    "${package}ConfigVersion.cmake"
    COMPATIBILITY SameMajorVersion
)

# Allow package maintainers to freely override the path for the configs
set(
  ladybird_INSTALL_CMAKEDIR "${CMAKE_INSTALL_DATADIR}/${package}"
  CACHE PATH "CMake package config location relative to the install prefix"
)
mark_as_advanced(ladybird_INSTALL_CMAKEDIR)

install(
  FILES cmake/LadybirdInstallConfig.cmake
  DESTINATION "${ladybird_INSTALL_CMAKEDIR}"
  RENAME "${package}Config.cmake"
  COMPONENT ladybird_Development
)

install(
  FILES "${PROJECT_BINARY_DIR}/${package}ConfigVersion.cmake"
  DESTINATION "${ladybird_INSTALL_CMAKEDIR}"
  COMPONENT ladybird_Development
)

install(
  EXPORT ladybirdTargets
  NAMESPACE ladybird-gtk4::
  DESTINATION "${ladybird_INSTALL_CMAKEDIR}"
  COMPONENT ladybird_Development
)
