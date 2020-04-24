set(LIBRARIES "infmostro")

set(TEST_SRC
	main.cpp
)

foreach(SOURCE ${TEST_SRC})
	get_filename_component(TARGET ${SOURCE} NAME_WE)

	add_executable(${TARGET} ${SOURCE})

	target_link_libraries(${TARGET} ${LIBRARIES})
	target_include_directories(${TARGET} PRIVATE ${INCLUDE_DIRS})
	target_compile_options(${TARGET} PUBLIC ${INFMOSTRO_CXX_FLAGS})
endforeach()
