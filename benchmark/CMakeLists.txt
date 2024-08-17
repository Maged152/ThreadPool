# Find all benchmark source files
file(GLOB_RECURSE BENCHMARK_SOURCES *.cpp)

foreach(BENCHMARK_SOURCE ${BENCHMARK_SOURCES})
    get_filename_component(BENCHMARK_NAME ${BENCHMARK_SOURCE} NAME_WE)
    add_executable(${BENCHMARK_NAME} ${BENCHMARK_SOURCE})
    target_link_libraries(${BENCHMARK_NAME} PRIVATE qlm::ThreadPool)
    set_target_properties(${BENCHMARK_NAME} PROPERTIES
                          CXX_STANDARD 20
                          CXX_EXTENSIONS OFF)
    
    add_custom_target(Benchmark_${BENCHMARK_NAME}
        COMMAND ${CMAKE_COMMAND} -E env
                ${CMAKE_CURRENT_BINARY_DIR}/${BENCHMARK_NAME}
        COMMENT "Running ${BENCHMARK_NAME} benchmark"
    )
endforeach()