#ifndef _CLUSTEROL_CLUSTER_H_
#define _CLUSTEROL_CLUSTER_H_

#include "matrix_based.hpp"
#include "minimum_spanning_tree.hpp"
#include <string>
#include <stdexcept>


// Easily use any clustering method provided by clusterol.

namespace clusterol{

  template <typename height_type, typename random_access_iterator, typename dissimilarity>
  dendrogram<height_type> cluster(random_access_iterator data, random_access_iterator data_end, const std::string& method, dissimilarity d){
    // Parse method and cluster data with dissimilarity.

    const std::string available_methods[] = {"matrix-single-link",  "complete-link",
					     "ward",
					     "group-average", "weighted-group-average",
					     "centroid", "median",
					     // "energy", "Linf",
					     "single-link"
    };
    const std::string* available_methods_end = available_methods + 8;

    if(std::find(available_methods, available_methods_end, method.c_str()) == available_methods_end){
      throw std::runtime_error("Requested clustering method not available.");
    }


    // lance_williams_generic lw(0.5, 0.5, 0, -0.5); // single link 
    // lance_williams lw(0.5, 0.5, 0, 0.5);	// complete link
    // lance_williams lw(0.5, 0.5, 0, 0);	// weighted group average
    // lance_williams lw(0.5, 0.5, -0.25, 0);// Median (weighted centroid

    size_t n_data_point = std::distance(data, data_end);
    dendrogram<height_type> dend(n_data_point);
    if(method == "matrix-single-link"){
      lance_williams_generic lw(0.5, 0.5, 0, -0.5);
      matrix_cluster<height_type>(dend, data, data_end, d, lw);
    }else if(method == "complete-link"){
      lance_williams_generic lw(0.5, 0.5, 0, 0.5);
      matrix_cluster<height_type>(dend, data, data_end, d, lw);
    }else if(method == "ward"){
      lance_williams_ward<height_type> lw(dend);
      matrix_cluster<height_type>(dend, data, data_end, d, lw);
    }else if(method == "group-average"){
      lance_williams_group_average<height_type> lw(dend);
      matrix_cluster<height_type>(dend, data, data_end, d, lw);
    }else if(method == "weighted-group-average"){
      lance_williams_generic lw(0.5, 0.5, 0, 0);
      matrix_cluster<height_type>(dend, data, data_end, d, lw);
    }else if(method == "centroid"){
      lance_williams_centroid<height_type> lw(dend);
      matrix_cluster<height_type>(dend, data, data_end, d, lw);
    }else if(method == "median"){
      lance_williams_generic lw(0.5, 0.5, -0.25, 0);
      matrix_cluster<height_type>(dend, data, data_end, d, lw);
    }else if(method == "single-link"){
      // single_link_mst is default single-link because it's faster
      single_link_mst(dend, data, data_end, d);
    }

    return dend;
  }
}
    

#endif /* _CLUSTEROL_CLUSTER_H_ */
