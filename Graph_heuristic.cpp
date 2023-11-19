#include "Utility.h"
#include "Graph.h"
#include "Timer.h"
// #define DEBUG
using namespace std;

struct Sorted_Node{     
    int node;
    int degree;
    Sorted_Node(){}
    Sorted_Node(int _node, int _degree){
        node = _node;
        degree = _degree;
    }
};
bool sort_by_degree(Sorted_Node A, Sorted_Node B){
    return A.degree > B.degree;
}

bool Graph::IsMaximal(vector<int>& R, int* r_attr){
    int* ncnt = new int [n];        

    for(auto u : R){
        for(int j = offset[u]; j < pend[u]; j ++){
            int v = edge_list[j];
            if(color[v] == -1) continue;
            ncnt[color[v]] = 1;
        }
    }
    bool is_max = 1;
    for(int i = 0; i < n; i ++){
        if(ncnt[i] == R.size()){        
            if(attribute[i] == 0){
                if(max(r_attr[0]+1, r_attr[1]) - min(r_attr[0]+1, r_attr[1]) <= delta){ 
                    is_max = 0;
                    break;
                }
            }
            else {
                if(max(r_attr[0], r_attr[1]+1) - min(r_attr[0], r_attr[1]+1) <= delta){ 
                    is_max = 0;
                    break;
                }
            }
        }
    }

    if(ncnt != nullptr) delete[] ncnt;
    return is_max;
}

void Graph::Degree_Heuristic(){
    string type = "Degree";

    vector<int> R;                  
    vector<int>* C = new vector<int> [2];               
    int* r_attr = new int [2];      
    int tar_attr;                   
    Sorted_Node* sn = new Sorted_Node [n];  
    int* vis = new int [n];         

    for(int i = 0; i < n; i ++){
        if(node_cut[i]){
            sn[i] = Sorted_Node(i, 0);
            continue;
        }
        sn[i] = Sorted_Node(i, pend[i] - offset[i]);
    }
    sort(sn, sn + n, sort_by_degree);     

    for(int i = 0; i < 1; i ++){         
        int max_node = sn[i].node;
        for(int j = 0; j < n; j ++) vis[j] = 0;
        component.clear();
        get_connected_component(max_node, vis);
        
        R.clear();  R.push_back(max_node);
        C[0].clear(); C[1].clear();
        if(attribute[max_node] == 0) r_attr[0] = 1, r_attr[1] = 0;  
        else r_attr[0] = 0, r_attr[1] = 1;
        for(int j = offset[max_node]; j < pend[max_node]; j ++){
            int v = edge_list[j];
            C[attribute[v]].push_back(v);
        }
        tar_attr = 1 - attribute[max_node];
        HeuBranch(R, C, r_attr, tar_attr, -1, type);
    }

    if(vis != nullptr) delete[] vis;
    if(sn != nullptr) delete[] sn;
    if(r_attr != nullptr) delete[] r_attr;
    if(C != nullptr) delete[] C;
    return ;
}

void Graph::Colorful_Degree_Heuristic(){
    if(!n) return ;     

    string type = "Colorful";
    get_colorful_attr_degree();     

    vector<int> R;                 
    vector<int>* C = new vector<int> [2];               
    int* r_attr = new int [2];      
    int tar_attr;                   
    Sorted_Node* sn = new Sorted_Node [n];  
    int* vis = new int [n];         

    for(int i = 0; i < n; i ++){
        if(node_cut[i]){
            sn[i] = Sorted_Node(i, 0);
            continue;
        }
        sn[i] = Sorted_Node(i, min(color_degree[i][0], color_degree[i][1]));         
    }
    sort(sn, sn + n, sort_by_degree);     

    for(int i = 0; i < 1; i ++){         
        int max_node = sn[i].node;
        for(int j = 0; j < n; j ++) vis[j] = 0;
        component.clear();
        get_connected_component(max_node, vis);
        
        R.clear();  R.push_back(max_node);
        C[0].clear(); C[1].clear();
        if(attribute[max_node] == 0) r_attr[0] = 1, r_attr[1] = 0;  
        else r_attr[0] = 0, r_attr[1] = 1;
        for(int j = offset[max_node]; j < pend[max_node]; j ++){
            int v = edge_list[j];
            C[attribute[v]].push_back(v);
        }
        tar_attr = 1 - attribute[max_node];
        HeuBranch(R, C, r_attr, tar_attr, -1, type);
    }

    if(vis != nullptr) delete[] vis;
    if(sn != nullptr) delete[] sn;
    if(r_attr != nullptr) delete[] r_attr;
    if(C != nullptr) delete[] C;
    return ;
}

