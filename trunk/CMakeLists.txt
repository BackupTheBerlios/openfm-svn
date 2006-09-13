PROJECT (OpenFM C)
SET (VERSION "0.5")

OPTION (NLS "Enable localization support" ON)
IF (NLS)
    MESSAGE ("Localization is enabled")
ELSE (NLS)
    MESSAGE ("Localization is disabled")
ENDIF (NLS)

CONFIGURE_FILE (
    ${CMAKE_SOURCE_DIR}/config.h.cmake
    ${CMAKE_BINARY_DIR}/config.h)
SUBDIRS (src)
