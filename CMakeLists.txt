cmake_minimum_required(VERSION 3.20)
project(SeeMusic)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
	set(VCPKG_ROOT "${CMAKE_SOURCE_DIR}/vcpkg")
	if(NOT EXISTS "${VCPKG_ROOT}/vcpkg")
		message(STATUS "vcpkg not found, downloading...")
		find_package(Git REQUIRED)
		execute_process(
			COMMAND ${GIT_EXECUTABLE} clone https://github.com/microsoft/vcpkg.git "${VCPKG_ROOT}"
			RESULT_VARIABLE VCPKG_CLONE_RESULT
		)
		if(NOT VCPKG_CLONE_RESULT EQUAL 0)
			message(FATAL_ERROR "Failed to download vcpkg")
		endif()
	endif()
	if(WIN32)
		set(VCPKG_EXECUTABLE "${VCPKG_ROOT}/vcpkg.exe")
	else()
		set(VCPKG_EXECUTABLE "${VCPKG_ROOT}/vcpkg")
	endif()
	if(NOT EXISTS "${VCPKG_EXECUTABLE}")
		message(STATUS "Bootstrapping vcpkg...")
		if(WIN32)
			execute_process(
				COMMAND "${VCPKG_ROOT}/bootstrap-vcpkg.bat"
				WORKING_DIRECTORY "${VCPKG_ROOT}"
				RESULT_VARIABLE VCPKG_BOOTSTRAP_RESULT
			)
		else()
			execute_process(
				COMMAND "${VCPKG_ROOT}/bootstrap-vcpkg.sh"
				WORKING_DIRECTORY "${VCPKG_ROOT}"
				RESULT_VARIABLE VCPKG_BOOTSTRAP_RESULT
			)
		endif()
		if(NOT VCPKG_BOOTSTRAP_RESULT EQUAL 0)
			message(FATAL_ERROR "Failed to bootstrap vcpkg")
		endif()
	endif()
	if(WIN32)
		set(VCPKG_TRIPLET "x64-windows")
	elseif(APPLE)
		set(VCPKG_TRIPLET "x64-osx")
	else()
		set(VCPKG_TRIPLET "x64-linux")
	endif()
	execute_process(
		COMMAND "${VCPKG_EXECUTABLE}" install
		WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
		RESULT_VARIABLE VCPKG_INSTALL_RESULT
	)
	if(NOT VCPKG_INSTALL_RESULT EQUAL 0)
		message(FATAL_ERROR "Failed to install dependencies with vcpkg")
	endif()
	set(CMAKE_TOOLCHAIN_FILE "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE FILEPATH "" FORCE)
	set(VCPKG_TARGET_TRIPLET "${VCPKG_TRIPLET}" CACHE STRING "" FORCE)
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
	"frontend/*cpp"
	"frontend/*hpp"
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