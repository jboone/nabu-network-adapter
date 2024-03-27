find_package(PkgConfig)

PKG_CHECK_MODULES(PC_GR_NABU gnuradio-nabu)

FIND_PATH(
    GR_NABU_INCLUDE_DIRS
    NAMES gnuradio/nabu/api.h
    HINTS $ENV{NABU_DIR}/include
        ${PC_NABU_INCLUDEDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/include
          /usr/local/include
          /usr/include
)

FIND_LIBRARY(
    GR_NABU_LIBRARIES
    NAMES gnuradio-nabu
    HINTS $ENV{NABU_DIR}/lib
        ${PC_NABU_LIBDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/lib
          ${CMAKE_INSTALL_PREFIX}/lib64
          /usr/local/lib
          /usr/local/lib64
          /usr/lib
          /usr/lib64
          )

include("${CMAKE_CURRENT_LIST_DIR}/gnuradio-nabuTarget.cmake")

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(GR_NABU DEFAULT_MSG GR_NABU_LIBRARIES GR_NABU_INCLUDE_DIRS)
MARK_AS_ADVANCED(GR_NABU_LIBRARIES GR_NABU_INCLUDE_DIRS)
