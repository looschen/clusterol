#ifndef _LANCE_WILLIAMS_H_
#define _LANCE_WILLIAMS_H_

#include "dissimilarity_matrix.hpp"
#include "dendrogram.hpp"

// Lance-Williams updates for matrix-based clustering
// D(x, a OR b) = alpha_i * D(x, a) + alpha_j * D(x, b)
//              + beta * D(a, b) + gamma * |D(x, a) - D(x, b)|
// Lit: Gan, Ma, Wu Table 6.8.4, p. 97

// A Lance-Williams formula object calculates the intercluster
// dissimilarity of a cluster x and the merged cluster (a OR b). This
// calculation is often done with the Lance-Williams equation
// D(x, a OR b) = alpha_i * D(x, a) + alpha_j * D(x, b)
//                + beta * D(a, b) + gamma * |D(x, a) - D(x, b)|.
// Values for the coefficients for many clustering methods are
// available in clustering literature. Clusterol expects an object of
// the form
// height_type LW(size_t x, size_t a, size_t b, const clusterol::dissimilarity_matrix<height_type>& dis_mat)
// where x, a and b are cluster ids for dis_mat.


namespace clusterol{
  
  struct lance_williams_generic{
    lance_williams_generic(double _alpha_i, double _alpha_j, double _beta, double _gamma)
      : alpha_i(_alpha_i),
	alpha_j(_alpha_j),
	beta(_beta),
	gamma(_gamma)
    {}

    lance_williams_generic(){}		// allow default construction


    template <typename height_type>
    height_type operator()(size_t x, size_t a, size_t b, const clusterol::dissimilarity_matrix<height_type>& dis_mat){
      return alpha_i * dis_mat(x, a) + alpha_j * dis_mat(x, b) + beta * dis_mat(a, b) + gamma * std::abs(dis_mat(x, a) - dis_mat(x,b));
    }
  
  private:
    // is copying 4 values everytime lance_williams needs to be copied a good idea?
    double alpha_i, alpha_j, beta, gamma;
  };


  // change these to objects, which store n_member-maps permanently for more uniform cluster-algorithms
  template <typename height_type = double>
  struct lance_williams_ward{
    lance_williams_ward(dendrogram<height_type>& dend)
      // dend must have been set up with correct number of data points
      : size(dend.size.begin())
    {}
    
    height_type operator()(size_t x, size_t a, size_t b, const clusterol::dissimilarity_matrix<height_type>& dis_mat){
      height_type member_sum = size[x] + size[a] + size[b];
      height_type alpha_i = (size[a] + size[x]) / member_sum;
      height_type alpha_j = (size[b] + size[x]) / member_sum;
      height_type beta = - height_type(size[x]) / member_sum;
      //  std::cout << alpha_i << ", " << alpha_j << ", " << beta << ": " << x << a << b << ", ""\n";
      
      return alpha_i * dis_mat(x, a) + alpha_j * dis_mat(x, b) + beta * dis_mat(a, b);
    }

  private:
    typename std::vector<size_t>::iterator size; // works like a property_map or c array
  };


  template <typename height_type = double>
  struct lance_williams_group_average{
    lance_williams_group_average(dendrogram<height_type>& dend)
      : size(dend.size.begin())
    {}
    
    height_type operator()(size_t x, size_t a, size_t b, const clusterol::dissimilarity_matrix<height_type>& dis_mat){
      size_t member_sum = size[a] + size[b];
      height_type alpha_i = height_type(size[a]) / member_sum;
      height_type alpha_j = height_type(size[b]) / member_sum;

      return alpha_i * dis_mat(x, a) + alpha_j * dis_mat(x, b);
    }

  private:
    typename std::vector<size_t>::iterator size;
  };


  template <typename height_type>
  struct lance_williams_centroid{
    lance_williams_centroid(dendrogram<height_type>& dend)
      : size(dend.size.begin())
    {}
    
    height_type operator()(size_t x, size_t a, size_t b, const clusterol::dissimilarity_matrix<height_type>& dis_mat){
      size_t member_sum = size[a] + size[b];
      height_type alpha_i = (height_type) (size[a]) / member_sum;
      height_type alpha_j = (height_type) (size[b]) / member_sum;
      height_type beta = - (height_type) size[a] * size[b] / (member_sum * member_sum);

      return alpha_i * dis_mat(x, a) + alpha_j * dis_mat(x, b) + beta * dis_mat(a, b);
    }

  private:
    typename std::vector<size_t>::iterator size;
  };


}

#endif /* _LANCE_WILLIAMS_H_ */
