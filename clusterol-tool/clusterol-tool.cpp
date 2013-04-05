#include <iostream>
#include <stdexcept>
#include <boost/program_options.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/version.hpp>
#if BOOST_VERSION >= 104100
#include <boost/property_map/property_map.hpp>
#else
#include <boost/property_map.hpp>
#endif

#include "clusterol/join_report.hpp"
#include "clusterol/dissimilarity.hpp"
#include "clusterol/lance_williams.hpp"
#include "clusterol/matrix_based.hpp"
#include "clusterol/cluster.hpp"
#include "input_output.hpp"


int main(int argc, char *argv[]){

  std::string data_point_filename, label_filename, clustering_method, graph_type, graph_filename, join_filename;
  char separator;
  
  namespace po = boost::program_options;
  po::options_description desc("Hierarchical Clustering with clusterol");

  desc.add_options()
    ("help", "produce help message\n")
    ("data-point-file,d", po::value(&data_point_filename), "file containing the data points")
    ("separator", po::value(&separator)->default_value(' '), "separator of values in the data-point file")
    // currently labels 1..N are used by default
    // ("label-file,l", po::value(&label_filename), "file containing labels")
    ("method,m", po::value(&clustering_method)->default_value("single-link"), "available methods are \"single\"") // and many more
    ("graph-type", po::value(&graph_type)->default_value("graphviz"), "available types are \"graphviz\"") // and \"graphml\"
    ("graph-file", po::value(&graph_filename), "write graph in specified format to this file")
    ("join-file", po::value(&join_filename)->default_value("-"), "put information about mergers/joins here")
    ;

  po::variables_map vm;
  try{
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);
  }catch(std::exception& e){
    std::cerr << "An error occured while parsing the options:\n"
	      << e.what() << "\n";
    exit(1);
  }

  if(vm.count("help")){
    std::cout << desc << "\n";
    exit(0);
  }

  // sanity-checks
  if(!vm.count("data-point-file")){
    std::cerr << "No data-point-file given\n";
    exit(1);
  }
  if(graph_type != "graphviz"){
    std::cerr << "Unsupported graph-type: " << graph_type << "\n";
    exit(1);
  }


  // open output files
  std::ofstream graph_out, join_out;
  try{
    if(!graph_filename.empty())
      open_outfile(graph_filename, graph_out);
    if(!join_filename.empty())		// always open, suppress with ""
      open_outfile(join_filename, join_out);
  }catch(std::exception& e){
    std::cerr << "An error occured during output: \n"
	      << e.what() << "\n";
    exit(1);
  }
  
  // Input
  // support only one data_point-type
  typedef std::vector<double> data_point;
  std::vector<std::string> line;
  std::vector<data_point> data_set;

  try{
    line = read_file(data_point_filename);
    data_set = lines_to_data_points(line, separator);
  }catch(std::exception& e){
    std::cerr << "An error occured during input: \n"
	      << e.what() << "\n";
    exit(1);
  }

  // clustering, clusterol::cluster checks if clustering_method is available
  clusterol::dendrogram<> dend = clusterol::cluster<double>(data_set.begin(), data_set.end(), clustering_method,
							    clusterol::dissimilarity_be<clusterol::euclidean_distance>());
  
  if(vm.count("graph-file"))
    boost::write_graphviz(graph_out, dend.tree, boost::make_label_writer(&dend.height[0]));

  if(vm.count("join-file")){
    typedef typename clusterol::dendrogram<>::join_report_entry_type join_report_entry_type;
    std::vector<join_report_entry_type> join_report = clusterol::get_join_report<join_report_entry_type>(dend.tree, &dend.height[0]);
    sort(join_report.begin(), join_report.end());
    
    for(std::vector<join_report_entry_type>::iterator i = join_report.begin(); i != join_report.end(); ++i)
      join_out << clusterol::vertex_descriptor_to_R(i->pair.first, data_set.size())
	       << " " << clusterol::vertex_descriptor_to_R(i->pair.second, data_set.size())
	       << " " << std::setprecision(15) << i->height
	       << "\n";
  }
  
  return 0;
}
