cmake_minimum_required(VERSION 3.24)

include(${CMAKE_CURRENT_BINARY_DIR}/local_conf.cmake OPTIONAL)

include(FetchContent)
FetchContent_Declare(
	ccommon
	GIT_REPOSITORY https://github.com/EddyTheCo/Common.git
	GIT_TAG v0.3.0
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

	qt_standard_project_setup()
        add_executable(qmlLintBot  src/qmllintBot.cpp
            include/gitHClient.hpp include/qmlReviewer.hpp
            src/gitHClient.cpp src/qmlReviewer.cpp
        )
        target_include_directories(qmlLintBot PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

	target_link_libraries(qmlLintBot PRIVATE Qt6::Core Qt6::Network)
	set_target_properties(qmlLintBot PROPERTIES
		WIN32_EXECUTABLE ON
		MACOSX_BUNDLE ON
	)
	install(TARGETS qmlLintBot COMPONENT qmlLintBot
		BUNDLE DESTINATION . COMPONENT qmlLintBot
	)
	if(QTDEPLOY)
		qt_generate_deploy_app_script(
			TARGET qmlLintBot
			OUTPUT_SCRIPT deploy_script
		)
		install(SCRIPT ${deploy_script} COMPONENT qmlLintBot)
	endif(QTDEPLOY)
	build_cpack()
	add_subdirectory(examples)
endif(Qt6_FOUND)
if(BUILD_DOCS)
	build_docs()
endif(BUILD_DOCS)
