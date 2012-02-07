list(APPEND ALL_LIBRARIES  ${UMUNDOCORE_LIBRARIES})
list(APPEND ALL_LIBRARIES  ${UMUNDOS11N_LIBRARIES})
list(APPEND ALL_LIBRARIES_DEBUG  ${UMUNDOCORE_LIBRARIES_DEBUG})
list(APPEND ALL_LIBRARIES_DEBUG  ${UMUNDOS11N_LIBRARIES_DEBUG})

add_executable(s11n-test-api test-api.cpp)

foreach(LIB ${ALL_LIBRARIES})
	target_link_libraries(s11n-test-api ${LIB})
endforeach()
foreach(LIB ${ALL_LIBRARIES_DEBUG})
	target_link_libraries(s11n-test-api debug ${LIB})
endforeach()

