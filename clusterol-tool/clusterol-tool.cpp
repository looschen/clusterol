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
#include "input_output.hpp"


int main(int argc, char *argv[]){

  std::string data_point_filename, label_filename, clustering_method, graph_type, graph_filename, join_filename;
    
  namespace po = boost::program_options;
  po::options_description desc("Hierarchical Clustering with clusterol");

    desc.add_options()
      ("help", "produce help message\n")
      ("data-point-file,d", po::value(&data_point_filename), "file containing the data points")
      // currently labels 1..N are used by default
      // ("label-file,l", po::value(&label_filename), "file containing labels")
      ("method,m", po::value(&clustering_method)->default_value("single"), "available methods are \"single\"") // and many more
      ("graph-type", po::value(&graph_type)->default_value("graphviz"), "available types are \"graphviz\"") // and \"graphml\"
      ("graph-file", po::value(&graph_filename), "write graph in specified format to this file")
      ("join-file", po::value(&join_filename)->default_value("-"), "put information about mergers/joins here")
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


  // open output files
  std::ofstream graph_out, join_out;
  try{
    if(!graph_filename.empty())
      open_outfile(graph_filename, graph_out);
    if(!join_filename.empty())		// always open, suppress with ""
      open_outfile(join_filename, join_out);
  }catch(std::exception& e){
    std::cerr << "An Error occured during output: \n"
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
    data_set = lines_to_data_points(line);
  }catch(std::exception& e){
    std::cerr << "An Error occured during input: \n"
	      << e.what() << "\n";
    exit(1);
  }

  // // test output
  // for(std::vector<data_point>::iterator i = data_set.begin(); i != data_set.end(); ++i){
  //   for(data_point::iterator j = i->begin(); j != i->end(); ++j)
  //     std::cout << *j << " ";
  //   std::cout << "\n";
  // }


  // cluster, preliminary testing implementation
  clusterol::lance_williams_generic lw(0.5, 0.5, 0, -0.5); // single link 
  // lance_williams lw(0.5, 0.5, 0, 0.5);	// complete link
  // lance_williams lw(0.5, 0.5, 0, 0);	// weighted group average
  // lance_williams lw(0.5, 0.5, -0.25, 0);// Median (weighted centroid

  boost::adjacency_list<> dendrogram;
  typedef boost::graph_traits< boost::adjacency_list<> >::vertex_descriptor vertex_descriptor;
  vertex_descriptor root;
  std::vector<double> height(2*data_set.size() - 1);
  clusterol::matrix_clustering
    (data_set.begin(), data_set.end(),
     dendrogram, root, &height[0],
     clusterol::dissimilarity_be<clusterol::euclidean_distance>(),
     lw);

  if(vm.count("graph-file"))
    boost::write_graphviz(graph_out, dendrogram, boost::make_label_writer(&height[0]));

  if(vm.count("join-file")){
    typedef clusterol::join_report_entry<double, vertex_descriptor> join_report_entry;
    std::vector<join_report_entry> join_report = clusterol::get_join_report<join_report_entry>(dendrogram, &height[0]);
    sort(join_report.begin(), join_report.end());
    
    for(std::vector<join_report_entry>::iterator i = join_report.begin(); i != join_report.end(); ++i)
      join_out << clusterol::vertex_descriptor_to_R(i->pair.first, data_set.size())
	       << " " << clusterol::vertex_descriptor_to_R(i->pair.second, data_set.size())
	       << " " << std::setprecision(15) << i->height
	       << "\n";
  }
  
  return 0;
}
