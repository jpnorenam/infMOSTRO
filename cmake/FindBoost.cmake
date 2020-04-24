include(FindPackageHandleStandardArgs)

find_path(BOOST_INCLUDE_DIR
	NAMES boost_version.h
	PATH_SUFFIXES boost
)

find_library(BOOST_LIBRARY NAMES boost)

# handle the QUIETLY and REQUIRED arguments and set SUNDIALS_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(Boost DEFAULT_MSG BOOST_LIBRARY BOOST_INCLUDE_DIR)
mark_as_advanced(BOOST_INCLUDE_DIR)