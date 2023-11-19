#include "Utility.h"
#include "Graph.h"
#include "Timer.h"
// #define DEBUG
using namespace std;

// Alg1, Colorful attribute-degree reduction
void Graph::Colorful_Degree_Reduction(){
    printf("Start Colorful degree reduction...\n");
    
    if(!preprocessed){Preprocess(); preprocessed = 1;}
    
    if(!n || !m){
        printf("Graph is empty!\n");
        return ;
    }

    int** colorful_d = new int* [n]; 
    int*** colorful_r =  new int** [n];    
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
            if(node_cut[neighbor] == 1) continue;        
            if(colorful_r[i][attribute[neighbor]][color[neighbor]] == 0){
                colorful_d[i][attribute[neighbor]] ++;
            }
            colorful_r[i][attribute[neighbor]][color[neighbor]] ++;
        }
    }

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
                    node_cut[neighbor] = 1;      
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

    // re-construct the graph
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
            offset[i] = pend[i] = 0;        
        }
    }
    m = startpos;           
    offset[n] = pend[n] = 0;                
    
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

    int** colorful_d = new int* [m]; // colorful_d[i][j] represents the number of colors of neighbors with attribute j for the edge i.
    int*** colorful_r =  new int** [m];     // colorful_r[i][j][k] represents the count of nodes with attribute j and color k among all neighbors of edge i.
    int* edge_cut = new int [m];   // edge_retain[i] indicates whether the i-th edge has been removed (in the reduced graph)
    
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
    for(int i = 0; i < m; i ++) edge_cut[i] = 0;
    
    unordered_edge.clear();
    for(int i = 0; i < n; i ++){
        if(node_cut[i]) continue;
        for(int j = offset[i]; j < pend[i]; j ++){
            if(node_cut[edge_list[j]]) continue;
            // edge_set[i].insert(edge_list[j]);
            unordered_edge[i][edge_list[j]] = j;
        }
    }

    int** edge_set = new int* [2];
    int scnt0 = 0, scnt1 = 0;
    for(int i = 0; i < attr_size; i ++) edge_set[i] = new int [n];
    for(int i = 0; i < n; i ++){
        if(node_cut[i]) continue;
        int u = i;
        for(int j = offset[u]; j < pend[u]; j ++){
            int v = edge_list[j]; if(node_cut[v]) continue;
            if(u > v) continue;
            scnt0 = scnt1 = 0;
            for(int k = offset[u]; k < pend[u]; k ++){
                int w = edge_list[k];
                if(w <= u || w <= v || node_cut[w]) continue;
                edge_set[0][scnt0 ++] = w;
            }
            for(int k = offset[v]; k < pend[v]; k ++){
                int w = edge_list[k];
                if(w <= u || w <= v || node_cut[w]) continue;
                edge_set[1][scnt1 ++] = w;
            }
            int* common_neighbor = new int [n];
            int common_size = calc_intersection(edge_set, scnt0, scnt1, common_neighbor);
            #ifdef DEBUG
                printf("<%d, %d> common_neighbor: ", u, v);
                for(auto w: common_neighbor) printf("%d ", w);
                printf("\n");
            #endif
            for(int ll = 0; ll < common_size; ll ++){
                int w = common_neighbor[ll];
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
            if(common_neighbor != nullptr) delete[] common_neighbor;
        }
    }

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
            if(u >= v) continue;       
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

    while(!Q.empty()){
        pair<int, int> qedge = Q.front(); Q.pop();
        int u = qedge.first, v = edge_list[qedge.second];
        int u1, v1, w1;
        edge_cut[qedge.second] = edge_cut[unordered_edge[v][u]] = 1;
        scnt0 = scnt1 = 0;
        for(int k = offset[u]; k < pend[u]; k ++){
            if(edge_cut[k]) continue;
            edge_set[0][scnt0 ++] = edge_list[k];
        }
        for(int k = offset[v]; k < pend[v]; k ++){
            if(edge_cut[k]) continue;
            edge_set[1][scnt1 ++] = edge_list[k];
        }
        int* common_neighbor = new int [n];
        int common_size = calc_intersection(edge_set, scnt0, scnt1, common_neighbor);
        for(int ll = 0; ll < common_size; ll ++){
            int w = common_neighbor[ll];
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
        delete[] common_neighbor;
    }
    for(int i = 0; i < attr_size; i ++) delete[] edge_set[i];
    delete[] edge_set;
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

    int startpos = 0;
    for(int i = 0; i < n; i ++){
        if(node_cut[i]){offset[i] = pend[i] = 0; continue;}
        int u = i;
        int offset_start = startpos;
        for(int j = offset[i]; j < pend[i]; j ++){
            int v = edge_list[j];
            if(node_cut[v]) continue;
            int u1 = min(u, v), v1 = max(u, v);
            if(!edge_cut[unordered_edge[u1][v1]]){  
                edge_list[startpos ++] = v;
            }
        }
        offset[i] = offset_start;
        pend[i] = startpos;
    }
    for(int i = 0; i < n; i ++){
        if(node_cut[i]) continue;
        if(offset[i] == pend[i]){       
            node_cut[i] = 1;
            offset[i] = pend[i] = 0;
        }
    }
    m = startpos;      
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

    int*** cntGroup = new int** [n];  // cntGroup[i][j][k] represents the count of neighbors of node i with color j and attribute k.
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
            if(cc0 > 0 && cc1 == 0) c1[i] ++;           
            else if(cc0 == 0 && cc1 > 0) c2[i] ++;
            else if(cc0 > 0 && cc1 > 0) cboth[i] ++;
        }
        if(attribute[i] == 0){      
            c1[i] ++;
        }
        else c2[i] ++;
    }

    queue<int> Q;
    for(int i = 0; i < n; i ++){
        if(node_cut[i]) continue;
        int need = cboth[i] - max(0, threshold-c1[i]) - max(0, threshold-c2[i]);
        if(need < 0){          
            Q.push(i);
            node_cut[i] = 1;
        }
    }
    while(!Q.empty()){
        int u = Q.front(); Q.pop();
        for(int i = offset[u]; i < pend[u]; i ++){
            int v = edge_list[i];
            if(node_cut[v]) continue;
            if((-- cntGroup[v][color[u]][attribute[u]]) <= 0){
                if(cntGroup[v][color[u]][attribute[u] ^ 1] == 0){   
                    if(attribute[u] == 0) c1[v] --;
                    else c2[v] --;
                }
                else {         
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
            offset[i] = pend[i] = 0;        
        }
    }
    m = startpos;          
    offset[n] = pend[n] = 0;                

    cout << "Enhanced Colorful Degree Reduction done!" << endl;
    cout.flush();
    return ;
}

void Graph::Enhanced_Colorful_Support_Reduction(){
    cout << "Start Enhanced Colorful Support Reduction..." << endl;
    cout.flush();

    if(!preprocessed){Preprocess(); preprocessed = 1;}

    if(!n || !m){
        cout << "Graph is empty!" << endl;
        cout.flush();
        return ;
    }

    int*** cntGroup = new int** [m];  
    int* c1 = new int [m];          
    int* c2 = new int [m];          
    int* cboth = new int [m];      
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
    
    int** edge_set = new int* [2];
    int scnt0 = 0, scnt1 = 0;
    for(int i = 0; i < attr_size; i ++) edge_set[i] = new int [n];
    
    unordered_edge.clear();
    for(int i = 0; i < n; i ++){
        if(node_cut[i]) continue;
        for(int j = offset[i]; j < pend[i]; j ++){
            unordered_edge[i][edge_list[j]] = j;
        }
    }
    for(int i = 0; i < n; i ++){
        if(node_cut[i]) continue;
        int u = i;
        for(int j = offset[u]; j < pend[u]; j ++){
            int v = edge_list[j];
            if(node_cut[v]) continue;
            // printf("%d %d\n", u, v);
            if(u >= v) continue;    
            scnt0 = scnt1 = 0;
            for(int k = offset[u]; k < pend[u]; k ++){
                int w = edge_list[k];
                if(node_cut[w] || w <= u || w <= v) continue;
                edge_set[0][scnt0 ++] = w;
            }
            for(int k = offset[v]; k < pend[v]; k ++){
                int w = edge_list[k];
                if(node_cut[w] || w <= u || w <= v) continue;
                edge_set[1][scnt1 ++] = w;
            }
            int* common_neighbor = new int [n];
            int common_size = calc_intersection(edge_set, scnt0, scnt1, common_neighbor);
            for(int ll = 0; ll < common_size; ll ++){
                int w = common_neighbor[ll];
                cntGroup[j][color[w]][attribute[w]] ++;
                int j_uw = unordered_edge[u][w], j_vw = unordered_edge[v][w];
                cntGroup[j_uw][color[v]][attribute[v]] ++;
                cntGroup[j_vw][color[u]][attribute[u]] ++;
            }
            if(common_neighbor != nullptr) delete[] common_neighbor;
        }
    }
    queue<pair<int, int> > Q;
    bool* in_queue = new bool [m];
    for(int i = 0; i < m; i ++) in_queue[i] = 0;
    for(int i = 0; i < n; i ++){
        if(node_cut[i]) continue;
        int u = i;
        for(int j = offset[u]; j < pend[u]; j ++){  
            int v = edge_list[j];
            if(node_cut[v]) continue;
            if(u >= v) continue;
            for(int k = 0; k < max_color; k ++){
                int cc0 = cntGroup[j][k][0], cc1 = cntGroup[j][k][1];
                if(cc0 > 0 && cc1 == 0) c1[j] ++;           
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
            }
        }
    }
    
    while(!Q.empty()){
        pair<int, int> qedge = Q.front(); Q.pop();
        int u = qedge.first, v = edge_list[qedge.second];
        
        edge_cut[qedge.second] = edge_cut[unordered_edge[v][u]] = 1;    

        scnt0 = scnt1 = 0;
        for(int k = offset[u]; k < pend[u]; k ++){
            if(edge_cut[k]) continue;
            edge_set[0][scnt0 ++] = edge_list[k];
        }
        for(int k = offset[v]; k < pend[v]; k ++){
            if(edge_cut[k]) continue;
            edge_set[1][scnt1 ++] = edge_list[k];
        }       
       
        int* common_neighbor = new int [n];
        int common_size = calc_intersection(edge_set, scnt0, scnt1, common_neighbor);
        for(int ll = 0; ll < common_size; ll ++){       
            int w = common_neighbor[ll];
            if(node_cut[w]){
                continue;
            }
            int u1, v1, w1;
            u1 = min(u, w); w1 = max(u, w);
            int j1 = unordered_edge[u1][w1];
            if(!edge_cut[j1]){      
                cntGroup[j1][color[v]][attribute[v]] --;
                if(cntGroup[j1][color[v]][attribute[v]] == 0){
                    if(cntGroup[j1][color[v]][attribute[v] ^ 1] == 0){   
                        if(attribute[v] == 0) c1[j1] --;
                        else c2[j1] --;
                    }
                    else {          
                        cboth[j1] --;
                        if(attribute[v] == 0) c2[j1] ++;
                        else c1[j1] ++;
                    }
                }
                int need = cboth[j1] - max(0, threshold - c1[j1]) - max(0, threshold - c2[j1]);
                if(need < 0 && !in_queue[j1]){
                    Q.push(make_pair(u1, j1));
                    in_queue[j1] = 1;
                }
            }
            v1 = min(v, w); w1 = max(v, w);
            int j2 = unordered_edge[v1][w1];
            if(!edge_cut[j2]){      
                cntGroup[j2][color[u]][attribute[u]] --;
                if(cntGroup[j2][color[u]][attribute[u]] == 0){
                    if(cntGroup[j2][color[u]][attribute[u] ^ 1] == 0){    
                        if(attribute[u] == 0) c1[j2] --;
                        else c2[j2] --;
                    }
                    else {          
                        cboth[j2] --;
                        if(attribute[u] == 0) c2[j2] ++;
                        else c1[j2] ++;
                    }
                }
                int need = cboth[j2] - max(0, threshold - c1[j2]) - max(0, threshold - c2[j2]);
                if(need < 0 && !in_queue[j2]){
                    Q.push(make_pair(v1, j2));
                    in_queue[j2] = 1;
                }
            }
        }
        if(common_neighbor != nullptr) delete[] common_neighbor;
    }
    for(int i = 0; i < attr_size; i ++) delete[] edge_set[i];
    delete[] edge_set;

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

    int startpos = 0;
    for(int i = 0; i < n; i ++){
        if(node_cut[i]) continue;
        int u = i;
        int offset_start = startpos;
        for(int j = offset[i]; j < pend[i]; j ++){
            int v = edge_list[j];
            if(node_cut[v]) continue;
            if(!edge_cut[unordered_edge[u][v]]){  
                edge_list[startpos ++] = v;
            }
        }
        offset[i] = offset_start;
        pend[i] = startpos;
    }
    for(int i = 0; i < n; i ++){
        if(node_cut[i]) continue;
        if(offset[i] == pend[i]){       
            node_cut[i] = 1;
            offset[i] = pend[i] = 0;
        }
    }
    m = startpos;       
    offset[n] = pend[n] = 0;                
    if(edge_cut != nullptr) delete[] edge_cut;

    printf("Enhanced Colorful Support Reduction done!\n");
    return ;
}

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
