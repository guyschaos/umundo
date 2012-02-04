add_executable(test-api test-api.cpp)

target_link_libraries(test-api 
	${UMUNDO_LIBRARIES}
	umundoserial
)
