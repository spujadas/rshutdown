cmake_minimum_required(VERSION 3.16)

project(rshutdown C)

# metadata
set(PROJECT_VERSION 1.0)

# facil.io target (static library)
set(FACIL_DIR "facil")
add_subdirectory(${FACIL_DIR})

# source files
set(SOURCE_FILES_DIR ${PROJECT_SOURCE_DIR}/src)
set(LOG_C_SOURCE_FILES_DIR ${PROJECT_SOURCE_DIR}/log.c/src)
set(PROJECT_SRCS
    ${SOURCE_FILES_DIR}/main.c
    ${LOG_C_SOURCE_FILES_DIR}/log.c
)

# build executable
add_executable(${PROJECT_NAME} ${PROJECT_SRCS})
target_include_directories(${PROJECT_NAME} PUBLIC ${LOG_C_SOURCE_FILES_DIR})
target_link_libraries(${PROJECT_NAME} facil.io)

# install executable
install(TARGETS rshutdown
    RUNTIME DESTINATION bin
)

# install scripts
set(PACKAGING_DIR ${PROJECT_SOURCE_DIR}/packaging)
install(FILES ${PACKAGING_DIR}/rshutdown-default
    DESTINATION /etc/default
    RENAME rshutdown
    PERMISSIONS OWNER_READ OWNER_WRITE)  # restrict permissions as will contain security token
install(PROGRAMS ${PACKAGING_DIR}/rshutdown-init
    DESTINATION /etc/init.d
    RENAME rshutdown)
install(FILES ${PACKAGING_DIR}/rshutdown-sudoers
    DESTINATION /etc/sudoers.d
    RENAME rshutdown
    PERMISSIONS OWNER_READ GROUP_READ)  # proper permissions
install(FILES ${PACKAGING_DIR}/rshutdown-logrotate
    DESTINATION /etc/logrotate.d
    RENAME rshutdown)

# create Makefile target for Debian package
set(CPACK_GENERATOR "DEB")

set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "rshutdown is a lightweight remote shutdown HTTP service for Linux.")
set(PACKAGE_LONG_DESCRIPTION "It is primarily intended for shutting down headless Raspberry Pis cleanly when SSH access is impractical.")
set(CPACK_PACKAGING_INSTALL_PREFIX "/usr")

if(NOT CPACK_DEBIAN_PACKAGE_ARCHITECTURE)  # default if not set by toolchain file
    set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "amd64")
endif()

set(CPACK_DEBIAN_PACKAGE_RELEASE "1")  # revision, i.e. changes in packaging
set(CPACK_DEBIAN_FILE_NAME "DEB-DEFAULT")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Sébastien Pujadas <sebastien@pujadas.net>")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "sudo")
set(CPACK_DEBIAN_PACKAGE_DESCRIPTION
    "${CPACK_PACKAGE_DESCRIPTION}\n ${PACKAGE_LONG_DESCRIPTION}")

set(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA
    "${PACKAGING_DIR}/debian/postinst;${PACKAGING_DIR}/debian/postrm;${PACKAGING_DIR}/debian/prerm")

include(CPack)
