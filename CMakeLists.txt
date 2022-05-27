cmake_minimum_required(VERSION 3.9)

project( qmqtt VERSION 1.0.2 )

include( GNUInstallDirs ) # needed to define vars used in install() directives.


# ===================================================================
# Configurable options

option( ${PROJECT_NAME}_SHARED "Build a shared library.  Turn off for static." ON )
option( ${PROJECT_NAME}_WEBSOCKETS "Enable WebSockets for MQTT" OFF )
option( ${PROJECT_NAME}_SSL "Enable SSL support for MQTT" ON )


if ( ${PROJECT_NAME}_SHARED )
    set( library_build_type SHARED )
    set( library_install_component Library )
else()
    set( library_build_type STATIC )
    set( library_install_component Devel )
endif()


set( ws_component )
set( ws_libname )
set( qt5_min_version "5.3.0" )

if ( ${PROJECT_NAME}_WEBSOCKETS )
    set( ws_component WebSockets )
    set( ws_libname   Qt5::WebSockets )
    set( qt5_min_version "5.7.0" )
endif()

if ( NOT ${PROJECT_NAME}_SSL)
    set( ssl_defs QT_NO_SSL )
endif()


find_package( Qt5 ${qt5_min_version} COMPONENTS Core Network ${ws_component} CONFIG REQUIRED )
set( CMAKE_AUTOMOC ON )
cmake_policy( SET CMP0020 NEW ) # Automatically link Qt executables to qtmain target on Windows.


# ===================================================================
# Project files

set( PUBLIC_HEADERS
    src/mqtt/qmqtt_global.h
    src/mqtt/qmqtt.h
    src/mqtt/qmqtt_client.h
    src/mqtt/qmqtt_frame.h
    src/mqtt/qmqtt_message.h
    src/mqtt/qmqtt_routesubscription.h
    src/mqtt/qmqtt_routedmessage.h
    src/mqtt/qmqtt_router.h
    src/mqtt/qmqtt_networkinterface.h
    src/mqtt/qmqtt_socketinterface.h
    src/mqtt/qmqtt_timerinterface.h
)

set( PRIVATE_HEADERS
    src/mqtt/qmqtt_client_p.h
    src/mqtt/qmqtt_message_p.h
    src/mqtt/qmqtt_network_p.h
    src/mqtt/qmqtt_socket_p.h
    src/mqtt/qmqtt_timer_p.h
)

set( SOURCES
    src/mqtt/qmqtt_client_p.cpp
    src/mqtt/qmqtt_client.cpp
    src/mqtt/qmqtt_frame.cpp
    src/mqtt/qmqtt_message.cpp
    src/mqtt/qmqtt_network.cpp
    src/mqtt/qmqtt_routesubscription.cpp
    src/mqtt/qmqtt_router.cpp
    src/mqtt/qmqtt_socket.cpp
    src/mqtt/qmqtt_timer.cpp
)

if ( ${PROJECT_NAME}_WEBSOCKETS )
    list( APPEND PRIVATE_HEADERS
        src/mqtt/qmqtt_websocket_p.h
        src/mqtt/qmqtt_websocketiodevice_p.h
    )
    list( APPEND SOURCES
        src/mqtt/qmqtt_websocket.cpp
        src/mqtt/qmqtt_websocketiodevice.cpp
    )
endif()

if ( ${PROJECT_NAME}_SSL)
    list( APPEND PRIVATE_HEADERS
        src/mqtt/qmqtt_ssl_socket_p.h
    )
    list( APPEND SOURCES
        src/mqtt/qmqtt_ssl_socket.cpp
    )
endif()


# Mark public headers as such
set_source_files_properties( ${PUBLIC_HEADERS} PROPERTIES PUBLIC_HEADER 1 )


# ===================================================================
# Library target

# Library has the same name as the project
add_library( ${PROJECT_NAME} ${library_build_type} ${SOURCES} ${PUBLIC_HEADERS} ${PRIVATE_HEADERS} )
target_link_libraries( ${PROJECT_NAME} PUBLIC Qt5::Core Qt5::Network ${ws_libname} )
target_compile_definitions( ${PROJECT_NAME}
    PRIVATE
        QT_NO_CAST_FROM_ASCII
        QT_NO_CAST_TO_ASCII
        QT_BUILD_QMQTT_LIB
        ${ssl_defs}
)

