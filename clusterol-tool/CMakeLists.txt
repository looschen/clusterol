add_executable("clusterol-tool" clusterol-tool.cpp)
target_link_libraries("clusterol-tool" boost_program_options)

install(TARGETS "clusterol-tool" DESTINATION bin)