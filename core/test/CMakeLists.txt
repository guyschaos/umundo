add_subdirectory(discovery)

add_executable(umundo-pingpong test-pingpong.cpp)

foreach(LIB ${UMUNDOCORE_LIBRARIES})
	target_link_libraries(umundo-pingpong ${LIB})
endforeach()

foreach(LIB ${UMUNDOCORE_LIBRARIES_DEBUG})
	target_link_libraries(umundo-pingpong debug ${LIB})
endforeach()

install(
	TARGETS umundo-pingpong
	DESTINATION bin
	PERMISSIONS WORLD_EXECUTE OWNER_EXECUTE GROUP_EXECUTE OWNER_WRITE OWNER_READ GROUP_READ WORLD_READ
	COMPONENT applications 
)