void Graph::HeuBranch(vector<int>& R, vector<int>* C, int* r_attr, int tar_attr, int a_min, string type){
    if(R.size() > MRFC_heu.size()){
        MRFC_heu = R;
    }

    if(R.size() + C[0].size() + C[1].size() <= MRFC_heu.size()) return;  
    if(C[0].size() + C[1].size() == 0){         
        if(R.size() > MRFC_heu.size()){
            MRFC_heu = R;
        }
        return;
    }
    if(a_min != -1 && r_attr[tar_attr] == a_min + delta){       
        if(R.size() > MRFC_heu.size()){
            MRFC_heu = R;
        }
        return ;
    }

    if(C[tar_attr].size() == 0){        
        a_min = r_attr[tar_attr];       
        tar_attr = 1 - tar_attr;
        HeuBranch(R, C, r_attr, tar_attr, a_min, type);
        return ;
    }

    int max_deg = 0, max_node = -1;
    if(type == "Degree"){           
        for(auto u : C[tar_attr]){
            if(pend[u] - offset[u] > max_deg){
                max_deg = pend[u] - offset[u];
                max_node = u;
            }
        }
    }
    else if(type == "Colorful"){        
        for(auto u : C[tar_attr]){
            int cd = min(color_degree[u][0], color_degree[u][1]);
            if(cd > max_deg){
                max_deg = cd;
                max_node = u;
            }
        }
    }
    R.push_back(max_node);
    r_attr[tar_attr] ++;

    set<int> tmp_set, tmp_res;
    set<int> nei;
    int ccnt0 = 0, ccnt1 = 0;
    for(int i = offset[max_node]; i < pend[max_node]; i ++){
        int v = edge_list[i];
        nei.insert(v);
    }
    tmp_set.clear(); tmp_res.clear();
    for(auto u : C[0]){
        tmp_set.insert(u);
    }
    C[0].clear();
    tmp_res = intersection(tmp_set, nei);
    for(auto u : tmp_res){
        C[0].push_back(u);
        if(attribute[u] == 0) ccnt0 ++;
        else ccnt1 ++;
    }

    tmp_set.clear(); tmp_res.clear();
    for(auto u : C[1]){
        tmp_set.insert(u);
    }
    C[1].clear();
    tmp_res = intersection(tmp_set, nei);
    for(auto u : tmp_res){
        C[1].push_back(u);
        if(attribute[u] == 0) ccnt0 ++;
        else ccnt1 ++;
    }

    if(R.size() + C[0].size() + C[1].size() < threshold * 2) return;  
    if((r_attr[0] + ccnt0 < threshold) || (r_attr[1] + ccnt1 < threshold)) return;  

    HeuBranch(R, C, r_attr, 1-tar_attr, a_min, type);  
    r_attr[tar_attr] --;    

    return;
}
// Alg: Heuristic Framework
int Graph::Find_MRFC_Heuristic(){
    MRFC_heu.clear();
    Degree_Heuristic();            

    int* cnt = new int[2];
    cnt[0] = cnt[1] = 0;
    for(auto u : MRFC_heu) cnt[attribute[u]] ++;
    // int k_star = max(0, max(cnt[0], cnt[1]) - delta);
    int k_star = MRFC_heu.size()-1;
    int k_cast = 0;
    
    // K_Core_Reduction(k_star);         
    Node_Contraction();
    // printf("k_star: %d\n", k_star);

    vector<int> now_MRFC = MRFC_heu;
    Colorful_Degree_Heuristic();    
    if(MRFC_heu.size() > now_MRFC.size()){  
        cnt[0] = cnt[1] = 0;
        for(auto u : MRFC_heu) cnt[attribute[u]] ++;
        // k_cast = max(cnt[0], cnt[1]) - delta;
        k_cast = MRFC_heu.size()-1;
        // printf("k_cast: %d\n", k_cast);
        K_Core_Reduction(k_cast);         
        Node_Contraction();
    }
    reColor();      
    int ub = max_color;
    have_ans = 1;
    delete[] cnt;

    cout << "Heur size: " << MRFC_heu.size() << endl;
    cout.flush();

    return ub;
}
