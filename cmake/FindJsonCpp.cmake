find_path(Jsoncpp_INCLUDE_DIR
	NAMES json/features.h
	PATH_SUFFIXES jsoncpp
	PATHS ${Jsoncpp_PKGCONF_INCLUDE_DIRS}
)

find_library(Jsoncpp_LIBRARY
	NAMES jsoncpp
)

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(Jsoncpp DEFAULT_MSG
    Jsoncpp_LIBRARY Jsoncpp_INCLUDE_DIR)

mark_as_advanced(Jsoncpp_INCLUDE_DIR Jsoncpp_LIBRARY)

set(JSONCPP_LIBRARIES ${Jsoncpp_LIBRARY})
set(JSONCPP_INCLUDE_DIRS ${Jsoncpp_INCLUDE_DIR})
