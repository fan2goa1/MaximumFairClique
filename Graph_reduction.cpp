#include "Utility.h"
#include "Graph.h"
#include "Timer.h"
// #define DEBUG
using namespace std;

// Alg1, Colorful attribute-degree reduction
void Graph::Colorful_Degree_Reduction(){
    // 计算Colorful attribute-degree
    printf("Start Colorful degree reduction...\n");
    
    if(!preprocessed){Preprocess(); preprocessed = 1;}
    
    if(!n || !m){
        printf("Graph is empty!\n");
        return ;
    }

    // 计算Colorful attribute-degree
    int** colorful_d = new int* [n]; // colorful_d[i][j]表示点i的属性为j的邻居的颜色数
    int*** colorful_r =  new int** [n];     // colorful_r[i][j][k]表示点i的所有邻居中，属性为j，颜色为k的点的个数
    for(int i = 0; i < n; i ++){
        colorful_d[i] = new int[attr_size];
        for(int j = 0; j < attr_size; j ++)
            colorful_d[i][j] = 0;
    }
    for(int i = 0; i < n; i ++){
        colorful_r[i] = new int*[attr_size];
        for(int j = 0; j < attr_size; j ++){
            colorful_r[i][j] = new int[max_color];
            for(int k = 0; k < max_color; k ++)
                colorful_r[i][j][k] = 0;
        }
    }
    for(int i = 0; i < n; i ++){
        for(int j = offset[i]; j < pend[i]; j ++){
            int neighbor = edge_list[j];
            if(node_cut[neighbor] == 1) continue;        // 说明已经被砍掉了
            if(colorful_r[i][attribute[neighbor]][color[neighbor]] == 0){
                colorful_d[i][attribute[neighbor]] ++;
            }
            colorful_r[i][attribute[neighbor]][color[neighbor]] ++;
        }
    }

    // 删除Colorful attribute-degree小于阈值的点
    queue<int> Q;   //Q is the set to delete
    int deletenode = 0;
    for(int i = 0; i < n; i ++){
        deletenode = 0;
        if(node_cut[i] == 1) continue;
        if(attribute[i] == 0){
            if(colorful_d[i][0] < threshold - 1 || colorful_d[i][1] < threshold)
                deletenode = 1;
        }
        else if(attribute[i] == 1){
            if(colorful_d[i][0] < threshold || colorful_d[i][1] < threshold - 1)
                deletenode = 1;
        }
        if(deletenode == 1){
            Q.push(i);
            node_cut[i] = 1;
        }
    }

    while(!Q.empty()){
        int cur = Q.front();
        Q.pop(); 
        for(int i = offset[cur]; i < pend[cur]; i ++){
            int neighbor = edge_list[i];
            if(!node_cut[neighbor]){     // v is not removed
                if(-- colorful_r[neighbor][attribute[cur]][color[cur]] <= 0){
                    colorful_d[neighbor][attribute[cur]] --;
                }
                deletenode = 0;

                if(attribute[neighbor] == 0){
                    if(colorful_d[neighbor][0] < threshold - 1 || colorful_d[neighbor][1] < threshold)
                        deletenode = 1;
                }
                else if(attribute[neighbor] == 1){
                    if(colorful_d[neighbor][0] < threshold || colorful_d[neighbor][1] < threshold - 1)
                        deletenode = 1;
                }

                if(deletenode == 1){
                    Q.push(neighbor);
                    node_cut[neighbor] = 1;      // 标记该点被删
                }
            }
        }
    }

    if(colorful_r != nullptr){
        for(int i = 0; i < n; i++) {
            for(int j = 0; j < attr_size; j++) {
                delete[] colorful_r[i][j];
            }
            delete[] colorful_r[i];
        }
        delete[] colorful_r;
    }
    if(colorful_d != nullptr){
        for(int i = 0; i < n; i++) {
            delete[] colorful_d[i];
        }
        delete[] colorful_d;
    }

    // 重建图
    int startpos = 0;
    for(int i = 0; i < n; i ++){
        if(!node_cut[i]){
            int offset_start = startpos;
            for(int j = offset[i]; j < pend[i]; j ++){
                if(!node_cut[edge_list[j]]){
                    edge_list[startpos ++] = edge_list[j];
                }
            }
            offset[i] = offset_start;
            pend[i] = startpos;
        }
        else{
            offset[i] = pend[i] = 0;        // 被删除的点的offset和pend都置为0
        }
    }
    m = startpos;           // 更新边数
    offset[n] = pend[n] = 0;                // n的offset和pend也置为0
    
    printf("Colorful degree reduction done!\n");
    return ;
}
// Alg2
void Graph::Colorful_Support_Reduction(){
    printf("Start Colorful Support Reduction...\n");

    if(!preprocessed){Preprocess(); preprocessed = true;}

    if(!n || !m){
        printf("Graph is empty!\n");
        return ;
    }

    int** colorful_d = new int* [m]; // colorful_d[i][j]表示边i的属性为j的邻居的颜色数
    int*** colorful_r =  new int** [m];     // colorful_r[i][j][k]表示边i的所有邻居中，属性为j，颜色为k的点的个数
    int* edge_cut = new int [m];   // edge_retain[i]表示第i条边是否被删掉（在reduced_graph中） 注：这里是无向图的边的下标<u, v>(u<v)
    
    for(int i = 0; i < m; i ++){
        colorful_d[i] = new int[attr_size];
        for(int j = 0; j < attr_size; j ++)
            colorful_d[i][j] = 0;
    }
    for(int i = 0; i < m; i ++){
        colorful_r[i] = new int*[attr_size];
        for(int j = 0; j < attr_size; j ++){
            colorful_r[i][j] = new int[max_color];
            for(int k = 0; k < max_color; k ++)
                colorful_r[i][j][k] = 0;
        }
    }
    // printf("%lld MB\n", 1ll * sizeof(int) * m * attr_size * max_color / 1024 / 1024);
    for(int i = 0; i < m; i ++) edge_cut[i] = 0;
    
    // set<int> edge_set[n+1];
    unordered_edge.clear();
    // 把每个点的连边装到set里用于求交集, 初始化unordered_edge
    for(int i = 0; i < n; i ++){
        if(node_cut[i]) continue;
        for(int j = offset[i]; j < pend[i]; j ++){
            if(node_cut[edge_list[j]]) continue;
            // edge_set[i].insert(edge_list[j]);
            unordered_edge[i][edge_list[j]] = j;
        }
    }

    set<int> edge_set[2];
    // 计算colorful_d和colorful_r
    for(int i = 0; i < n; i ++){
        if(node_cut[i]) continue;
        int u = i;
        for(int j = offset[u]; j < pend[u]; j ++){
            int v = edge_list[j]; if(node_cut[v]) continue;
            // printf("%d %d\n", u, v);
            if(u > v) continue;     // 只枚举u<v的情况，防止重复枚举
            // printf("<%d, %d>\n", u, v);
            edge_set[0].clear(); edge_set[1].clear();
            for(int k = offset[u]; k < pend[u]; k ++){
                int w = edge_list[k];
                if(w <= u || w <= v || node_cut[w]) continue;
                edge_set[0].insert(w);
            }
            for(int k = offset[v]; k < pend[v]; k ++){
                int w = edge_list[k];
                if(w <= u || w <= v || node_cut[w]) continue;
                edge_set[1].insert(w);
            }
            set<int> common_neighbor = intersection(edge_set[0], edge_set[1]); // 求交集
            // 找到的w都是u < v < w，保证每个三角形只被枚举一次
            #ifdef DEBUG
                printf("<%d, %d> common_neighbor: ", u, v);
                for(auto w: common_neighbor) printf("%d ", w);
                printf("\n");
            #endif
            for(auto w: common_neighbor){
                // 计算Alg 2的5-6行
                if(colorful_r[j][attribute[w]][color[w]] == 0){
                    colorful_d[j][attribute[w]] ++;
                }
                colorful_r[j][attribute[w]][color[w]] ++;

                int j_uw = unordered_edge[u][w];
                if(colorful_r[j_uw][attribute[v]][color[v]] == 0){
                    colorful_d[j_uw][attribute[v]] ++;
                }
                colorful_r[j_uw][attribute[v]][color[v]] ++;

                int j_vw = unordered_edge[v][w];
                if(colorful_r[j_vw][attribute[u]][color[u]] == 0){
                    colorful_d[j_vw][attribute[u]] ++;
                }
                colorful_r[j_vw][attribute[u]][color[u]] ++;

            }
        }
    }

    // 将sup不达标的都删除
    queue<pair<int, int> > Q;
    bool* in_queue = new bool [m];
    int deleteedge = 0;
    for(int i = 0; i < m; i ++) in_queue[i] = 0;
    for(int i = 0; i < n; i ++){
        if(node_cut[i]) continue;
        int u = i;
        for(int j = offset[u]; j < pend[u]; j ++){
            int v = edge_list[j];
            if(node_cut[v]) continue;
            if(u >= v) continue;        // 为保证不重复，只枚举u<v的边
            deleteedge = 0;
            if(attribute[u] == 0 && attribute[v] == 0){
                if(colorful_d[j][0] < threshold - 2 || colorful_d[j][1] < threshold)
                    deleteedge = 1;
            }
            else if(attribute[u] == 1 && attribute[v] == 1){
                if(colorful_d[j][0] < threshold || colorful_d[j][1] < threshold - 2)
                    deleteedge = 1;
            }
            else {
                if(colorful_d[j][0] < threshold - 1 || colorful_d[j][1] < threshold - 1)
                    deleteedge = 1;
            }
            if(deleteedge == 1 && !in_queue[j]){
                Q.push(make_pair(u, j));
                in_queue[j] = 1;
            }
        }
    }
    // for(int i = offset[66]; i < pend[66];i ++) printf("%d ", edge_list[i]); puts("");
    // for(int i = offset[77]; i < pend[77];i ++) printf("%d ", edge_list[i]); puts("");

    while(!Q.empty()){
        pair<int, int> qedge = Q.front(); Q.pop();
        int u = qedge.first, v = edge_list[qedge.second];
        int u1, v1, w1;
        edge_cut[qedge.second] = edge_cut[unordered_edge[v][u]] = 1;
        edge_set[0].clear(); edge_set[1].clear();
        for(int k = offset[u]; k < pend[u]; k ++){
            if(edge_cut[k]) continue;
            edge_set[0].insert(edge_list[k]);
        }
        for(int k = offset[v]; k < pend[v]; k ++){
            if(edge_cut[k]) continue;
            edge_set[1].insert(edge_list[k]);
        }
        set<int> common_neighbor = intersection(edge_set[0], edge_set[1]); // 求交集
        for(auto w: common_neighbor){
            if(node_cut[w]){
                continue;
            }
            u1 = min(u, w); w1 = max(u, w);
            int j1 = unordered_edge[u1][w1];
            if(!edge_cut[j1] && !in_queue[j1]){
                colorful_r[j1][attribute[v]][color[v]] --;
                if(colorful_r[j1][attribute[v]][color[v]] <= 0){
                    colorful_d[j1][attribute[v]] --;
                    deleteedge = 0;
                    if(attribute[u] == 0 && attribute[w] == 0){
                        if(colorful_d[j1][0] < threshold - 2 || colorful_d[j1][1] < threshold) deleteedge = 1;
                    }
                    else if(attribute[u] == 1 && attribute[w] == 1){
                        if(colorful_d[j1][0] < threshold || colorful_d[j1][1] < threshold - 2) deleteedge = 1;
                    }
                    else {
                        if(colorful_d[j1][0] < threshold - 1 || colorful_d[j1][1] < threshold - 1) deleteedge = 1;
                    }
                    
                    if(deleteedge == 1 && !in_queue[j1]){
                        Q.push(make_pair(u1, j1));
                        in_queue[j1] = 1;
                        #ifdef DEBUG
                            printf("now edge <%d, %d>; delete edge <%d, %d>\n", u, v, u1, w1);
                        #endif
                    }
                }
            }
            v1 = min(v, w); w1 = max(v, w);
            int j2 = unordered_edge[v1][w1];
            if(!edge_cut[j2] && !in_queue[j2]){
                colorful_r[j2][attribute[u]][color[u]] --;
                if(colorful_r[j2][attribute[u]][color[u]] <= 0){
                    colorful_d[j2][attribute[u]] --;
                    deleteedge = 0;
                    if(attribute[v] == 0 && attribute[w] == 0){
                        if(colorful_d[j2][0] < threshold - 2 || colorful_d[j2][1] < threshold) deleteedge = 1;
                    }
                    else if(attribute[v] == 1 && attribute[w] == 1){
                        if(colorful_d[j2][0] < threshold || colorful_d[j2][1] < threshold - 2) deleteedge = 1;
                    }
                    else {
                        if(colorful_d[j2][0] < threshold - 1 || colorful_d[j2][1] < threshold - 1) deleteedge = 1;
                    }

                    if(deleteedge == 1 && !in_queue[j2]){
                        Q.push(make_pair(v1, j2));
                        in_queue[j2] = 1;
                        #ifdef DEBUG
                            printf("now edge <%d, %d>; delete edge <%d, %d>\n", u, v, v1, w1);
                        #endif
                    }
                }
            }
        }
    }
    if(colorful_r != nullptr){
        for(int i = 0; i < m; i++) {
            for(int j = 0; j < attr_size; j++) {
                delete[] colorful_r[i][j];
            }
            delete[] colorful_r[i];
        }
        delete[] colorful_r;
    }
    if(colorful_d != nullptr){
        for(int i = 0; i < m; i++) {
            delete[] colorful_d[i];
        }
        delete[] colorful_d;
    }
    if(in_queue != nullptr) delete[] in_queue;


    // 重建图(这轮没有删点，只进行了删边)
    int startpos = 0;
    for(int i = 0; i < n; i ++){
        if(node_cut[i]){offset[i] = pend[i] = 0; continue;}
        int u = i;
        int offset_start = startpos;
        for(int j = offset[i]; j < pend[i]; j ++){
            int v = edge_list[j];
            if(node_cut[v]) continue;
            int u1 = min(u, v), v1 = max(u, v);
            if(!edge_cut[unordered_edge[u1][v1]]){  // 如果这条边没有被删掉
                edge_list[startpos ++] = v;
            }
        }
        offset[i] = offset_start;
        pend[i] = startpos;
    }
    for(int i = 0; i < n; i ++){
        if(node_cut[i]) continue;
        if(offset[i] == pend[i]){       // 说明这个点的所有连边都已经被删掉，则把该点也删掉
            node_cut[i] = 1;
            offset[i] = pend[i] = 0;
        }
    }
    m = startpos;       // 更新m
    if(edge_cut != nullptr) delete[] edge_cut;

    printf("Colorful Support Reduction done!\n");
    return ;
}

