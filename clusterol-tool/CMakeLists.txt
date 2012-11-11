add_executable("clusterol-tool" clusterol-tool.cpp input_output.cpp input_output.hpp)
target_link_libraries("clusterol-tool" boost_program_options)

install(TARGETS "clusterol-tool" DESTINATION bin)