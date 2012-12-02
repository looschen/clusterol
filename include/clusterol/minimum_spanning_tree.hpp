#ifndef _MINIMUM_SPANNING_TREE_H_
#define _MINIMUM_SPANNING_TREE_H_

#include <vector>
#include <list>
#include <limits>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/depth_first_search.hpp>
#include <boost/pending/disjoint_sets.hpp>
// shut up warning on new boost
#include <boost/version.hpp>
#if BOOST_VERSION >= 104100
#include <boost/property_map/property_map.hpp>
#else
#include <boost/property_map.hpp>
#endif

#include "dendrogram.hpp"

namespace clusterol{
  template<typename vertex, typename weight_type>
  struct weighted_edge{
    vertex source;
    vertex target;
    weight_type weight;
  };

  
  template <typename random_access_data, typename graph, typename property_map, typename dissimilarity>
  void minimum_spanning_tree(const random_access_data data, const random_access_data data_end, graph& mst, property_map weight, dissimilarity d = dissimilarity()){
    // Prim's algorithm, O(N^2)
    using namespace std; using namespace boost;
  
    typedef typename graph_traits<graph>::edge_descriptor edge;
    typedef typename graph_traits<graph>::vertex_descriptor vertex;
    typedef typename property_traits<property_map>::value_type weight_type;
    typedef weighted_edge<vertex, weight_type> w_edge;
    typedef typename list<w_edge>::iterator list_it;

    size_t N = data_end - data;

    // add vertices 0 .. N-1 to the mst, start at v_new = 0
    mst = graph(N);
    vertex v_new = 0;
  
    // list of candidate edges connecting vertices not in the tree to vertices in the tree
    list<w_edge> c_edge;
    for(size_t i = 1; i < N; ++i)
      c_edge.push_back((w_edge) {i, 0, numeric_limits<weight_type>::max()});

    while(! c_edge.empty()){
      list_it best_edge = c_edge.begin();

      for(list_it i = c_edge.begin(); i != c_edge.end(); ++i){
	weight_type w = d(data[i->source], data[v_new]);

	// update candidate edges
	if(i->weight > w){
	  i->weight = w;
	  i->target = v_new;
	}

	// update best edge
	if(best_edge->weight > i->weight)
	  best_edge = i;
      }

      // add best to tree
      v_new = best_edge->source;
      edge e_new = add_edge(best_edge->source, best_edge->target, mst).first;
      weight[e_new] = best_edge->weight;

      // delete from candidate list
      c_edge.erase(best_edge);
    }
  }


  template<typename graph_mst, typename graph_tree, typename property_map_weight, typename property_map_h, typename propery_map_edge>
  void get_tree_from_mst(graph_tree& T, property_map_h h, propery_map_edge corresponding_edge, const graph_mst& mst, const property_map_weight weight){
    // get the corresponding clustering tree from a mst and its weights.
    // corresponding_edge will contain the correspondig mst edges to the heights in h.
    // DO NOT CHANGE! This is used by Linf!  

    using namespace std; using namespace boost;
  
    typedef typename property_traits<property_map_weight>::value_type weight_type;
    typedef typename graph_traits<graph_mst>::edge_descriptor mst_edge;
    typedef typename graph_traits<graph_mst>::vertex_descriptor mst_vertex;
    typedef typename graph_traits<graph_tree>::vertex_descriptor tree_vertex;

    // sorted weights
    multimap<weight_type, mst_edge> weight_edge_map;
    // insert into map
    typename graph_traits<graph_mst>::edge_iterator ei, ei_end;
    for(tie(ei, ei_end) = edges(mst); ei != ei_end; ++ei)
      weight_edge_map.insert(make_pair(weight[*ei], *ei));

    // init T, h, mst_edge
    T = graph_tree(num_vertices(mst));
    typename graph_traits<graph_tree>::vertex_iterator vi, vi_end;
    for(tie(vi, vi_end) = vertices(T); vi != vi_end; ++vi){
      h[*vi] = 0;
    }

    // disjoint sets, aka union find
    // init: put every data point into its own set
    disjoint_sets_with_storage<> dis_sets(num_vertices(mst));
    for(size_t i = 0; i != num_vertices(mst); ++i)
      dis_sets.make_set(i);

    // representative element -> vertex in T
    vector<tree_vertex> rep_to_vertex(num_vertices(mst));
    for(size_t i = 0; i != rep_to_vertex.size(); ++i)
      rep_to_vertex[i] = i;
  
    // cluster
    for(typename multimap<weight_type, mst_edge>::const_iterator i = weight_edge_map.begin(); i != weight_edge_map.end(); ++i){
      tree_vertex parent = add_vertex(T);
      h[parent] = i->first;
      corresponding_edge[parent] = i->second;

      size_t s = source(i->second, mst);
      size_t t = target(i->second, mst);

      // find representatives of sets containing s and t
      size_t rep_s = dis_sets.find_set(s);
      size_t rep_t = dis_sets.find_set(t);

      // get tree vertices for rep_s and rep_t
      tree_vertex child_s = rep_to_vertex[rep_s];
      tree_vertex childissimilarity = rep_to_vertex[rep_t];

      // connect parent to children
      add_edge(parent, child_s, T);
      add_edge(parent, childissimilarity, T);
        
      // set union
      dis_sets.link(rep_s, rep_t);

      // update rep_to_vertex
      rep_to_vertex[dis_sets.find_set(rep_s)] = parent;
    
    }
  }


