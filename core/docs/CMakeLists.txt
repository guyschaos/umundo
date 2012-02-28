if (UNIX)
	ADD_CUSTOM_TARGET(
		build-docs 
		doxygen 
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} 
		COMMENT generate the documentation in the source directory using \"make docs\" - doxygen is required)

	ADD_CUSTOM_TARGET(
	    docs
		bash -c "find . -name *.png -exec convert {} -colors 16 -depth 4 {} \;"
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
		COMMENT "Decreasing quality of images in documentation"
		VERBATIM
	)
	add_dependencies(docs build-docs)
else()
	ADD_CUSTOM_TARGET(
		docs 
		doxygen 
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} 
		COMMENT generate the documentation in the source directory using \"make docs\" - doxygen is required)

endif()
