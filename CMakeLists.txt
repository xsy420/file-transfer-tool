cmake_minimum_required(VERSION 3.25)
project(server C)
project(client C)

set(CMAKE_C_STANDARD 11)

IF (CMAKE_SYSTEM_NAME MATCHES "Linux")
    MESSAGE(STATUS "current platform: Linux")
    add_executable(server-linux server.c util.c util.h)
    add_executable(client-linux client.c util.c util.h)
ELSEIF (CMAKE_SYSTEM_NAME MATCHES "Windows")
    MESSAGE(STATUS "current platform: Windows")
    add_executable(server-windows server.c util.c util.h)
    add_executable(client-windows client.c util.c util.h)
    TARGET_LINK_LIBRARIES(server-windows ws2_32)
    TARGET_LINK_LIBRARIES(client-windows ws2_32)
ELSE ()
    MESSAGE(STATUS "other platform: ${CMAKE_SYSTEM_NAME}")
ENDIF (CMAKE_SYSTEM_NAME MATCHES "Linux")