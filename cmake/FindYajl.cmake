find_package(PkgConfig QUIET)

pkg_check_modules(PC_YAJL QUIET yajl)

find_library(YAJL_LIBRARY yajl
  HINTS ${PC_YAJL_LIBDIR} ${PC_YAJL_LIBRARY_DIRS}
)

find_path(YAJL_INCLUDE_DIR yajl/yajl_parse.h
  HINTS ${PC_YAJL_INCLUDEDIR} ${PKGCONFIG_YAJL_INCLUDE_DIRS}
)

set(YAJL_LIBRARIES ${YAJL_LIBRARY})
set(YAJL_INCLUDE_DIRS ${YAJL_INCLUDE_DIR})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Yajl  DEFAULT_MSG
  YAJL_LIBRARY  YAJL_INCLUDE_DIR)
mark_as_advanced(YAJL_INCLUDE_DIR YAJL_LIBRARY )
