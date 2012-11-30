#ifndef _DISSIMILARITY_MATRIX_H_
#define _DISSIMILARITY_MATRIX_H_

#include <vector>
#include <map>
#include <utility>
#include <iostream>
#include <limits>
#include <iterator>
#include <stdexcept>

// A  dissimilarity matrix based on various STL-containers

namespace clusterol{

  template <typename iterator>
  struct const_key_iterator{
    // bidirectional iterator which iterates over underlying iterator
    // and returns it->first when dereferenced

    typedef typename std::iterator_traits<iterator>::value_type it_value_type;
    typedef typename it_value_type::first_type value_type;
    typedef const value_type* pointer;
    typedef const value_type& reference;
    typedef std::bidirectional_iterator_tag iterator_category;
  
    const_key_iterator(iterator it_ = iterator()): it(it_) {}

    bool operator==(const const_key_iterator<iterator>& other) const{
      return it == other.it;
    }

    bool operator!=(const const_key_iterator<iterator>& other) const{
      return it != other.it;
    }

    reference operator*() const{
      return it->first;
    }

    pointer operator->() const{
      return & it->first;
    }

    // prefix: reference, postfix: copy
    const_key_iterator<iterator>& operator++(){
      ++it;
      return *this;
    }

    const_key_iterator<iterator> operator++(int postfix){
      const_key_iterator<iterator> result = *this;
      ++it;
      return result;
    }

    const_key_iterator<iterator>& operator--(){
      --it;
      return *this;
    }

    const_key_iterator<iterator> operator--(int postfix){
      const_key_iterator<iterator> result = *this;
      --it;
      return result;
    }  

  private:
    iterator it;
  };


  template <typename matrix_t, typename index_t, typename compare_t>
  struct matrix_compare{
    // compare indirectly by matrix entries
    matrix_compare(const matrix_t& matrix_, compare_t compare_ = compare_t())
      : matrix(matrix_), compare(compare_) {}

    bool operator()(const std::pair<index_t, index_t>& a, const std::pair<index_t, index_t>& b){
      return compare(matrix[a.first][a.second], matrix[b.first][b.second]);
    }
  
  private:
    const matrix_t& matrix;
    compare_t compare;
  };


  template <typename dis_val = double, typename index_t = uint16_t>
  class dissimilarity_matrix{
    // typedefs
  private:
    typedef typename std::vector< std::vector<dis_val> > matrix_t;
    typedef typename  std::multiset< std::pair<index_t, index_t>, matrix_compare<matrix_t, index_t, std::less<dis_val> > > set_t;
    typedef std::vector< std::vector<typename set_t::iterator> > it_matrix_t;
  public:
    typedef const_key_iterator< typename std::map<size_t, index_t>::const_iterator > id_iterator;

    // exposesindex pairs without translation, debugging use only
    // typedef typename set_t::iterator raw_sorted_pair_iterator;


    // Constructor:
    //dissimilarity_matrix(size_t size);
    //   // generate a dissimilarity_matrix filled with 0
    //   // this would be very inefficient due to the multimap-generation

    template <typename random_access_iterator, typename dissimilarity_t>
    dissimilarity_matrix(random_access_iterator data, random_access_iterator data_end, dissimilarity_t dissimilarity);

  
    void print(std::ostream& os) const; 
    void update(size_t id_a, size_t id_b, dis_val value);
    void erase(size_t id);

  
    dis_val operator()(size_t id_a, size_t id_b) const{
      size_t a = external_to_internal(id_a);
      size_t b = external_to_internal(id_b);
        
      if(a > b)
	return matrix[a][b];
      else if(a < b)
	return matrix[b][a];
      else
	return 0;			// id_a == id_b
    }
  
    void move(size_t old_id, size_t new_id){
      // mv from old_id to new_id
      // this changes the maps only.

      size_t ind = external_to_internal(old_id);

      external_to_internal_map.erase(old_id);
      external_to_internal_map[new_id] = ind;
      internal_to_external_map[ind] = new_id;
    }    

  
    std::pair<index_t, index_t> min_pair() const{
      // return minimum entry of dissimilarity matrix
      std::pair<index_t, index_t> mp = *mset.begin();
      return std::make_pair(internal_to_external(mp.first), internal_to_external(mp.second));
    }


    bool is_valid(size_t id) const{
      // return true, if id is a valid cluster id
      return external_to_internal_map.count(id);
    }

  
    size_t valid() const{
      // get number of valid ids
      return external_to_internal_map.size();
    }

  
    id_iterator id_begin() const{
      // iterate over ids
      return id_iterator(external_to_internal_map.begin()); 
    }

    id_iterator id_end() const{
      return id_iterator(external_to_internal_map.end());
    }

    // exposes internal ids, may still be useful for debugging
    // raw_sorted_pair_iterator raw_sorted_begin() const{
    //   // dangerous if matrix has been used for clustering, but fast otherwise
    //   return mset.begin();
    // }

    // raw_sorted_pair_iterator raw_sorted_end() const{
    //   return mset.end();
    // }

  private:

