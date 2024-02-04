project(simple-window-example)

add_executable(${PROJECT_NAME} src/main.cpp)

target_link_libraries(${PROJECT_NAME} aetherium-core-static)
add_dependencies(${PROJECT_NAME} aetherium-core-static)
target_link_libraries(${PROJECT_NAME} aetherium-renderer-static)
add_dependencies(${PROJECT_NAME} aetherium-renderer-static)