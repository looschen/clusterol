#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <boost/program_options.hpp>

#include "clusterol/dissimilarity.hpp"
#include "clusterol/lance_williams.hpp"
#include "clusterol/matrix_based.hpp"
#include "input_output.hpp"


int main(int argc, char *argv[]){

  std::string data_point_filename, label_filename, clustering_method, graph_type;
    
  namespace po = boost::program_options;
  po::options_description desc("Hierarchical Clustering with clusterol");

    desc.add_options()
      ("help", "produce help message\n")
      ("data-point-file,d", po::value(&data_point_filename), "file containing the data points")
      // currently labels 1..N are used by default
      // ("label-file,l", po::value(&label_filename), "file containing labels")
      ("method,m", po::value(&clustering_method)->default_value("single"), "available methods are \"single\"") // and many more
      ("graph-type", po::value(&graph_type)->default_value("graphviz"), "available types are \"graphviz\"") // and \"graphml\"
    ;

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);

  if(vm.count("help")){
    std::cout << desc << "\n";
    exit(0);
  }

  // sanity-checks
  if(!vm.count("data-point-file")){
    std::cerr << "No data-point-file given\n";
    exit(1);
  }
  if(clustering_method != "single"){
    std::cerr << "Unsupported clustering method: " << clustering_method << "\n";
    exit(1);
  }
  if(graph_type != "graphviz"){
    std::cerr << "Unsupported graph-type: " << graph_type << "\n";
    exit(1);
  }

  
  // Input
  // support only one data_point-type
  typedef std::vector<double> data_point;
  std::vector<std::string> line;
  std::vector<data_point> data_set;


  try{
    read_file(data_point_filename, line);
    lines_to_data_points(line, data_set);
  }catch(std::exception& e){
    std::cerr << "An Error occured during Input: \n"
	      << e.what() << "\n";
    exit(1);
  }

  // test output
  for(std::vector<data_point>::iterator i = data_set.begin(); i != data_set.end(); ++i){
    for(data_point::iterator j = i->begin(); j != i->end(); ++j)
      std::cout << *j << " ";
    std::cout << "\n";
  }
  


  return 0;
}