# Where to look for headers while compiling the target or when compiling against
# the target.
target_include_directories( ${PROJECT_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/mqtt>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

set_target_properties( ${PROJECT_NAME}
    PROPERTIES
        VERSION                   ${PROJECT_VERSION}
        SOVERSION                 ${PROJECT_VERSION_MAJOR}
        CXX_STANDARD              11
        CXX_STANDARD_REQUIRED     OFF  # Whether CXX_STANDARD is enforced
)

if ( ${CMAKE_HOST_WIN32} )
    # On Windows, libraries are not generally prefixed with "lib".
    # If left unchanged, cmake will still add this prefix.
    set_target_properties( ${PROJECT_NAME}
        PROPERTIES
            PREFIX        ""
            IMPORT_PREFIX ""
    )
endif()


# ===================================================================
# Installation

# Rule to install runtime components (ie: the shared library)
install(
    TARGETS   ${PROJECT_NAME}
    EXPORT    ${PROJECT_NAME}
    COMPONENT ${library_install_component}
    RUNTIME   DESTINATION "${CMAKE_INSTALL_BINDIR}"
    LIBRARY   DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    ARCHIVE   DESTINATION "${CMAKE_INSTALL_LIBDIR}"
)

install(
    EXPORT ${PROJECT_NAME}
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}"
    FILE ${PROJECT_NAME}Targets.cmake
    COMPONENT Devel
)


install(
    FILES       ${PUBLIC_HEADERS}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    COMPONENT   Devel
    OPTIONAL
)

# Generate a CMake file into the installation, to easily use the library
install(
    EXPORT      ${PROJECT_NAME}
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}"
    COMPONENT   Devel
    OPTIONAL
)



include( CMakePackageConfigHelpers )

file( WRITE "${CMAKE_CURRENT_BINARY_DIR}/qmqttConfig.cmake.in"
      "@PACKAGE_INIT@\ninclude( \${CMAKE_CURRENT_LIST_DIR}/qmqttTargets.cmake )" )

configure_package_config_file(
    "${CMAKE_CURRENT_BINARY_DIR}/qmqttConfig.cmake.in"
    "qmqttConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
    PATH_VARS           CMAKE_INSTALL_LIBDIR
)

