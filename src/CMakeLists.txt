file(GLOB_RECURSE SOURCE_FILES . *.cpp)
file(GLOB_RECURSE HEADER_FILES . *.h)
list(FILTER SOURCE_FILES EXCLUDE REGEX "(client|server).cpp$")

add_executable(SvrLogger server.cpp ${HEADER_FILES} ${SOURCE_FILES})
add_executable(LoggerTerminal client.cpp ${HEADER_FILES} ${SOURCE_FILES})
add_library(${PROJECT_NAME} SHARED ${HEADER_FILES} ${SOURCE_FILES})