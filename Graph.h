#ifndef _GRAPH_H_
#define _GRAPH_H_
#include <bits/stdc++.h>

using namespace std;

class Graph{
    private:
    int n, m;           // number of node and edges
    int threshold;      // k in paper
    int attr_size;      // it's a constant of 2 here
    int delta;          
    int max_color;     
    int preprocessed;   // 0 represents whether the graph has been colored
    bool have_ans;      // 1 reparesents anwser is already found

    int* offset;        // offset[i] indicates the starting position in the edge_list for all edges associated with the i-th node.
    int* pend;          // pend[i] represents the ending position in the edge_list for all edges associated with the i-th node.
    int* edge_list;     // edge_list[i] represents the other end node of the i-th edge.
    int* attribute;     
    int* color;         
    int* node_cut;      // node_retain[i] indicates whether the i-th node has been removed (in the reduced graph).
    int* enhanced_degree;   // enhanced_degree[i] represents the enhanced degree (ED) of the i-th node.
    int* to_old_node;       
    int** color_degree;     // color_degree[i][j] represents the colorful degree of attribute j for the node i.
    int* RCvis;            
    int* nvis;             

    unordered_map<int, unordered_map<int, int> > unordered_edge;  // unordered_edge[u][v] (u < v) records the index of the edge in the edge_list corresponding to this pair of vertices.
    vector<int> component;  // Connected component of a particular node in the reduced graph
    vector<int> MRFC_heu;       // Maximum Relative Fair Clique by heuristic algorithm
    vector<int> MRFC_real;      // Maximum Relative Fair Clique by baseline algorithm


    public:
    string alg_type;        // baseline or ub or heuristic
    // in Graph_utils.cpp
    Graph();
    ~Graph();
    void ReadGraph(const char*, const char*, int, int);
    void Preprocess();
    void reColor();
    void check();
    void printGraph();
    void Node_Contraction();
    void get_connected_component(int, int*);
    void get_colorful_attr_degree();
    vector<int> GetColorfulOrdering();
    void printMRFC_real();
    int calc_intersection(int**, int, int, int*);

    // in Graph_reduction.cpp
    void Colorful_Degree_Reduction();
    void Colorful_Support_Reduction();
    void Enhanced_Colorful_Degree_Reduction();
    void Enhanced_Colorful_Support_Reduction();
    void K_Core_Reduction(int);
    
    // in Graph_heuristic.cpp
    void Degree_Heuristic();
    void Colorful_Degree_Heuristic();
    bool IsMaximal(vector<int>&, int*);
    void HeuBranch(vector<int>&, vector<int>*, int*, int, int, string);
    int Find_MRFC_Heuristic();

    // in Graph_ub.cpp
    bool calc_base(vector<int> &, vector<int>*);        // baseline
    bool calc_ub(vector<int> &, vector<int>*);         
    int ub_size(vector<int> &, vector<int>*);          // Size based upper bound
    int ub_color(vector<int> &, vector<int>*);         // Color based upper bound
    int ub_attr(vector<int> &, vector<int>*);          // Attribute based upper bound
    int ub_attr_color(vector<int> &, vector<int>*, vector<int> *);    // Attribute color based upper bound
    int ub_new(vector<int> &, vector<int>*);             // New based upper bound
    int ub_degeneracy(vector<int> &, vector<int>*, int*, int);    // Degeneracy based upper bound
    int ub_color_degeneracy(vector<int> &, vector<int>*, int*, int);  // Color degeneracy based upper bound
    int ub_h_index(vector<int>*, int*);       // H-index based upper bound
    int ub_color_h_index(vector<int>*, int*); // Color H-index based upper bound
    int ub_colorful_path(vector<int> &, vector<int>*); // Colorful path based upper bound
    int ub_colorful_triangle(vector<int> &, vector<int>*); // Colorful triangle based upper bound
  
    // in Graph_baseline.cpp
    void update_map();      
    void MaxRFClique();
    void Branch(vector<int>&, vector<int>*, int, vector<int>, int, int);

    private:
    set<int> intersection(set<int> set_a, set<int> set_b) {
        set<int> result;
        set_intersection(set_a.begin(), set_a.end(), set_b.begin(), set_b.end(), inserter(result, result.begin()));
        return result;
    }
};

#endif
