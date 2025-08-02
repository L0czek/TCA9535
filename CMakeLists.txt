cmake_minimum_required(VERSION 3.23)
project(tca9535 CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(tca9535 STATIC
    src/tca9535.cpp
)

target_include_directories(tca9535 PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc>
    $<INSTALL_INTERFACE:include>
)

# Export and install
include(GNUInstallDirs)

install(TARGETS tca9535 EXPORT tca9535Targets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(DIRECTORY inc/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(EXPORT tca9535Targets
    FILE tca9535Targets.cmake
    NAMESPACE tca9535::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/tca9535
)

include(CMakePackageConfigHelpers)
configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/tca9535Config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/tca9535Config.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/tca9535
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/tca9535Config.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/tca9535
)

