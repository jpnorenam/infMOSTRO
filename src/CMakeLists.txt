# copyright 2020 Universidad Nacional de Colombia,
# Laboratorio de Gestión de Sistemas en Tiempo Real
# 
# Licencia,
# otorgada a Secretaría de Movilidad de Medellín
###################################################################################
set(INFMOSTRO_SRC
    mostro.cpp
)
list(APPEND INFMOSTRO_INCLUDE_DIRS
    ${PROJECT_SOURCE_DIR}/include
)
list(APPEND LIBRARIES_INCLUDE_DIRS
    ${Jsoncpp_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
)
set(INFMOSTRO_LIBRARIES
    jsoncpp
    stdc++
)

if(CMAKE_CXX_COMPILER_ID STREQUAL GNU)
	list(APPEND INFMOSTRO_LIBRARIES "-lstdc++fs")
endif()

add_library(infmostro SHARED ${INFMOSTRO_SRC})
target_link_libraries(infmostro PUBLIC ${INFMOSTRO_LIBRARIES})# -Wl,--whole-archive ${WHOLE_ARCHIVES} -Wl,--no-whole-archive)
target_include_directories(infmostro PUBLIC ${INFMOSTRO_INCLUDE_DIRS} ${LIBRARIES_INCLUDE_DIRS})
target_compile_options(infmostro PUBLIC ${INFMOSTRO_CXX_FLAGS})

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

set_target_properties(infmostro PROPERTIES
    #PUBLIC_HEADER ${INFMOSTRO_INCLUDE_DIRS}/infmostro.h
    VERSION ${CMAKE_PROJECT_VERSION}
    SOVERSION 1
)

install(TARGETS infmostro
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    #PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/infmostro DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        FILES_MATCHING PATTERN "*.h"
)
install(DIRECTORY ${PROJECT_SOURCE_DIR}/data/infmostro DESTINATION ${CMAKE_INSTALL_SYSCONFDIR}
        FILES_MATCHING PATTERN "*"
)
set(ENV{INFMOSTRO_HOME} ${CMAKE_INSTALL_SYSCONFDIR}/infmostro)