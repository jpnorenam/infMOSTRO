include(FindPackageHandleStandardArgs)

find_path(JSONCPP_INCLUDE_DIR
	NAMES jsoncpp_version.h
	PATH_SUFFIXES jsoncpp
)

find_library(JSONCPP_LIBRARY NAMES boost)

# handle the QUIETLY and REQUIRED arguments and set SUNDIALS_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(JsonCpp DEFAULT_MSG JSONCPP_LIBRARY JSONCPP_INCLUDE_DIR)
mark_as_advanced(JSONCPP_INCLUDE_DIR)