cmake_minimum_required(VERSION 3.20)

project(SeeMusic)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(UNIX AND NOT APPLE)
	find_program(PACMAN_EXECUTABLE pacman)

	if(NOT PACMAN_EXECUTABLE)
		message(FATAL_ERROR "pacman was not found. This dependency installer currently supports Arch Linux.")
	endif()

	set(PACKAGES
		base-devel
		pkgconf
		sdl2
		sdl2-compat
		glew
		mesa
		libglvnd
		nlohmann-json
		ffmpeg
		util-linux
		autoconf
		autoconf-archive
		automake
		libtool
	)

	foreach(PACKAGE ${PACKAGES})
		execute_process(
			COMMAND "${PACMAN_EXECUTABLE}" -Q ${PACKAGE}
			RESULT_VARIABLE PACKAGE_RESULT
			OUTPUT_QUIET
			ERROR_QUIET
		)

		if(NOT PACKAGE_RESULT EQUAL 0)
			message(STATUS "Installing missing dependency: ${PACKAGE}")

			execute_process(
				COMMAND sudo "${PACMAN_EXECUTABLE}" -S --needed --noconfirm ${PACKAGE}
				RESULT_VARIABLE INSTALL_RESULT
			)

			if(NOT INSTALL_RESULT EQUAL 0)
				message(FATAL_ERROR "Failed to install ${PACKAGE}")
			endif()
		endif()
	endforeach()
endif()

find_package(PkgConfig REQUIRED)
find_package(SDL2 CONFIG REQUIRED)
find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

pkg_check_modules(FFMPEG REQUIRED
	libavformat
	libavcodec
	libswresample
	libavutil
)

pkg_check_modules(UUID REQUIRED uuid)

file(GLOB_RECURSE SOURCE_FILES
	"backend/*.hpp"
	"backend/*.cpp"
	"include/../*.hpp"
	"Main.cpp"
	"frontend/*.cpp"
	"frontend/*.hpp"
)

add_executable(SeeMusic ${SOURCE_FILES})

target_include_directories(SeeMusic
	PRIVATE
		backend
		.env
		${FFMPEG_INCLUDE_DIRS}
		${UUID_INCLUDE_DIRS}
)

target_link_directories(SeeMusic
	PRIVATE
		${FFMPEG_LIBRARY_DIRS}
		${UUID_LIBRARY_DIRS}
)

target_link_libraries(SeeMusic
	PRIVATE
		SDL2::SDL2
		GLEW::GLEW
		OpenGL::GL
		nlohmann_json::nlohmann_json
		${FFMPEG_LIBRARIES}
		${UUID_LIBRARIES}
)