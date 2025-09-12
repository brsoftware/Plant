cmake_minimum_required(VERSION 3.16)

project(Plant VERSION 0.8.1 LANGUAGES C)

set(PLANT_OS "")

if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    set(PLANT_OS "win32")
elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
    set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64" CACHE STRING "macOS Architectures" FORCE)
    set(PLANT_OS "darwin")
elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    set(PLANT_OS "linux")
else ()
    set(PLANT_OS "others")
endif (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")

set(EXECUTABLE_OUTPUT_PATH "${CMAKE_CURRENT_LIST_DIR}/bin/${PLANT_OS}")

execute_process(
    COMMAND git log -1 --pretty=format:%h
    OUTPUT_VARIABLE PLGITHASH
    OUTPUT_STRIP_TRAILING_WHITESPACE
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
)

message(STATUS "Building Plant at git ${PLGITHASH}")

configure_file(
    ${CMAKE_CURRENT_LIST_DIR}/PL_GIT_HASH.h.in
    ${CMAKE_CURRENT_LIST_DIR}/src/plgithash.h
    @ONLY
)

set(PLANT_SOURCES "")

include_directories(${CMAKE_CURRENT_LIST_DIR}/src)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/src)

add_executable(Plant
    ${PLANT_SOURCES}
)

if (${PLANT_OS} STREQUAL "linux")
    target_link_libraries(Plant PRIVATE m)
endif (${PLANT_OS} STREQUAL "linux")

set_target_properties(Plant
    PROPERTIES OUTPUT_NAME "plant"
)

if (${PLANT_OS} STREQUAL "darwin")
    set_target_properties(Plant
        PROPERTIES OUTPUT_NAME "plant-darwin"
    )
endif (${PLANT_OS} STREQUAL "darwin")

include(GNUInstallDirs)
install(TARGETS Plant
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
