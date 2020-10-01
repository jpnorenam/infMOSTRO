find_path(Spdlog_INCLUDE_DIR
	NAMES spdlog/spdlog.h
	PATH_SUFFIXES spdlog
	PATHS ${Spdlog_PKGCONF_INCLUDE_DIRS}
)

find_library(Spdlog_LIBRARY
	NAMES spdlog
)

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(Spdlog DEFAULT_MSG
    Spdlog_LIBRARY Spdlog_INCLUDE_DIR)

mark_as_advanced(Spdlog_INCLUDE_DIR spdlog_LIBRARY)

set(SPDLOG_LIBRARIES ${Spdlog_LIBRARY})
set(SPDLOG_INCLUDE_DIRS ${Spdlog_INCLUDE_DIR})
