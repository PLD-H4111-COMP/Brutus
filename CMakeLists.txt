# minimum required CMAKE version
CMAKE_MINIMUM_REQUIRED(VERSION 3.7 FATAL_ERROR)

project(Brutus)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# compiler must be 11 or 14
set(CMAKE_CXX_STANDARD 11)

# required if linking to static library
add_definitions(-DANTLR4CPP_STATIC)

# using /MD flag for antlr4_runtime (for Visual C++ compilers only)
set(ANTLR4_WITH_STATIC_CRT OFF)
# add external build for antlrcpp
include(ExternalAntlr4Cpp)
# add antrl4cpp artifacts to project environment
include_directories(${ANTLR4_INCLUDE_DIRS})

# set variable pointing to the antlr tool that supports C++
# this is not required if the jar file can be found under PATH environment
LIST(APPEND CMAKE_PROGRAM_PATH "/shares/public/tp/ANTLR4-CPP/bin")
#set(ANTLR_EXECUTABLE /usr/bin/antlr4)
# add macros to generate ANTLR Cpp code from grammar
find_package(ANTLR REQUIRED)

# Call macro to add lexer and grammar to your build dependencies.
antlr_target(CProg CProg.g4 VISITOR)

# include generated files in project environment
include_directories(${ANTLR_CProg_OUTPUT_DIR})
# add generated grammar to Brutus binary target
add_executable(Brutus main.cpp CProgCSTVisitor.cpp parserOptions.cpp Writer.cpp
               ${ANTLR_CProg_CXX_OUTPUTS})
target_link_libraries(Brutus antlr4_static)
add_custom_command(TARGET Brutus POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                   ${CMAKE_SOURCE_DIR}/progs $<TARGET_FILE_DIR:Brutus>/progs)

configure_file(scripts/compile.sh ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)
configure_file(scripts/tests.sh ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)

