add_executable(minifs-console main.c)
target_link_libraries(minifs-console PUBLIC minifs-core)
target_include_directories(minifs-console PUBLIC . ${PROJECT_SOURCE_DIR}/core/include)
