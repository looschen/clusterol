#ifndef _LANCE_WILLIAMS_H_
#define _LANCE_WILLIAMS_H_

#include "dissimilarity_matrix.hpp"

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
// dis_val LW(size_t x, size_t a, size_t b, const clusterol::dissimilarity_matrix<dis_val>& dis_mat)
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


    template <typename dis_val>
    dis_val operator()(size_t x, size_t a, size_t b, const clusterol::dissimilarity_matrix<dis_val>& dis_mat){
      return alpha_i * dis_mat(x, a) + alpha_j * dis_mat(x, b) + beta * dis_mat(a, b) + gamma * std::abs(dis_mat(x, a) - dis_mat(x,b));
    }
  
  private:
    // is copying 4 values everytime lance_williams needs to be copied a good idea?
    double alpha_i, alpha_j, beta, gamma;
  };


  // // change these to objects, which store n_member-maps permanently for more uniform cluster-algorithms
  // template <typename dis_val, typename property_map>
  // dis_val lance_williams_ward(size_t x, size_t a, size_t b, const clusterol::dissimilarity_matrix<dis_val>& dis_mat, property_map n_member){
  //   dis_val member_sum = n_member[x] + n_member[a] + n_member[b];
  //   dis_val alpha_i = (n_member[a] + n_member[x]) / member_sum;
  //   dis_val alpha_j = (n_member[b] + n_member[x]) / member_sum;
  //   dis_val beta = - dis_val(n_member[x]) / member_sum;
  //   //  std::cout << alpha_i << ", " << alpha_j << ", " << beta << ": " << x << a << b << ", ""\n";

  //   return alpha_i * dis_mat(x, a) + alpha_j * dis_mat(x, b) + beta * dis_mat(a, b);
  // }


  // template <typename dis_val, typename property_map>
  // dis_val lance_williams_group_average(size_t x, size_t a, size_t b, const clusterol::dissimilarity_matrix<dis_val>& dis_mat, property_map n_member){
  //   size_t member_sum = n_member[a] + n_member[b];
  //   dis_val alpha_i = (dis_val) (n_member[a]) / member_sum;
  //   dis_val alpha_j = (dis_val) (n_member[b]) / member_sum;

  //   return alpha_i * dis_mat(x, a) + alpha_j * dis_mat(x, b);
  // }


  // template <typename dis_val, typename property_map>
  // dis_val lance_williams_centroid(size_t x, size_t a, size_t b, const clusterol::dissimilarity_matrix<dis_val>& dis_mat, property_map n_member){
  //   size_t member_sum = n_member[a] + n_member[b];
  //   dis_val alpha_i = (dis_val) (n_member[a]) / member_sum;
  //   dis_val alpha_j = (dis_val) (n_member[b]) / member_sum;
  //   dis_val beta = - (dis_val) n_member[a] * n_member[b] / (member_sum * member_sum);

  //   return alpha_i * dis_mat(x, a) + alpha_j * dis_mat(x, b) + beta * dis_mat(a, b);
  // }

}

#endif /* _LANCE_WILLIAMS_H_ */
