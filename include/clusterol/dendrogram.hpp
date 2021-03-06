#ifndef _CLUSTEROL_DENDROGRAM_H_
#define _CLUSTEROL_DENDROGRAM_H_

#include "join_report.hpp"
#include <boost/graph/adjacency_list.hpp>
#include <vector>


// Dendrogram struct

namespace clusterol{

  template <typename height_type = double>
  struct dendrogram{
    typedef boost::adjacency_list<> tree_type;
    typedef boost::graph_traits<tree_type>::vertex_descriptor vertex_descriptor;
    typedef join_report_entry<height_type, vertex_descriptor> join_report_entry_type;

    // convenient constructor
    dendrogram(size_t n_data_point)
      : tree(tree_type(n_data_point)),
	root(0),
	size(2 * n_data_point - 1),
	height(2 * n_data_point - 1)
    {
      // 1 data point in first n clusters
      for(size_t i = 0; i != n_data_point; ++i)
	size[i] = 1;
    }

    // allow default construction
    dendrogram() {}
    

    tree_type tree;
    // mostly unnecessary, since last vertex in tree is (always?) root:
    vertex_descriptor root;	
    std::vector<size_t> size;    // number of data points in a cluster
    std::vector<height_type> height;
  };

}
    
#endif /* _CLUSTEROL_DENDROGRAM_H_ */







