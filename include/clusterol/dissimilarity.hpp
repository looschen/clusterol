#ifndef _CLUSTEROL_DISSIMILARITY_H_
#define _CLUSTEROL_DISSIMILARITY_H_

#include <cmath>
#include <algorithm>


namespace clusterol{

  template <typename dissimilarity_t>
  class dissimilarity_be{
    // dissimilarity measure for data_points that have begin and end
    // (without n_member)
  public:
    dissimilarity_be(): dissimilarity(dissimilarity_t()) {}

    template<typename data_point>
    double operator()(data_point& a, data_point& b){
      return dissimilarity(a.begin(), a.end(), b.begin());
    }

  private:
    dissimilarity_t dissimilarity;
  };


  template <typename dissimilarity_t>
  class dissimilarity_dim{
    // untested
    // use with c-arrays (or vec.begin())
  public:
    dissimilarity_dim(size_t dim_): dissimilarity(dissimilarity_t()), dim(dim_) {}

    template <typename data_point>
    double operator()(data_point a, data_point b){
      return dissimilarity(a, a + dim, b);
    }

  private:
    dissimilarity_t dissimilarity;
    size_t dim;
  };


  struct euclidean_distance{
    template <typename input_iterator>
    double operator()(input_iterator a_begin, input_iterator a_end, input_iterator b_begin){
      double result = 0;
      while(a_begin != a_end){
	double diff = *a_begin++ - *b_begin++;
	result += diff * diff;
      }

      return sqrt(result);
    }
  };


  /********************************************************************************/
  // more and advanced dissimilarities currently unsupported
  
  // struct squared_euclidean_distance{
  //   template <typename input_iterator>
  //   double operator()(input_iterator a_begin, input_iterator a_end, input_iterator b_begin){
  //     double result = 0;
  //     while(a_begin != a_end){
  // 	double diff = *a_begin++ - *b_begin++;
  // 	result += diff * diff;
  //     }

  //     return result;
  //   }
  // };


  // struct hamming_distance{
  //   template <typename input_iterator>
  //   unsigned operator()(input_iterator a_begin, input_iterator a_end, input_iterator b_begin){
  //     unsigned result = 0;
  //     while(a_begin != a_end){
  // 	if(*a_begin++ != *b_begin++)
  // 	  result++;
  //     }
    
  //     return result;
  //   }
  // };


  // struct hamming_distance_bitset{
  //   // hamming distance for std::bitset and boost::dynamic_bitset
  //   template <typename bitset_t>
  //   size_t operator()(const bitset_t& a,const bitset_t& b){
  //     bitset_t tmp = a ^ b;
  //     return tmp.count();
  //   }
  // };

  // struct hamming_distance_bitset_opt{
  //   // this should work, be fast and does not require non-const arguments
  //   // (untested)
  //   hamming_distance_bitset_opt(size_t size)
  //     : helper_bs(size)
  //   {}
  
  //   size_t operator()(const boost::dynamic_bitset<>& a, const boost::dynamic_bitset<>& b){
  //     helper_bs.reset();
  //     helper_bs ^= a;
  //     helper_bs ^= b;

  //     return helper_bs.count();
  //   }

  // private:
  //   boost::dynamic_bitset<> helper_bs;
  // }
    

}



#endif /* _CLUSTEROL_DISSIMILARITY_H_ */
