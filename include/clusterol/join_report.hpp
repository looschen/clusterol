#ifndef _JOIN_REPORT_H_
#define _JOIN_REPORT_H_

#include <vector>
#include <boost/version.hpp>
#if BOOST_VERSION >= 104100
#include <boost/property_map/property_map.hpp>
#else
#include <boost/property_map.hpp>
#endif

// Report joined pairs and heights in a simple vector.

namespace clusterol{

  template <typename height_type, typename vertex_descriptor>
  struct join_report_entry{
    std::pair<vertex_descriptor, vertex_descriptor> pair;
    height_type height;

    bool operator<(const join_report_entry<height_type, vertex_descriptor>& other) const{
      // compare by height first, then by pair
      if(this->height == other.height)
	return this->pair < other.pair;
      else
	return this->height < other.height;
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
	join_report.push_back((join_report_entry_type) {std::make_pair(v_left, v_right), height[*vi]}); 
      }
    }
    
    return join_report;
  }


  // template <typename height_type, typename vertex_descriptor>
  // void sort_join_report(std::vector< join_report_entry<height_type, vertex_descriptor> >& join_report){
  //   // sort by height, then by pair
  //   // this is trivial due to operator< in join_report_entry
  //   std::sort(join_report.begin(), join_report.end());
  // }
  
  

    
  
}

#endif /* _JOIN_REPORT_H_ */