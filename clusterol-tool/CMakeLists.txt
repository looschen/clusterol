add_executable("clusterol-tool" clusterol-tool.cpp input_output.cpp input_output.hpp)
target_link_libraries("clusterol-tool" ${Boost_LIBRARIES})

install(TARGETS "clusterol-tool" DESTINATION bin)