void Graph::Enhanced_Colorful_Degree_Reduction(){
    printf("Start Enhanced Colorful Degree Reduction...\n");

    if(!preprocessed){Preprocess(); preprocessed = 1;}

    if(!n || !m){
        printf("Graph is empty!\n");
        return ;
    }

    int*** cntGroup = new int** [n];  // cntGroup[i][j][k]表示点i的邻居中颜色为j，属性为k的数量
    int* c1 = new int [n];          // c1[i]-> number of groups of attribute 0 for i's neighbors
    int* c2 = new int [n];          // c2[i]-> number of groups of attribute 1 for i's neighbors
    int* cboth = new int [n];
    for(int i = 0; i < n; i ++){
        cntGroup[i] = new int* [max_color];
        for(int j = 0; j < max_color; j ++){
            cntGroup[i][j] = new int [attr_size];
            for(int k = 0; k < attr_size; k ++){
                cntGroup[i][j][k] = 0;
            }
        }
    }
    
    for(int i = 0; i < n; i ++){
        c1[i] = c2[i] = cboth[i] = 0;
        if(node_cut[i]) continue;
        for(int j = offset[i]; j < pend[i]; j ++){
            int v = edge_list[j];
            if(!node_cut[v])
                ++ cntGroup[i][color[v]][attribute[v]];
        }
        for(int j = 0; j < max_color; j ++){
            int cc0 = cntGroup[i][j][0], cc1 = cntGroup[i][j][1];
            if(cc0 > 0 && cc1 == 0) c1[i] ++;           // 说明当前颜色的所有邻居都为属性a
            else if(cc0 == 0 && cc1 > 0) c2[i] ++;
            else if(cc0 > 0 && cc1 > 0) cboth[i] ++;
        }
        if(attribute[i] == 0){      // 把i自己算上
            c1[i] ++;
        }
        else c2[i] ++;
    }

    // 打印每个点的c1 c2 cboth
    // for(int i = 0; i < n; i ++){
    //     if(node_cut[i]) continue;
    //     printf("node %d: c1 = %d, c2 = %d, cboth = %d\n", i, c1[i], c2[i], cboth[i]);
    // }

    queue<int> Q;
    for(int i = 0; i < n; i ++){
        if(node_cut[i]) continue;
        int need = cboth[i] - max(0, threshold-c1[i]) - max(0, threshold-c2[i]);
        if(need < 0){           // 该点的ED不能满足要求
            Q.push(i);
            node_cut[i] = 1;
        }
    }
    while(!Q.empty()){
        int u = Q.front(); Q.pop();
        // printf("%d\n", u);
        for(int i = offset[u]; i < pend[u]; i ++){
            int v = edge_list[i];
            if(node_cut[v]) continue;
            if((-- cntGroup[v][color[u]][attribute[u]]) <= 0){
                if(cntGroup[v][color[u]][attribute[u] ^ 1] == 0){   // 说明该颜色的所有邻居本来只有一种属性
                    if(attribute[u] == 0) c1[v] --;
                    else c2[v] --;
                }
                else {          // 说明该颜色的所有邻居本来有2种属性
                    cboth[v] --;
                    if(attribute[u] == 0) c2[v] ++;
                    else c1[v] ++;
                }
            }
            int need = cboth[v] - max(0, threshold-c1[v]) - max(0, threshold-c2[v]);
            if(need < 0){
                Q.push(v);
                node_cut[v] = 1;
            }
        }
    }

    if(cntGroup != nullptr){
        for(int i = 0; i < n; i++) {
            for(int j = 0; j < max_color; j++) {
                delete[] cntGroup[i][j];
            }
            delete[] cntGroup[i];
        }
        delete[] cntGroup;
    }
    if(c1 != nullptr) delete[] c1;
    if(c2 != nullptr) delete[] c2;
    if(cboth != nullptr) delete[] cboth;

    // 重建图
    int startpos = 0;
    for(int i = 0; i < n; i ++){
        if(!node_cut[i]){
            int offset_start = startpos;
            for(int j = offset[i]; j < pend[i]; j ++){
                if(!node_cut[edge_list[j]]){
                    edge_list[startpos ++] = edge_list[j];
                }
            }
            offset[i] = offset_start;
            pend[i] = startpos;
        }
        else{
            offset[i] = pend[i] = 0;        // 被删除的点的offset和pend都置为0
        }
    }
    m = startpos;           // 更新边数
    offset[n] = pend[n] = 0;                // n的offset和pend也置为0

    printf("Enhanced Colorful Degree Reduction done!\n");
    return ;
}

