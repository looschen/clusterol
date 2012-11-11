#ifndef _MATRIX_BASED_H_
#define _MATRIX_BASED_H_

#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/version.hpp>
#if BOOST_VERSION >= 104100
#include <boost/property_map/property_map.hpp>
#else
#include <boost/property_map.hpp>
#endif
//#include <iostream>

#include "lance_williams.hpp"
#include "dissimilarity_matrix.hpp"

// Matrix-based clustering algorithms

namespace clusterol{

  template <typename dis_val, typename tree_t, typename property_map>
  void join(dissimilarity_matrix<dis_val>& dis_mat, tree_t& T, property_map height, lance_williams lw){
    // find minimum pair and join
    using namespace std;
    using namespace boost;

    typedef typename graph_traits<tree_t>::vertex_descriptor vertex;
  
    pair<size_t, size_t> min_pair = dis_mat.min_pair();
    // size_t first = cluster_map[min_pair.first];
    // size_t second = cluster_map[min_pair.second];
  
    // add to tree
    vertex parent = add_vertex(T);
    add_edge(parent, min_pair.first, T);
    add_edge(parent, min_pair.second, T);
    height[parent] = dis_mat(min_pair.first, min_pair.second);

    // update dis_mat(min_pair.first, *)
    for(typename dissimilarity_matrix<dis_val>::id_iterator i = dis_mat.id_begin(); i != dis_mat.id_end(); ++i){
      if(*i != min_pair.first && *i != min_pair.second){
	dis_val new_val = lw(*i, min_pair.first, min_pair.second, dis_mat);
	dis_mat.update(min_pair.first, *i, new_val);
      }
    }

    dis_mat.erase(min_pair.second);
    dis_mat.move(min_pair.first, parent);
  }


  template <typename dissimilarity_t, typename random_access_iterator, typename tree_t, typename property_map>
  void matrix_clustering(random_access_iterator data, random_access_iterator data_end, tree_t& T, property_map height, dissimilarity_t dissimilarity, lance_williams lw){
    // Note:
    // keep order of arguments consistent with possible future algos
    
    // use value type of height map for the dissimilarity matrix
    typedef typename  boost::property_traits<property_map>::value_type dis_val;
  
    size_t n_data_point = distance(data, data_end);
    //  std::vector<size_t> cluster_map(n_data_point);
  
    // init tree
    T = tree_t(n_data_point);
    for(size_t i = 0; i != n_data_point; ++i){
      height[i] = 0;
      // cluster_map[i] = i;
    }

    // init dissimilarity matrix
    dissimilarity_matrix<dis_val> dis_mat(data, data_end, dissimilarity);

    while(dis_mat.valid() > 1){
      // std::cerr << dis_mat.valid() << "\n";

      // std::pair<size_t, size_t> min_pair = dis_mat.min_pair();
      // std::cout << "min_pair: (" << min_pair.first << ", " << min_pair.second << ") = " << dis_mat(min_pair.first, min_pair.second) << "\n";
      // std::cout << dis_mat << "\n";
    
      join(dis_mat, T, height, lw);
    }
  }

  /********************************************************************************/
  // clustering algorithm with n_member property map
  // This is to be integrated into the almost identical ordinary algo, when support
  // for n_member is readded

  // template <typename dis_val, typename lance_williams_n,
  // 	    typename tree_t, typename property_map_h, typename property_map_n>
  // void join(dissimilarity_matrix<dis_val>& dis_mat,
  // 	    tree_t& T, property_map_h height, property_map_n n_member,
  // 	    lance_williams_n lw){
  
  //   // find minimum pair and join
  //   using namespace std;
  //   using namespace boost;

  //   typedef typename graph_traits<tree_t>::vertex_descriptor vertex;
  
  //   pair<size_t, size_t> min_pair = dis_mat.min_pair();
  //   // size_t first = cluster_map[min_pair.first];
  //   // size_t second = cluster_map[min_pair.second];

  //   // add to tree
  //   vertex parent = add_vertex(T);
  //   add_edge(parent, min_pair.first, T);
  //   add_edge(parent, min_pair.second, T);
  //   height[parent] = dis_mat(min_pair.first, min_pair.second);
  //   n_member[parent] = n_member[min_pair.first] + n_member[min_pair.second];
  //   // cluster_map[min_pair.first] = parent;
  //   // cluster_map[min_pair.second] = parent;

  //   // update dis_mat(min_pair.first, *)
  //   for(typename dissimilarity_matrix<dis_val>::id_iterator i = dis_mat.id_begin(); i != dis_mat.id_end(); ++i){
  //     // for(size_t i = 0; i != 2 * dis_mat.size(); ++i){
  //     if(*i != min_pair.first && *i != min_pair.second){
  // 	dis_val new_val = lw(*i, min_pair.first, min_pair.second, dis_mat, n_member);
  // 	dis_mat.update(min_pair.first, *i, new_val);
  //     }
  //   }

  //   dis_mat.erase(min_pair.second);
  //   dis_mat.move(min_pair.first, parent);
  // }


  // template <typename dissimilarity_t, typename lance_williams_n,
  // 	    typename random_access_iterator,
  // 	    typename tree_t, typename property_map_h, typename property_map_n>
  // void matrix_clustering(random_access_iterator data, random_access_iterator data_end,
  // 			 tree_t& T, property_map_h height, property_map_n n_member,
  // 			 lance_williams_n lwn, dissimilarity_t dissimilarity){

  
  //   // use value type of height map for the dissimilarity matrix
  //   typedef typename  boost::property_traits<property_map_h>::value_type dis_val;

  //   size_t n_data_point = distance(data, data_end);
  //   //  std::vector<size_t> cluster_map(n_data_point);

  //   // init tree
  //   T = tree_t(n_data_point);
  //   for(size_t i = 0; i != n_data_point; ++i){
  //     n_member[i] = 1;
  //     height[i] = 0;
  //     // cluster_map[i] = i;
  //   }

  //   // init dissimilarity matrix
  //   dissimilarity_matrix<dis_val> dis_mat(data, data_end, dissimilarity);

  //   while(dis_mat.valid() > 1){
  //     // // debug output:
  //     // std::cerr << dis_mat.valid() << "\n";

  //     // std::pair<size_t, size_t> min_pair = dis_mat.min_pair();
  //     // std::cout << "min_pair: (" << min_pair.first << ", " << min_pair.second
  //     //   //	      << ") -> (" << cluster_map[min_pair.first] << ", " << cluster_map[min_pair.second]
  //     // 	      << ") = " << dis_mat(min_pair.first, min_pair.second) << "\n";
  //     // std::cout << dis_mat << "\n";
    
    
  //     join(dis_mat, T, height, n_member, lwn);
  //   }
  // }

}  

#endif /* _MATRIX_BASED_H_ */
