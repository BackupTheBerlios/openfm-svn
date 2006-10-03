PROJECT (OpenFM C)
SET (VERSION "0.5")


OPTION (NLS "Enable localization support" ON)
IF (NLS)
    MESSAGE ("Localization: enabled")
ELSE (NLS)
    MESSAGE ("Localization: disabled")
ENDIF (NLS)


SET (CMAKE_C_FLAGS "-W -Wall -Werror -pipe")
OPTION (DEBUG "Enable debug mode" OFF)
IF (NOT DEBUG)
    SET (CMAKE_BUILD_TYPE Release)
    SET (CMAKE_C_FLAGS_RELEASE "-O2")
    MESSAGE ("Debug mode  : disabled")
    MESSAGE ("Build flags : ${CMAKE_C_FLAGS} ${CMAKE_C_FLAGS_RELEASE}")
ELSE (NOT DEBUG)
    SET (CMAKE_BUILD_TYPE Debug)
    SET (CMAKE_C_FLAGS_DEBUG "-O0 -g")
    MESSAGE ("Debug mode  : enabled")
    MESSAGE ("Build flags : ${CMAKE_C_FLAGS} ${CMAKE_C_FLAGS_DEBUG}")
ENDIF (NOT DEBUG)


CONFIGURE_FILE (
    ${CMAKE_SOURCE_DIR}/config.h.cmake
    ${CMAKE_BINARY_DIR}/config.h)

ENABLE_TESTING ()

SUBDIRS (src tests)
