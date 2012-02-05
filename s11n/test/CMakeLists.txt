add_executable(s11n-test-api test-api.cpp)

target_link_libraries(s11n-test-api 
	${UMUNDO_LIBRARIES}
	umundoserial
)
