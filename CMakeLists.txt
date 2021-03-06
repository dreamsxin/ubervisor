#
# Copyright (c) 2011-2013 Kilian Klimek <kilian.klimek@googlemail.com>
# All rights reserved.
# 
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 
#   1. Redistributions of source code must retain the above copyright
#      notice, this list of conditions and the following disclaimer.
# 
#   2. Redistributions in binary form must reproduce the above copyright
#      notice, this list of conditions and the following disclaimer in the
#      documentation and/or other materials provided with the distribution.
# 
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER AND CONTRIBUTORS
# ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT
# HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
CMAKE_POLICY(SET CMP0005 OLD)

SET(VERSION_MAJOR 0)
SET(VERSION_MINOR 0)
SET(VERSION_BUILD 3-dev)
IF(NOT IS_DIRECTORY ${CMAKE_SOURCE_DIR}/.git)
	SET(VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_BUILD}")
ELSE()
	EXEC_PROGRAM(git ${CMAKE_CURRENT_SOURCE_DIR} ARGS rev-parse --short -q HEAD
		OUTPUT_VARIABLE COMMIT)
	SET(VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_BUILD}-${COMMIT}")
ENDIF()

PROJECT(ubervisor C)
INCLUDE(CheckIncludeFiles)
INCLUDE(CheckLibraryExists)
INCLUDE (CheckFunctionExists)
SET(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

FIND_PACKAGE(Event REQUIRED)
FIND_PACKAGE(jsonc REQUIRED)

ADD_DEFINITIONS("-DUV_VERSION=\\\"${VERSION}\\\"")

IF(CMAKE_BUILD_TYPE STREQUAL "")
	set (CMAKE_BUILD_TYPE Release CACHE STRING "build types: Debug Release." FORCE)
ENDIF()

SET (CMAKE_C_FLAGS "")
SET (CMAKE_C_FLAGS_RELEASE "-Wall -O2 -DNDEBUG")
SET (CMAKE_C_FLAGS_DEBUG "-Wall -Wextra -g -ggdb -DDEBUG -Wunused -Wcast-align -Wcast-qual -Wchar-subscripts -Wnested-externs -Wpointer-arith -Wbad-function-cast -Wredundant-decls -Wshadow -Wstrict-prototypes -Wwrite-strings")

IF(CLANG_ASAN)
	ADD_DEFINITIONS("-faddress-sanitizer")
ENDIF(CLANG_ASAN)

IF(CLANG_ANALYZE)
	ADD_DEFINITIONS("--analyze")
ENDIF(CLANG_ANALYZE)

CHECK_FUNCTION_EXISTS(setproctitle HAVE_SETPROCTITLE)

SET(INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

IF(MAN_INSTALL_DIR)
ELSE(MAN_INSTALL_DIR)
	SET(MAN_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/man/man1)
ENDIF(MAN_INSTALL_DIR)

MESSAGE(STATUS "mandir: " ${MAN_INSTALL_DIR})
FIND_PROGRAM(SPHINX_BUILD sphinx-build)
IF(SPHINX_BUILD)
	MESSAGE(STATUS "sphinx-build: " ${SPHINX_BUILD})
	ADD_CUSTOM_TARGET(man sphinx-build -b man ${CMAKE_CURRENT_SOURCE_DIR}/doc/source ${CMAKE_CURRENT_BINARY_DIR}/doc/build/man)
	ADD_CUSTOM_TARGET(htmldoc sphinx-build -b html "${CMAKE_CURRENT_SOURCE_DIR}/doc/source" ${CMAKE_CURRENT_BINARY_DIR}/doc/build/html)
	INSTALL(CODE "FILE(GLOB MANPAGES \"${CMAKE_CURRENT_BINARY_DIR}/doc/build/man/*.1\")"
		CODE "FILE(INSTALL \${MANPAGES} DESTINATION \"${MAN_INSTALL_DIR}\")")
ELSE(SPHINX_BUILD)
	MESSAGE(STATUS "sphinx-build not found. Documentation cannot be build.")
	ADD_CUSTOM_TARGET(man echo sphinx-build was not found by cmake)
	ADD_CUSTOM_TARGET(htmldoc echo sphinx-build was not found by cmake)
ENDIF(SPHINX_BUILD)

CONFIGURE_FILE(cmake-config.h.in
	${PROJECT_BINARY_DIR}/config.h
	@ONLY)

INCLUDE_DIRECTORIES(.
	${PROJECT_BINARY_DIR}
	${EVENT_INCLUDE_DIR}
	${JSON_INCLUDE_DIR})

ADD_EXECUTABLE(ubervisor
	child_config.c client.c cmd_start.c cmd_update.c main.c misc.c cmd_server.c
	cmd_get.c cmd_proxy.c subscription.c cmd_subscribe.c process.c uvhash.c
	cmd_read.c cmd_exit.c cmd_pids.c cmd_list.c cmd_dump.c cmd_delete.c
	cmd_kill.c)

TARGET_LINK_LIBRARIES(ubervisor
	${EVENT_LIBRARIES}
	${JSON_LIBRARIES})

INSTALL(TARGETS ubervisor DESTINATION bin)
INSTALL(PROGRAMS ubervisor-all DESTINATION bin)
INSTALL(DIRECTORY fatal/
	DESTINATION share/ubervisor/fatal
	FILES_MATCHING PATTERN "*"
	PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)

INSTALL(DIRECTORY scripts/
	DESTINATION share/ubervisor/scripts
	FILES_MATCHING PATTERN "*"
	PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)

INSTALL(DIRECTORY commands/
	DESTINATION share/ubervisor/commands
	FILES_MATCHING PATTERN "*"
	PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)

# building .debs
SET(PACKAGE_STRING "ubervisor-${VERSION}")
SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "Kilian Klimek")
SET(CPACK_DEBIAN_PACKAGE_DEPENDS "libevent-1.4-2, libjson0")

SET(CPACK_GENERATOR "TGZ;RPM;DEB")
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "process control daemon")
SET(CPACK_PACKAGE_INSTALL_DIRECTORY "/usr/local")
SET(CPACK_PACKAGING_INSTALL_PREFIX "/usr/local")
SET(CPACK_PACKAGE_NAME "ubervisor")
SET(CPACK_PACKAGE_VENDOR "Kilian Klimek")
SET(CPACK_PACKAGE_VERSION_MAJOR ${VERSION_MAJOR})
SET(CPACK_PACKAGE_VERSION_MINOR ${VERSION_MINOR})
SET(CPACK_PACKAGE_VERSION_PATCH ${VERSION_BUILD})
SET(CPACK_SOURCE_GENERATOR "TGZ")
SET(CPACK_SOURCE_PACKAGE_FILE_NAME "ubervisor-src")
SET(CPACK_SYSTEM_NAME ${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR})

INCLUDE(CPack)
