add_executable(minifs-server main.c answers.c)
target_link_libraries(minifs-server PUBLIC minifs-core picohttpparser)
target_include_directories(minifs-server PUBLIC . ${PROJECT_SOURCE_DIR}/core/include ${PROJECT_SOURCE_DIR}/contrib/picohttpparser)
