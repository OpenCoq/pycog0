# Install packageconfig
INSTALL(FILES
	"atomspace.pc.in"
	DESTINATION
	${DATADIR}/pkgconfig
	RENAME atomspace.pc)

set(INCLUDE_INSTALL_DIR include/ )
set(LIB_INSTALL_DIR lib/ )

set(ATOMSPACE_CMAKE_DIR lib/cmake/AtomSpace)

include(CMakePackageConfigHelpers)
configure_package_config_file(AtomSpaceConfig.cmake.in
	${CMAKE_CURRENT_BINARY_DIR}/AtomSpaceConfig.cmake
	INSTALL_DESTINATION ${ATOMSPACE_CMAKE_DIR}
	PATH_VARS INCLUDE_INSTALL_DIR LIB_INSTALL_DIR)

write_basic_package_version_file(
	"${CMAKE_CURRENT_BINARY_DIR}/AtomSpaceConfigVersion.cmake"
	VERSION ${SEMANTIC_VERSION}
	COMPATIBILITY SameMajorVersion
)

INSTALL(FILES
	${CMAKE_CURRENT_BINARY_DIR}/AtomSpaceConfigVersion.cmake
	${CMAKE_CURRENT_BINARY_DIR}/AtomSpaceConfig.cmake
	DESTINATION
	${ATOMSPACE_CMAKE_DIR})