  namespace{
    // since single_link_mst does not write n_member, code from
    // tree_util.hpp is included here. n_member has been renamed to
    // size in dendrogram<>, but write_n_member and the
    // n_member_writer are copied and pasted without renaming.
    
    template <typename property_map>
      struct n_member_writer: public boost::base_visitor< n_member_writer<property_map> >{
      // write number of members (i.e. number of leaves under a vertex) to a property map.
      // use with depth-first search, works on directed graphs.
  
      typedef boost::on_finish_vertex event_filter;
  
      n_member_writer(property_map pm_): pm(pm_) {}

      template <typename Graph, typename Vertex>
      void operator()(const Vertex v, const Graph& G){
	using namespace boost;
	if(out_degree(v,G) == 0)
	  pm[v] = 1;
	else{
	  pm[v] = 0;
	  typename graph_traits<Graph>::adjacency_iterator ai, ai_end;
	  for(tie(ai, ai_end) = adjacent_vertices(v, G); ai != ai_end; ++ai)
	    pm[v] += pm[*ai];
	}
      }
  
    protected:
      property_map pm;
    };


    template <typename Graph, typename Vertex, typename property_map>
      void write_n_member(const Graph& G, const Vertex root, property_map pm){
      using namespace boost;
      // write number of members (i.e. number of leaves under a vertex) to a property map.
      n_member_writer<property_map> writer(pm);
      depth_first_search(G, visitor(make_dfs_visitor(writer)) .root_vertex(root));
    }
  }
  
  
  template <typename height_type, typename random_access_iterator, typename dissimilarity>
  void single_link_mst(dendrogram<height_type>& dend, random_access_iterator data, random_access_iterator data_end, dissimilarity d = dissimilarity()){
    // single link in O(n^2) with a minimum spanning tree
    using namespace std; using namespace boost;

    // declare the mst
    // typedef typename property_traits<property_map>::value_type h_type;
    typedef adjacency_list<vecS, vecS, undirectedS, no_property, property<edge_weight_t, height_type> > mst_type;
    typedef typename graph_traits<mst_type>::edge_descriptor mst_edge;
    mst_type mst;

    // get mst
    minimum_spanning_tree(data, data_end, mst, get(edge_weight, mst), d);
    // write_graphviz(cout, mst, make_label_writer(get(vertex_index, mst)), make_label_writer(get(&mst_edge_bundle<height_type>::weight, mst)));

    // get dendrogram
    vector<mst_edge> corresponding_edge(2*num_vertices(mst) - 1); // unused variable
    get_tree_from_mst(dend.tree, dend.height.begin(), corresponding_edge.begin(), mst, get(edge_weight, mst));

    // write n_member for a nice fully specified  dendrogram
    write_n_member(dend.tree, dend.root, dend.size.begin());
  }
}


#endif /* _MINIMUM_SPANNING_TREE_H_ */
