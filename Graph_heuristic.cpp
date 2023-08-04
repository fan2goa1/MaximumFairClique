#include "Utility.h"
#include "Graph.h"
#include "Timer.h"
// #define DEBUG
using namespace std;

struct Sorted_Node{     // 用于进行点的degree(or enhanced colorful degree)的排序
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

// 用于判断当前找到的R是否是一个极大的maximal relative fair clique
bool Graph::IsMaximal(vector<int>& R, int* r_attr){
    int* ncnt = new int [n];        // ncnt[i]表示R集合中与i相邻的点的个数

    for(auto u : R){
        for(int j = offset[u]; j < pend[u]; j ++){
            int v = edge_list[j];
            if(color[v] == -1) continue;
            ncnt[color[v]] = 1;
        }
    }
    bool is_max = 1;
    for(int i = 0; i < n; i ++){
        if(ncnt[i] == R.size()){        // 说明当前点与R集合中的点都相邻，可以被选
            if(attribute[i] == 0){
                if(max(r_attr[0]+1, r_attr[1]) - min(r_attr[0]+1, r_attr[1]) <= delta){ // 说明还能加点
                    is_max = 0;
                    break;
                }
            }
            else {
                if(max(r_attr[0], r_attr[1]+1) - min(r_attr[0], r_attr[1]+1) <= delta){ // 说明还能加点
                    is_max = 0;
                    break;
                }
            }
        }
    }

    if(ncnt != nullptr) delete[] ncnt;
    return is_max;
}
// Degree Heuristic算法求maximum fair clique
void Graph::Degree_Heuristic(){
    string type = "Degree";

    vector<int> R;                  // 当前的relative fair clique
    vector<int>* C = new vector<int> [2];               // C[0]是属性为0的候选，C[1]是属性为1的候选
    int* r_attr = new int [2];      // r_attr[i]表示当前R集合中属性为i的点数
    int tar_attr;                   // tar_attr表示将要找的点的属性
    Sorted_Node* sn = new Sorted_Node [n];  // 用于对点进行排序
    int* vis = new int [n];         // vis用于寻找标记一个连通分量

    for(int i = 0; i < n; i ++){
        if(node_cut[i]){
            sn[i] = Sorted_Node(i, 0);
            continue;
        }
        sn[i] = Sorted_Node(i, pend[i] - offset[i]);
    }
    sort(sn, sn + n, sort_by_degree);     // 按点从大到小排序

    for(int i = 0; i < 10; i ++){         // 分别枚举10个度数最大的点进行Branch
        int max_node = sn[i].node;
        for(int j = 0; j < n; j ++) vis[j] = 0;
        component.clear();
        get_connected_component(max_node, vis);
        
        R.clear();  R.push_back(max_node);
        C[0].clear(); C[1].clear();
        if(attribute[max_node] == 0) r_attr[0] = 1, r_attr[1] = 0;  // 设定初始值
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
// Colorful Attribute Degree Heuristic算法求maximum fair clique
void Graph::Colorful_Degree_Heuristic(){
    if(!n) return ;     // 空图直接返回

    string type = "Colorful";
    get_colorful_attr_degree();     // 先计算图的colorful attribute degree

    vector<int> R;                  // 当前的relative fair clique
    vector<int>* C = new vector<int> [2];               // C[0]是属性为0的候选，C[1]是属性为1的候选
    int* r_attr = new int [2];      // r_attr[i]表示当前R集合中属性为i的点数
    int tar_attr;                   // tar_attr表示将要找的点的属性
    Sorted_Node* sn = new Sorted_Node [n];  // 用于对点进行排序
    int* vis = new int [n];         // vis用于寻找标记一个连通分量

    for(int i = 0; i < n; i ++){
        if(node_cut[i]){
            sn[i] = Sorted_Node(i, 0);
            continue;
        }
        sn[i] = Sorted_Node(i, min(color_degree[i][0], color_degree[i][1]));         // 将min(color_degree[i][0], color_degree[i][1])放入
    }
    sort(sn, sn + n, sort_by_degree);     // 按点从大到小排序

    for(int i = 0; i < 10; i ++){         // 分别枚举10个度数最大的点进行Branch
        int max_node = sn[i].node;
        for(int j = 0; j < n; j ++) vis[j] = 0;
        component.clear();
        get_connected_component(max_node, vis);
        
        R.clear();  R.push_back(max_node);
        C[0].clear(); C[1].clear();
        if(attribute[max_node] == 0) r_attr[0] = 1, r_attr[1] = 0;  // 设定初始值
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
    if(R.size() + C[0].size() + C[1].size() <= MRFC_heu.size()) return;  // 说明当前搜索到的结果已经不可能比当前最优解更优了
    if(C[0].size() + C[1].size() == 0){         // 此时候选集为空，搜索到底
        if(R.size() > MRFC_heu.size()){
            MRFC_heu = R;
            cout << "Find a new MRFC with size " << MRFC_heu.size() << endl;
            for(auto u : MRFC_heu) cout << to_old_node[u] << " "; puts("");
        }
        return;
    }
    if(a_min != -1 && r_attr[tar_attr] == a_min + delta){       // 说明加到上限不能再加了
        if(R.size() > MRFC_heu.size()){
            MRFC_heu = R;
            cout << "Find a new MRFC with size " << MRFC_heu.size() << endl;
            for(auto u : MRFC_heu) cout << to_old_node[u] << " "; puts("");
        }
        return ;
    }

    if(C[tar_attr].size() == 0){        // 当前要找的属性的候选集为空，说明要找的属性已经找完了，转而找另一个属性
        a_min = r_attr[tar_attr];       // 设置属性个数的下限，上限a_max = a_min + delta
        tar_attr = 1 - tar_attr;
        HeuBranch(R, C, r_attr, tar_attr, a_min, type);
        return ;
    }

    // 寻找degree最大的点加入R
    int max_deg = 0, max_node = -1;
    if(type == "Degree"){           // 以degree选最大的点
        for(auto u : C[tar_attr]){
            if(pend[u] - offset[u] > max_deg){
                max_deg = pend[u] - offset[u];
                max_node = u;
            }
        }
    }
    else if(type == "Colorful"){        // 以Colorful attribute degree选最大的点
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

    // 更新候选集
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

    if(R.size() + C[0].size() + C[1].size() < threshold * 2) return;  // 说明总点数不够
    if((r_attr[0] + ccnt0 < threshold) || (r_attr[1] + ccnt1 < threshold)) return;  // 说明有一个属性的点数不够k，找不到RFC了

    HeuBranch(R, C, r_attr, 1-tar_attr, a_min, type);  // 接着往下找
    r_attr[tar_attr] --;    // 回溯

    return;
}
// Alg9 Heuristic Framework
int Graph::Find_MRFC_Heuristic(){
    Degree_Heuristic();             // 找到用度数排序得到的当前MRFC

    int* cnt = new int[2];
    cnt[0] = cnt[1] = 0;
    for(auto u : MRFC_heu) cnt[attribute[u]] ++;
    // int k_star = max(0, max(cnt[0], cnt[1]) - delta);
    int k_star = MRFC_heu.size()-1;
    int k_cast = 0;
    
    K_Core_Reduction(k_star);         // 计算K-Core
    Node_Contraction();
    printf("k_star: %d\n", k_star);

    vector<int> now_MRFC = MRFC_heu;
    Colorful_Degree_Heuristic();    // 再用Colorful degree排序试找更大的MRFC
    if(MRFC_heu.size() > now_MRFC.size()){  // 说明找到了更大的MRFC
        cnt[0] = cnt[1] = 0;
        for(auto u : MRFC_heu) cnt[attribute[u]] ++;
        // k_cast = max(cnt[0], cnt[1]) - delta;
        k_cast = MRFC_heu.size()-1;
        printf("k_cast: %d\n", k_cast);
        K_Core_Reduction(k_cast);         // 计算K-Core
        Node_Contraction();
    }
    reColor();      // 重新着色
    int ub = max_color;
    have_ans = 1;
    delete[] cnt;

    return ub;
}
