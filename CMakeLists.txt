cmake_minimum_required(VERSION 3.24)

include(${CMAKE_CURRENT_BINARY_DIR}/local_conf.cmake OPTIONAL)

include(FetchContent)
FetchContent_Declare(
	ccommon
	GIT_REPOSITORY https://github.com/EddyTheCo/Common.git
	GIT_TAG v0.4.0
	)
FetchContent_MakeAvailable(ccommon)
version_from_git(
	LOG       OFF
	TIMESTAMP "%Y%m%d%H%M%S"
	)
project(qmllintBot VERSION ${VERSION} DESCRIPTION "create code reviews from qmllint json" LANGUAGES CXX)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
include(GNUInstallDirs)

find_package(Qt6 COMPONENTS Core Network Gui Quick Qml)




if(Qt6_FOUND)
	include(InstallRequiredSystemLibraries)
	FetchContent_Declare(
		YAML-CPP
		GIT_REPOSITORY git@github.com:jbeder/yaml-cpp.git
		GIT_TAG 0.8.0
		FIND_PACKAGE_ARGS CONFIG
		)
	FetchContent_MakeAvailable(YAML-CPP)

	qt_standard_project_setup()
	add_library(gitReviewer src/gitHClient.cpp include/gitHClient.hpp
		src/reviewer.cpp include/reviewer.hpp)
	target_include_directories(gitReviewer PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
		"$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/Esterv/>")
	target_link_libraries(gitReviewer PUBLIC Qt6::Core Qt6::Network)

	add_executable(yamlToJson  src/yamltojson.cpp)

	add_executable(lintBot  src/lintBot.cpp)

	target_link_libraries(yamlToJson PRIVATE yaml-cpp Qt6::Core)

	target_link_libraries(lintBot PRIVATE gitReviewer)


	set_target_properties(lintBot yamlToJson PROPERTIES
		WIN32_EXECUTABLE ON
		MACOSX_BUNDLE ON
		)
	install(IMPORTED_RUNTIME_ARTIFACTS Qt6::QTlsBackendOpenSSLPlugin
		DESTINATION plugins/tls
		COMPONENT opensslBend
		)
	install(TARGETS lintBot COMPONENT lintBot
		BUNDLE DESTINATION . COMPONENT lintBot
		)
	install(TARGETS yamlToJson COMPONENT yamlToJson
		BUNDLE DESTINATION . COMPONENT yamlToJson
		)
	if(QTDEPLOY)
		qt_generate_deploy_app_script(
			TARGET lintBot
			OUTPUT_SCRIPT deploy_script
			)
		install(SCRIPT ${deploy_script} COMPONENT lintBot)
		qt_generate_deploy_app_script(
			TARGET yamlToJson
			OUTPUT_SCRIPT deploy_script
			)
		install(SCRIPT ${deploy_script} COMPONENT yamlToJson)
	endif(QTDEPLOY)
	build_cpack()
	add_subdirectory(examples)
endif(Qt6_FOUND)
if(BUILD_DOCS)
	build_docs()
endif(BUILD_DOCS)