write_basic_package_version_file(
    "qmqttConfigVersion.cmake"
    VERSION       ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(
    FILES
        ${CMAKE_CURRENT_BINARY_DIR}/qmqttConfig.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/qmqttConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
    COMPONENT   Devel
)


# ===================================================================
# Package creation

set( CPACK_PACKAGE_NAME ${PROJECT_NAME} )
set( CPACK_PACKAGE_VENDOR "emqtt" ) # Github project owner
set( CPACK_PACKAGE_DESCRIPTION_SUMMARY "mqtt client for Qt" )
set( CPACK_PACKAGE_HOMEPAGE_URL "https://github.com/emqtt/qmqtt" )
set( CPACK_PACKAGE_CONTACT      "https://github.com/emqtt/qmqtt" )
set( CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR} )
set( CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR} )
set( CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH} )
set( CPACK_PACKAGE_VERSION ${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH} )
set( CPACK_PACKAGE_INSTALL_DIRECTORY ${PROJECT_NAME} )
set( CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/edl-v10" )
set( CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md" )

set( CPACK_COMPONENT_Library_DISPLAY_NAME "QMQTT Library" )
set( CPACK_COMPONENT_Library_DESCRIPTION "The QMQTT binary library." )
set( CPACK_COMPONENT_Library_REQUIRED 1 )
set( CPACK_COMPONENT_Devel_DISPLAY_NAME "QMQTT Development Files" )
set( CPACK_COMPONENT_Devel_DESCRIPTION "Development files for compiling against QMQTT." )
set( CPACK_COMPONENT_Devel_REQUIRED 0 )

if( ${CMAKE_SYSTEM_NAME} STREQUAL "Linux" )

    if ( "${CPACK_PACKAGE_ARCHITECTURE}" STREQUAL "" )
        # Note: the architecture should default to the local architecture, but it
        # in fact comes up empty.  We call `uname -m` to ask the kernel instead.
        EXECUTE_PROCESS( COMMAND uname -m COMMAND tr -d '\n' OUTPUT_VARIABLE CPACK_PACKAGE_ARCHITECTURE )
    endif()

    set( CPACK_INCLUDE_TOPLEVEL_DIRECTORY 1 )
    set( CPACK_PACKAGE_RELEASE 1 )


    # RPM - https://cmake.org/cmake/help/latest/cpack_gen/rpm.html
    set( CPACK_RPM_PACKAGE_RELEASE ${CPACK_PACKAGE_RELEASE} )
    set( CPACK_RPM_PACKAGE_ARCHITECTURE ${CPACK_PACKAGE_ARCHITECTURE} )
    set( CPACK_RPM_PACKAGE_DESCRIPTION ${CPACK_PACKAGE_DESCRIPTION_SUMMARY} )
    set( CPACK_RPM_PACKAGE_URL ${CPACK_PACKAGE_HOMEPAGE_URL} )
    set( CPACK_RPM_PACKAGE_LICENSE "EPL-1 AND EDL-1" )
    set( CPACK_RPM_COMPONENT_INSTALL 1 )
    set( CPACK_RPM_COMPRESSION_TYPE "xz" )
    set( CPACK_RPM_PACKAGE_AUTOPROV 1 )

    set( CPACK_RPM_Library_PACKAGE_SUMMARY ${CPACK_COMPONENT_Library_DESCRIPTION} )
    set( CPACK_RPM_Library_PACKAGE_ARCHITECTURE ${CPACK_RPM_PACKAGE_ARCHITECTURE} )
    set( CPACK_RPM_Library_PACKAGE_NAME ${CPACK_PACKAGE_NAME} )
    set( CPACK_RPM_Library_FILE_NAME "RPM-DEFAULT" )

    set( CPACK_RPM_Devel_PACKAGE_REQUIRES "cmake >= ${CMAKE_MINIMUM_REQUIRED_VERSION},qmqtt >= ${CPACK_PACKAGE_VERSION}" )
    set( CPACK_RPM_Devel_PACKAGE_SUMMARY ${CPACK_COMPONENT_Devel_DESCRIPTION} )
    set( CPACK_RPM_Devel_PACKAGE_ARCHITECTURE ${CPACK_RPM_PACKAGE_ARCHITECTURE} )
    set( CPACK_RPM_Devel_PACKAGE_NAME "${CPACK_PACKAGE_NAME}-devel" )
    set( CPACK_RPM_Devel_FILE_NAME "RPM-DEFAULT" )


    # DEB - https://cmake.org/cmake/help/latest/cpack_gen/deb.html
    set( CPACK_DEBIAN_PACKAGE_RELEASE ${CPACK_PACKAGE_RELEASE} )
    set( CPACK_DEBIAN_PACKAGE_HOMEPAGE ${CPACK_PACKAGE_HOMEPAGE_URL} )
    set( CPACK_DEB_COMPONENT_INSTALL 1 )
    set( CPACK_DEBIAN_COMPRESSION_TYPE "xz")

    if ( ${CPACK_PACKAGE_ARCHITECTURE} STREQUAL "x86_64" )
        set( CPACK_DEBIAN_PACKAGE_ARCHITECTURE "amd64" )  # DEB doesn't always use the kernel's arch name
    else()
        set( CPACK_DEBIAN_PACKAGE_ARCHITECTURE ${CPACK_PACKAGE_ARCHITECTURE} )
    endif()

    set( CPACK_DEBIAN_FILE_NAME "DEB-DEFAULT" ) # Use default naming scheme

    set( CPACK_DEBIAN_LIBRARY_PACKAGE_NAME ${CPACK_PACKAGE_NAME} )
    set( CPACK_DEBIAN_LIBRARY_PACKAGE_SHLIBDEPS 1 )

    set( CPACK_DEBIAN_DEVEL_PACKAGE_DEPENDS "cmake (>= ${CMAKE_MINIMUM_REQUIRED_VERSION}), qmqtt (>= ${CPACK_PACKAGE_VERSION})" )
    set( CPACK_DEBIAN_DEVEL_PACKAGE_NAME "${CPACK_PACKAGE_NAME}-dev" )

elseif( ${CMAKE_HOST_WIN32} )
    set( CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON )
    set( CPACK_NSIS_DISPLAY_NAME ${PROJECT_NAME} )
    set( CPACK_NSIS_PACKAGE_NAME ${PROJECT_NAME} )
    set( CPACK_NSIS_URL_INFO_ABOUT ${CPACK_PACKAGE_HOMEPAGE_URL} )
endif()

# This must always be last!
include(CPack)
