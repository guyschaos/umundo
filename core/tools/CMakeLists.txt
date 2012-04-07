if (WIN32)
	include_directories(${PROJECT_SOURCE_DIR}/contrib/snippets)
	add_executable(umundo-monitor umundo-monitor.cpp ${PROJECT_SOURCE_DIR}/contrib/snippets/XGetopt.cpp)
else()
	add_executable(umundo-monitor umundo-monitor.cpp)
endif()
target_link_libraries(umundo-monitor ${UMUNDOCORE_LIBRARIES})
set_target_properties(umundo-monitor PROPERTIES FOLDER "Tools")

INSTALL_EXECUTABLE(
	TARGETS umundo-monitor
	COMPONENT tools 
)

add_executable(umundo-pingpong umundo-pingpong.cpp)
target_link_libraries(umundo-pingpong ${UMUNDOCORE_LIBRARIES})
set_target_properties(umundo-pingpong PROPERTIES FOLDER "Tools")

INSTALL_EXECUTABLE(
	TARGETS umundo-pingpong
	COMPONENT tools 
)