    // external / internal conversion
    index_t external_to_internal(size_t id) const{
      typename std::map<size_t, index_t>::const_iterator result_it =  external_to_internal_map.find(id);
      // good for debugging:
      if(result_it == external_to_internal_map.end())
	throw std::runtime_error("invalid external id");

      return result_it->second;
    }

    size_t internal_to_external(index_t index) const{
      return internal_to_external_map.find(index)->second;
    }


    std::map<size_t, index_t> external_to_internal_map; // id -> index
    std::map<index_t, size_t> internal_to_external_map; // index -> id

    set_t mset;			// (index, index) sorted
    matrix_t matrix;		// index, index -> val
    it_matrix_t it_matrix;	// index, index -> iterator
  };


  template <typename dis_val, typename index_t>
  template <typename random_access_iterator, typename dissimilarity_t>
  dissimilarity_matrix<dis_val, index_t>::dissimilarity_matrix(random_access_iterator data, random_access_iterator data_end, dissimilarity_t dissimilarity)
    : mset(matrix)
  {
    // calculate matrix from data with dissimilarity
    using namespace std;

    size_t size = distance(data, data_end);
  
    // get memory and set up maps
    matrix.resize(size); it_matrix.resize(size);
    for(size_t i = 0; i != size; ++i){
      external_to_internal_map[i] = i;
      internal_to_external_map[i] = i;
      matrix[i].resize(i);
      it_matrix[i].resize(i);
    }

    // fill matrix and mset
    for(size_t i = 0; i != size; ++i){
      for(size_t j = 0; j != i; ++j){
	matrix[i][j] = dissimilarity(data[i], data[j]);
      }
    }

    // two loops is better for cache and simd
    for(size_t i = 0; i != size; ++i)
      for(size_t j = 0; j != i; ++j)
	it_matrix[i][j] =  mset.insert(make_pair(i, j));

  } 


  template <typename dis_val, typename index_t>
  void dissimilarity_matrix<dis_val, index_t>::update(size_t id_a, size_t id_b, dis_val value){
    // change entry (id_a, id_b) to value
    using namespace std;

    size_t a = external_to_internal(id_a);
    size_t b = external_to_internal(id_b);
        
    if(a == b)
      return;			// dis_mat(x, x) == 0
    
    if(a < b)
      swap(a, b);

    matrix[a][b] = value;
 
    // // insert with indication of the right position
    // DON'T DO THIS! in rare cases something goes wrong.
    // typename set_t::iterator old_pos = it_matrix[a][b];
    // it_matrix[a][b] = mset.insert(old_pos, make_pair(a, b));
    // mset.erase(old_pos);
    // Standard says: (23.1.2)
    // The insert members shall not affect the validity of iterators
    // and references to the container, and the erase members shall
    // invalidate only iterators and references to the erased
    // elements.
    // BUT:
    // matrix[a][b] gets a new value above and is used in comparisons
    // in the set. Inserting before erasing may result in a wrong
    // order for the set.
    
    // without indication
    mset.erase(it_matrix[a][b]);
    it_matrix[a][b] = mset.insert(make_pair(a, b));
  }


  template <typename dis_val, typename index_t>
  void dissimilarity_matrix<dis_val, index_t>::erase(size_t id){
    // erase information related to id

    using namespace std;

    size_t ind = external_to_internal(id);

    typedef typename map<index_t, size_t>::iterator it_type;
    it_type ind_it = internal_to_external_map.find(ind);

    // The lower triangle exists. Draw this on paper to understand.
    // The two loops don't overlap
    for(it_type j = internal_to_external_map.begin(); j != ind_it; ++j)
      mset.erase(it_matrix[ind][j->first]);

    it_type begin = ind_it; ++begin;
    for(it_type i = begin; i != internal_to_external_map.end(); ++i)
      mset.erase(it_matrix[i->first][ind]);
    
    // // save some ram
    it_matrix[ind].clear();
    matrix[ind].clear();

    // really save ram
    typename it_matrix_t::value_type().swap(it_matrix[ind]);
    typename matrix_t::value_type().swap(matrix[ind]);
  
    internal_to_external_map.erase(ind_it);
    external_to_internal_map.erase(id); // key not iterator, optimisation possible
  }


  template <typename dis_val, typename index_t>
  void dissimilarity_matrix<dis_val, index_t>::print(std::ostream& os) const{
    // print the matrix and some more
    for(id_iterator i = id_begin(); i != id_end(); ++i){
      for(id_iterator j = id_begin(); j != id_end(); ++j)
	os << (*this)(*i, *j) << "\t";
      os << "\n";
    }

    for(typename set_t::const_iterator i = mset.begin(); i != mset.end(); ++i){
      os << "(" << i->first << ", " << i->second << ")\t";
      os << matrix[i->first][i->second] << "\n";
    }
    
    os << "\n";
  }


  template <typename dis_val, typename index_t>
  std::ostream& operator<<(std::ostream& os, const dissimilarity_matrix<dis_val, index_t> & dis_mat){
    // cout << dissimilarity_matrix
    dis_mat.print(os);
    return os;
  }

}
#endif /* _DISSIMILARITY_MATRIX_H_ */
