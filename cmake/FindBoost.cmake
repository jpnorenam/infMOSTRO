find_path(Boost_INCLUDE_DIR
	NAMES numeric/ublas/blas.hpp
	PATH_SUFFIXES boost
	PATHS ${Boost_PKGCONF_INCLUDE_DIRS}
)

find_library(Boost_LIBRARY
	NAMES boost_math_c99
)

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(Boost DEFAULT_MSG
    Boost_LIBRARY Boost_INCLUDE_DIR)

mark_as_advanced(Boost_INCLUDE_DIR Boost_LIBRARY)

set(Boost_LIBRARIES ${Boost_LIBRARY})
set(Boost_INCLUDE_DIRS ${Boost_INCLUDE_DIR})
