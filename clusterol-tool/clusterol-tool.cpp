#include <iostream>
#include <string>
#include <boost/program_options.hpp>

#include "clusterol/dissimilarity.hpp"
#include "clusterol/lance_williams.hpp"
#include "clusterol/matrix_based.hpp"


int main(int argc, char *argv[]){

  namespace po = boost::program_options;
  po::options_description desc("Hierarchical Clustering with clusterol");

  desc.add_options()
    ("help", "produce help message\n")
    ;

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);

  if(vm.count("help")){
    std::cout << desc << "\n";
    exit(0);
  }

  
  


  return 0;
}
