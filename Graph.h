#ifndef _GRAPH_H_
#define _GRAPH_H_
#include <bits/stdc++.h>

using namespace std;

class Graph{
    private:
    int n, m;           // 图的点数和边数
    int threshold;      // 输入中的k
    int attr_size;      // 属性数量，在这里恒为2
    int delta;          // 输入中的delta
    int max_color;      // 对图染色的总颜色数
    int preprocessed;   // 0表示未对图进行染色

    int* offset;        // offset[i]表示第i个节点的所有边在edge_list中的起始位置
    int* edge_list;     // edge_list[i]表示第i条边的另一端节点
    int* attribute;     // attribute[i]表示第i个点的属性标号
    int* color;         // color[i]表示第i个点的颜色标号
    int* pend;          // pend[i]表示第i个节点的所有边在edge_list中的终止位置
    int* node_cut;      // node_retain[i]表示第i个点是否被删掉（在reduced_graph中）
    int* enhanced_degree;   // enhanced_degree[i]表示第i个点的ED
    int* to_old_node;       // to_old_node[i]表示当前点i对应原图中的点的编号
    int** color_degree;     // color_degree[i][j]表示点i的属性j的colorful degree
    int* RCvis;             // 1表示当前点在R或C中，0表示不在

    unordered_map<int, unordered_map<int, int> > unordered_edge;  // unorderd_edge[u][v](u<v)记录该边对应的edge_list的下标
    vector<int> left;       // 记录ColorfulCore除去点后reduced Graph里的点
    vector<int> component;  // reduced Graph里某个点的连通分量
    vector<int> MRFC;        // Maximum Relative Fair Clique

    public:
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
    bool calc_ub(vector<int> &, vector<int> &);         // 计算上界，并判断是否被剪枝
    
    int ub_size(vector<int> &, vector<int> &);          // 求Size based upper bound
    int ub_color(vector<int> &, vector<int> &);         // 求Color based upper bound
    int ub_attr(vector<int> &, vector<int> &);          // 求Attribute based upper bound
    int ub_attr_color(vector<int> &, vector<int> &);    // 求Attribute color based upper bound
    int ub_degeneracy(vector<int> &, vector<int> &, int*);    // 求Degeneracy based upper bound
    int ub_color_degeneracy(vector<int> &, vector<int> &, int*);  // 求Color degeneracy based upper bound
    int ub_h_index(vector<int> &, vector<int> &);       // 求H-index based upper bound
    // int ub_color_h_index(vector<int> &, vector<int> &); // 求Color H-index based upper bound
    // int ub_colorful_path(vector<int> &, vector<int> &); // 求Colorful path based upper bound
    // int ub_colorful_triangle(vector<int> &, vector<int> &); // 求Colorful triangle based upper bound
  
    private:
    inline void printClique(vector<int> clique, ofstream& ofs){ // 打印RFC到文件中
        sort(clique.begin(), clique.end());
        for(vector<int>::iterator it = clique.begin(); it != clique.end(); it ++)
            //ofs<<*it<<"("<<attribute[*it]<<")"<<" "<<"["<<idx_pos[*it]<<"]"<<" ";
           ofs<<*it<<" ";
        ofs<<"\n";
    }
    set<int> intersection(set<int> set_a, set<int> set_b) {
        set<int> result;
        set_intersection(set_a.begin(), set_a.end(), set_b.begin(), set_b.end(), inserter(result, result.begin()));
        return result;
    }
};

#endif