void Graph::Enhanced_Colorful_Support_Reduction(){
    printf("Start Enhanced Colorful Support Reduction...\n");

    if(!preprocessed){Preprocess(); preprocessed = 1;}
    // Node_Contraction();

    if(!n || !m){
        printf("Graph is empty!\n");
        return ;
    }

    int*** cntGroup = new int** [m];  // cntGroup[i][j][k]表示边i的共同邻居中颜色为j，属性为k的点的数量
    int* c1 = new int [m];          // c1[i]表示对于边i，有多少个颜色，其所有该颜色的共同邻居属性均为0
    int* c2 = new int [m];          // c2[i]表示对于边i，有多少个颜色，其所有该颜色的共同邻居属性均为1
    int* cboth = new int [m];       // cboth[i]表示对于边i，有多少个颜色，其所有该颜色的共同邻居属性既有0也有1
    for(int i = 0; i < m; i ++){
        cntGroup[i] = new int* [max_color];
        for(int j = 0; j < max_color; j ++){
            cntGroup[i][j] = new int [attr_size];
            for(int k = 0; k < attr_size; k ++)
                cntGroup[i][j][k] = 0;
        }
    }
    for(int i = 0; i < m; i ++) c1[i] = c2[i] = cboth[i] = 0;

    int* edge_cut = new int [m];
    for(int i = 0; i < m; i ++) edge_cut[i] = 0;
    
    set<int> edge_set[2];
    unordered_edge.clear();
    // 把每个点的连边装到set里用于求交集, 初始化unordered_edge
    for(int i = 0; i < n; i ++){
        if(node_cut[i]) continue;
        for(int j = offset[i]; j < pend[i]; j ++){
            // edge_set[i].insert(edge_list[j]);
            unordered_edge[i][edge_list[j]] = j;
        }
    }
    // 计算ED(u, v)
    for(int i = 0; i < n; i ++){
        if(node_cut[i]) continue;
        int u = i;
        for(int j = offset[u]; j < pend[u]; j ++){
            int v = edge_list[j];
            if(node_cut[v]) continue;
            // printf("%d %d\n", u, v);
            if(u >= v) continue;     // 只枚举u<v的情况，防止重复枚举
            // 枚举 u < v < w 的三角形，一个三角形只被枚举一次，贡献三次
            edge_set[0].clear(); edge_set[1].clear();
            for(int k = offset[u]; k < pend[u]; k ++){
                int w = edge_list[k];
                if(node_cut[w] || w <= u || w <= v) continue;
                edge_set[0].insert(w);
            }
            for(int k = offset[v]; k < pend[v]; k ++){
                int w = edge_list[k];
                if(node_cut[w] || w <= u || w <= v) continue;
                edge_set[1].insert(w);
            }
            set<int> common_neighbor = intersection(edge_set[0], edge_set[1]); // 求交集
            for(auto w: common_neighbor){
                cntGroup[j][color[w]][attribute[w]] ++;
                int j_uw = unordered_edge[u][w], j_vw = unordered_edge[v][w];
                cntGroup[j_uw][color[v]][attribute[v]] ++;
                cntGroup[j_vw][color[u]][attribute[u]] ++;
            }
        }
    }
    // 将不达标的ED放入队列Q
    queue<pair<int, int> > Q;
    bool* in_queue = new bool [m];
    for(int i = 0; i < m; i ++) in_queue[i] = 0;
    for(int i = 0; i < n; i ++){
        if(node_cut[i]) continue;
        int u = i;
        for(int j = offset[u]; j < pend[u]; j ++){  // 枚举每条边，只枚举一次
            int v = edge_list[j];
            if(node_cut[v]) continue;
            if(u >= v) continue;
            for(int k = 0; k < max_color; k ++){
                int cc0 = cntGroup[j][k][0], cc1 = cntGroup[j][k][1];
                if(cc0 > 0 && cc1 == 0) c1[j] ++;           // 说明当前颜色的所有共同邻居都为属性a
                else if(cc0 == 0 && cc1 > 0) c2[j] ++;
                else if(cc0 > 0 && cc1 > 0) cboth[j] ++;
            }
            if(attribute[u] == 0) c1[j] ++;
            else c2[j] ++;
            if(attribute[v] == 0) c1[j] ++;
            else c2[j] ++;

            int need = cboth[j] - max(0, threshold - c1[j]) - max(0, threshold - c2[j]);
            if(need < 0){
                Q.push(make_pair(u, j));
                in_queue[j] = 1;
                // printf("%d %d\n", u, v);
            }
        }
    }
    
    while(!Q.empty()){
        pair<int, int> qedge = Q.front(); Q.pop();
        int u = qedge.first, v = edge_list[qedge.second];
        
        edge_cut[qedge.second] = edge_cut[unordered_edge[v][u]] = 1;    // 将删边标记置为1

        edge_set[0].clear(); edge_set[1].clear();
        for(int k = offset[u]; k < pend[u]; k ++){
            if(edge_cut[k]) continue;
            edge_set[0].insert(edge_list[k]);
        }
        for(int k = offset[v]; k < pend[v]; k ++){
            if(edge_cut[k]) continue;
            edge_set[1].insert(edge_list[k]);
        }       
       
        set<int> common_neighbor = intersection(edge_set[0], edge_set[1]); // 求交集
        for(auto w: common_neighbor){       // 枚举公共邻居
            if(node_cut[w]){
                continue;
            }
            int u1, v1, w1;
            u1 = min(u, w); w1 = max(u, w);
            int j1 = unordered_edge[u1][w1];
            if(!edge_cut[j1]){      // 讨论边<u, w>的情况
                cntGroup[j1][color[v]][attribute[v]] --;
                if(cntGroup[j1][color[v]][attribute[v]] == 0){
                    if(cntGroup[j1][color[v]][attribute[v] ^ 1] == 0){    // 说明这种颜色本来就只有一种属性
                        if(attribute[v] == 0) c1[j1] --;
                        else c2[j1] --;
                    }
                    else {          // 说明这种颜色本来有两种属性
                        cboth[j1] --;
                        if(attribute[v] == 0) c2[j1] ++;
                        else c1[j1] ++;
                    }
                }
                int need = cboth[j1] - max(0, threshold - c1[j1]) - max(0, threshold - c2[j1]);
                if(need < 0 && !in_queue[j1]){
                    Q.push(make_pair(u1, j1));
                    in_queue[j1] = 1;
                    // printf("%d %d\n", u1, w1);
                }
            }
            v1 = min(v, w); w1 = max(v, w);
            int j2 = unordered_edge[v1][w1];
            if(!edge_cut[j2]){      // 讨论边<v, w>的情况
                cntGroup[j2][color[u]][attribute[u]] --;
                if(cntGroup[j2][color[u]][attribute[u]] == 0){
                    if(cntGroup[j2][color[u]][attribute[u] ^ 1] == 0){    // 说明这种颜色本来就只有一种属性
                        if(attribute[u] == 0) c1[j2] --;
                        else c2[j2] --;
                    }
                    else {          // 说明这种颜色本来有两种属性
                        cboth[j2] --;
                        if(attribute[u] == 0) c2[j2] ++;
                        else c1[j2] ++;
                    }
                }
                int need = cboth[j2] - max(0, threshold - c1[j2]) - max(0, threshold - c2[j2]);
                if(need < 0 && !in_queue[j2]){
                    Q.push(make_pair(v1, j2));
                    in_queue[j2] = 1;
                    // printf("%d %d\n", v1, w1);
                }
            }
        }
    }

    if(cntGroup != nullptr){
        for(int i = 0; i < m; i++) {
            for(int j = 0; j < max_color; j++) {
                delete[] cntGroup[i][j];
            }
            delete[] cntGroup[i];
        }
        delete[] cntGroup;
    }
    if(c1 != nullptr) delete[] c1;
    if(c2 != nullptr) delete[] c2;
    if(cboth != nullptr) delete[] cboth;
    if(in_queue != nullptr) delete[] in_queue;

    // int ccc = 0;
    // for(int i = 0; i < m; i ++)
    //     if(edge_cut[i]) ccc ++;
    // printf("cutting %d edges.", ccc);

    // 重建图(这轮没有删点，只进行了删边)
    int startpos = 0;
    for(int i = 0; i < n; i ++){
        if(node_cut[i]) continue;
        int u = i;
        int offset_start = startpos;
        for(int j = offset[i]; j < pend[i]; j ++){
            int v = edge_list[j];
            if(node_cut[v]) continue;
            if(!edge_cut[unordered_edge[u][v]]){  // 如果这条边没有被删掉
                edge_list[startpos ++] = v;
            }
        }
        offset[i] = offset_start;
        pend[i] = startpos;
    }
    for(int i = 0; i < n; i ++){
        if(node_cut[i]) continue;
        if(offset[i] == pend[i]){       // 说明这个点的所有连边都已经被删掉，则把该点也删掉
            node_cut[i] = 1;
            offset[i] = pend[i] = 0;
        }
    }
    m = startpos;       // 更新m
    offset[n] = pend[n] = 0;                // n的offset和pend也置为0
    if(edge_cut != nullptr) delete[] edge_cut;

    printf("Enhanced Colorful Support Reduction done!\n");
    return ;
}
// 求当前图的K-Core
void Graph::K_Core_Reduction(int level){
    queue<int> Q;
    int* degree = new int [n];
    int* in_queue = new int [n];
    for(int i = 0; i < n; i ++) degree[i] = in_queue[i] = 0;
    for(int i = 0; i < n; i ++){
        if(node_cut[i]) continue;
        degree[i] = pend[i] - offset[i];
        if(degree[i] < level){
            Q.push(i);
            in_queue[i] = 1;
        }
    }

    while(!Q.empty()){
        int u = Q.front(); Q.pop();
        node_cut[u] = 1;
        for(int i = offset[u]; i < pend[u]; i ++){
            int v = edge_list[i];
            if(node_cut[v]) continue;
            degree[v] --;
            if(degree[v] < level && !in_queue[v]){
                Q.push(v);
                in_queue[v] = 1;
            }
        }
    }

    if(degree != nullptr) delete[] degree;
    if(in_queue != nullptr) delete[] in_queue;

    printf("K-Core Reduction done!\n");
    return ;
}