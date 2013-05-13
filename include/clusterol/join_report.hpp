#ifndef _CLUSTEROL_JOIN_REPORT_H_
#define _CLUSTEROL_JOIN_REPORT_H_

#include <boost/graph/adjacency_list.hpp>
#include <boost/version.hpp>
#if BOOST_VERSION >= 104100
#include <boost/property_map/property_map.hpp>
#else
#include <boost/property_map.hpp>
#endif
#include <vector>


// Report joined pairs and heights in a simple vector.

namespace clusterol{

  template <typename height_type, typename vertex_descriptor>
  struct join_report_entry{
    vertex_descriptor vertex;
    std::pair<vertex_descriptor, vertex_descriptor> pair;
    height_type height;

    bool operator<(const join_report_entry<height_type, vertex_descriptor>& other) const{
      // Compare by vertex
      // This works for non-ultrametric clustering methods.
      return this->vertex < other.vertex;
    }
  };


  template <typename join_report_entry_type, typename tree_type, typename property_map>
  std::vector<join_report_entry_type>
  get_join_report(const tree_type& tree, property_map height){
    // get vector for N-1 inner vertices
    // zero error checking, will fail miserably on malformed tree
    
    std::vector<join_report_entry_type> join_report;
    join_report.reserve(num_vertices(tree) - 1);
    
    typename boost::graph_traits<tree_type>::vertex_iterator vi, vi_end;
    for(boost::tie(vi, vi_end) = vertices(tree); vi != vi_end; ++vi){
      typename boost::graph_traits<tree_type>::out_edge_iterator oi, oi_end;
      boost::tie(oi, oi_end) = boost::out_edges(*vi, tree);
      if(std::distance(oi, oi_end)){
	// This is an inner vertex, write data to join_report
	typename boost::graph_traits<tree_type>::vertex_descriptor v_left, v_right;
	v_left = boost::target(*oi, tree);
	++oi;
	v_right = boost::target(*oi, tree);
	join_report.push_back((join_report_entry_type) {*vi, std::make_pair(v_left, v_right), height[*vi]}); 
      }
    }
    
    return join_report;
  }


  template <typename vertex_descriptor>
  int vertex_descriptor_to_R(vertex_descriptor v, size_t n_data_point){
    // convert a numeric vertex_descriptor to R-style descriptor as in hclust$merge
    // i.e.:
    // k<0 denotes a data point / leaf
    // k>0 denotes a cluster / inner vertex
    // counting starts at 1.

    v++;
    if(v <= n_data_point)
      return -int(v);
    else
      return int(v) - n_data_point;
  }
  
}

#endif /* _CLUSTEROL_JOIN_REPORT_H_ */
