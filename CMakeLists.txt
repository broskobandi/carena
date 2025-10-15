# Project

cmake_minimum_required(VERSION 3.20)
project(carena)

# Global options

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(SRC_DIR "${CMAKE_SOURCE_DIR}/src")
set(INC_DIR "${CMAKE_SOURCE_DIR}/include")
set(TEST_DIR "${CMAKE_SOURCE_DIR}/test")
set(EXAMPLE_DIR "${CMAKE_SOURCE_DIR}/example")
set(SRC "${SRC_DIR}/${PROJECT_NAME}.c")
set(INC "${INC_DIR}/${PROJECT_NAME}.h")
set(TEST_MAIN ${TEST_DIR}/test.c)
set(EXAMPLE_MAIN ${EXAMPLE_DIR}/example.c)
set(LIB_SH "${PROJECT_NAME}")
set(LIB_ST "${PROJECT_NAME}-static")

# Targets

add_library(${LIB_SH} SHARED ${SRC})
add_library(${LIB_ST} STATIC ${SRC})
add_executable(example EXCLUDE_FROM_ALL ${EXAMPLE_MAIN})
add_executable(test EXCLUDE_FROM_ALL ${TEST_MAIN} ${SRC})

# Target options

set_target_properties(${LIB_ST} PROPERTIES OUTPUT_NAME "${PROJECT_NAME}")
install(TARGETS ${LIB_SH} LIBRARY DESTINATION lib)
install(TARGETS ${LIB_ST} ARCHIVE DESTINATION lib)
install(FILES ${INC} DESTINATION include)
target_link_libraries(example PRIVATE "${PROJECT_NAME}")
target_link_libraries(test PRIVATE ctest)
target_include_directories(test PRIVATE ${INC_DIR} ${SRC_DIR})
target_include_directories(${LIB_SH} PRIVATE ${INC_DIR})
target_include_directories(${LIB_ST} PRIVATE ${INC_DIR})

# Compiler options

if (CMAKE_C_COMPILER_ID MATCHES "GNU|Clang|AppleClang")
	target_compile_options(test PRIVATE -Wall -Wextra -Werror -Wunused-result -Wconversion)
	target_compile_options(example PRIVATE -Wall -Wextra -Werror -Wunused-result -Wconversion)
	target_compile_options(${LIB_SH} PRIVATE -O3 -march=native -flto)
	target_compile_options(${LIB_ST} PRIVATE -O3 -march=native -flto)
endif ()
