#ifndef _CLUSTEROL_MATRIX_BASED_H_
#define _CLUSTEROL_MATRIX_BASED_H_

#include "dendrogram.hpp"
#include "lance_williams.hpp"
#include "dissimilarity_matrix.hpp"
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/version.hpp>
#if BOOST_VERSION >= 104100
#include <boost/property_map/property_map.hpp>
#else
#include <boost/property_map.hpp>
#endif
#include <iostream>


// Matrix-based clustering algorithms

namespace clusterol{

  template <typename height_type, typename lance_williams>
  void join(dissimilarity_matrix<height_type>& dis_mat, dendrogram<height_type>& dend, lance_williams lw){
    // find minimum pair and join

    std::pair<size_t, size_t> min_pair = dis_mat.min_pair();

    // insert into dendrogram
    typename dendrogram<height_type>::vertex_descriptor parent = add_vertex(dend.tree);
    add_edge(parent, min_pair.first, dend.tree);
    add_edge(parent, min_pair.second, dend.tree);
    dend.height[parent] = dis_mat(min_pair.first, min_pair.second);
    dend.size[parent] = dend.size[min_pair.first] + dend.size[min_pair.second];
    dend.root = parent; 
    
    // update dis_mat(min_pair.first, *)
    for(typename dissimilarity_matrix<height_type>::id_iterator i = dis_mat.id_begin(); i != dis_mat.id_end(); ++i){
      if(*i != min_pair.first && *i != min_pair.second){
	height_type new_val = lw(*i, min_pair.first, min_pair.second, dis_mat);
	dis_mat.update(min_pair.first, *i, new_val);
      }
    }

    dis_mat.erase(min_pair.second);
    dis_mat.move(min_pair.first, parent);
  }

  template <typename height_type, typename random_access_iterator, typename dissimilarity, typename lance_williams>
  void matrix_cluster(dendrogram<height_type>& dend, random_access_iterator data, random_access_iterator data_end, dissimilarity d, lance_williams lw){
    // Cluster with a dissimilarity_matrix. If lance_williams needs to
    // access property maps of dend, dend can not be generated here.
    dissimilarity_matrix<height_type> dis_mat(data, data_end, d);

    while(dis_mat.valid() > 1)
      join(dis_mat, dend, lw);

  }

  
  // template <typename height_type, typename random_access_iterator, typename dissimilarity, typename lance_williams>
  // dendrogram<height_type> matrix_cluster(random_access_iterator data, random_access_iterator data_end, dissimilarity d, lance_williams lw){
  //   // Cluster with dissimilarity matrix and return a dendrogram
    
  //   size_t n_data_point = std::distance(data, data_end);

  //   // init dendrogram
  //   dendrogram<height_type> dend(n_data_point);
    
  //   dissimilarity_matrix<height_type> dis_mat(data, data_end, d);

  //   while(dis_mat.valid() > 1)
  //     join(dis_mat, dend, lw);

  //   return dend;
  // }

  
}

#endif /* _CLUSTEROL_MATRIX_BASED_H_ */
