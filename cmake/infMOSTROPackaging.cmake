set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
set(CPACK_PACKAGE_VENDOR ${PROJECT_AUTHOR})
set(CPACK_PACKAGE_CONTACT "Juan Noreña <jpnorenam@unal.edu.co>")
set(CPACK_PACKAGE_VERSION ${INFMOSTRO_SHORT_VERSION})
set(CPACK_PACKAGE_VERSION_MAJOR ${INFMOSTRO_MAJOR_VERSION})
set(CPACK_PACKAGE_VERSION_MINOR ${INFMOSTRO_MINOR_VERSION})
set(CPACK_PACKAGE_VERSION_PATCH ${INFMOSTRO_PATH_VERSION})
set(CPACK_RPM_PACKAGE_RELEASE ${INFMOSTRO_RELEASE})

set(CPACK_RPM_PACKAGE_VERSION ${INFMOSTRO_VERSION})
set(CPACK_RPM_PACKAGE_RELEASE ${INFMOSTRO_RELEASE})
set(CPACK_RPM_PACKAGE_ARCHITECTURE "x86_64")
set(CPACK_RPM_PACKAGE_LICENSE "GPLv3")
set(CPACK_RPM_PACKAGE_REQUIRES "libjsoncpp-dev libboost-math-dev")
set(CPACK_RPM_PACKAGE_GROUP "Development/Libraries")

# As close as possible to Fedoras naming
set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CPACK_RPM_PACKAGE_RELEASE}.${CPACK_RPM_PACKAGE_ARCHITECTURE}")

set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/COPYING.md")
set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")

if(NOT CPACK_GENERATOR)
    set(CPACK_GENERATOR "RPM;TGZ")
endif()

include(CPack)
