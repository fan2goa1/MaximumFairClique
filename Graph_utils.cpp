#include "Utility.h"
#include "Graph.h"
#include "LinearHeap.h"
#include "Timer.h"
// #define DEBUG
using namespace std;


Graph::Graph(){
    attr_size = 2;
    n = m = threshold = delta = max_color = preprocessed = 0;
    offset = edge_list = attribute = pend = color = node_cut = nullptr;
    enhanced_degree = to_old_node = nullptr;
    color_degree = nullptr;
    RCvis = nullptr;
}
Graph::~Graph(){
    if(offset != nullptr) delete[] offset;
    if(edge_list != nullptr) delete[] edge_list;
    if(attribute != nullptr) delete[] attribute;
    if(pend != nullptr) delete[] pend;
    if(color != nullptr) delete[] color;
    if(node_cut != nullptr) delete[] node_cut;
    if(enhanced_degree != nullptr) delete[] enhanced_degree;
    if(to_old_node != nullptr) delete[] to_old_node;
    if(color_degree != nullptr) delete[] color_degree;
}

void Graph::printGraph(){
    printf("<node num: %d   edge num: %d>\n", n, m);
    printf("Left nodes: ");
    for(int i = 0; i < n; i ++){
        if(!node_cut[i]) printf("%d ", i);
    } puts("");
    printf("Edge list: ");
    for(int i = 0; i < n; i ++){
        if(node_cut[i]) continue;
        for(int j = offset[i]; j < pend[i]; j ++){
            printf("<%d, %d>, ", i, edge_list[j]);
        }
    } printf("Total: %d\n", m);

    // for(int i = 0; i < n; i ++){
    //     printf("new: %d; old: %d\n", i, to_old_node[i]);
    // }
    printf("The number of colors: %d\n", max_color);

    return ;
}
// 用于检查当前图中对于无向边(u, v)是不是都出现了<u, v>和<v, u>
void Graph::check(){
    unordered_edge.clear();
    for(int i = 0; i < n; i ++){
        if(node_cut[i]) continue;
        for(int j = offset[i]; j < pend[i]; j ++){
            int v = edge_list[j];
            if(node_cut[v]) continue;
            unordered_edge[i][v] = 1;
        }
    }
    for(int i = 0; i < n; i ++){
        if(node_cut[i]) continue;
        for(int j = offset[i]; j < pend[i]; j ++){
            int v = edge_list[j];
            if(node_cut[v]) continue;
            if(unordered_edge[v][i] != 1){puts("Check Fail!"); return ;}
        }
    }
    puts("OK");
    return ;
}


// 输入图，无向图，边已经按第一个点的顺序从小到大排好， 同时必须保证第一点相同的情况下第二个点从小到大排好序
void Graph::ReadGraph(const char* inputname, const char* attr, int th, int _delta){
    threshold = th;
    delta = _delta;
    printf("Start reading graph from %s\n", inputname);
    FILE *f = fopen(inputname, "r");
    if(f == nullptr){
        printf("Can not open the graph file!\n");
        exit(1);
    }
    vector<pair<int, int> > pairs;
    int from, to;
    while(fscanf(f, "%d%d", &from, &to) == 2){
        if(from == to) continue;
        pairs.push_back(make_pair(from, to));
    }
    for(int i = 0; i < pairs.size(); i++){
        if(pairs[i].first > n) n = pairs[i].first;
        if(pairs[i].second > n) n = pairs[i].second;
        if(i > 0 && pairs[i].first == pairs[i - 1].first && pairs[i].second == pairs[i - 1].second) printf("Exist self loop!\n");
    }
    n ++;           //range from [0, n-1];
    m = pairs.size();   // range from [0, m-1]
    printf("< Number of nodes= %d, number of edges= %d >\n", n, m);

    // 保存边
    edge_list = new int [m + 1];
    offset = new int [n + 1];
    pend = new int [n + 1];
    offset[0] = 0;
    int indx = 0;
    for(int i = 0; i < n ; i++){
        offset[i + 1] = offset[i];
        while(indx < pairs.size() && pairs[indx].first == i){
            edge_list[offset[i + 1] ++] = pairs[indx ++].second;
        }
        pend[i] = offset[i + 1];    // 点u的边在数组中的范围[offset[u], pend[u])
    }
    fclose(f);
    node_cut = new int [n];
    for(int i = 0; i < n; i ++) node_cut[i] = 0;
    to_old_node = new int [n];
    for(int i = 0; i < n; i ++) to_old_node[i] = i;     // 最开始每个点的id都表示原图中的自己
    RCvis = new int [n];
    for(int i = 0; i < n; i ++) RCvis[i] = 0;

    // 读入点的属性
    attribute = new int[n];
    f = fopen(attr, "r");
    if(f == nullptr){
        printf("Can not open the attribute file\n");
        exit(1);
    }
    int node, a;
    attr_size = 0;
    while(fscanf(f, "%d%d", &node, &a) == 2){
        if(node >= n){
        //    printf("attribute error! node=%d\n",node);
        //    exit(1);
            continue;
        }
        if(a >= 2){printf("Exist more than 2 attributes!"); exit(0);}
        attribute[node] = a;
        if(a > attr_size) attr_size = a;
    }
    attr_size ++;
    fclose(f);
    printf("read done\n");
}

// 图预处理. 进行简单的（关于度的）剪枝和贪心法图染色
void Graph::Preprocess(){  
    int* cvis = new int[n];
    color = new int[n];
    int* head = new int[n];
    int* nxt = new int[n];
    int max_degree = 0;             // 所有点的最大度数
    int* degree = new int[n];
    // 将度数不满足k*2-1的点直接删除
    for(int i = 0; i < n; i ++){
        degree[i] = offset[i + 1] - offset[i];
        head[i] = n;
    }
    int cut_thre = threshold * attr_size - 1;
    int first_reduction = 0;
    for(int i = 0; i < n; i ++){
        if(degree[i] < cut_thre){
            node_cut[i] = 1;
            first_reduction ++;
            continue;
        }
        nxt[i] = head[degree[i]];
		head[degree[i]] = i;
        if(degree[i] > max_degree) max_degree = degree[i];
    }
    if(degree != nullptr) delete[] degree;
    // 进行贪心法的图染色
    for(int i = 0; i < n; i ++) cvis[i] = 0;    // 表示颜色i还没被用过
    for(int i = 0; i < n; i ++) color[i] = n;   // n表示还没有被染色
	// 依次从最大度数开始决定每个点的颜色
	max_color = 0;
	for(int ii = max_degree; ii >= 1; ii --){
		for(int jj = head[ii]; jj != n; jj = nxt[jj]){
			int u = jj;
			for(int j = offset[u]; j < offset[u+1]; j ++) {
                int c = color[edge_list[j]];
                if(c != n) {        // 如果点u的邻居已经被染色了，这个颜色不能用
                    cvis[c] = 1;
                }
			}
			for(int j = 0; ; j ++){     // 找一个没被用过的颜色
                if(!cvis[j]){
                    color[u] = j;       // 将点u染成颜色j
                    if(j > max_color) max_color = j;
                    break;
			    }
            }
			for(int j = offset[u]; j < offset[u + 1]; j ++){
                int c = color[edge_list[j]];
                if(c != n) cvis[c] = 0;     // 复原
			}
		}
	}
    // for(int i = 0; i < n; i ++) printf("%d ", color[i]); puts("");
    max_color ++;
    if(cvis != nullptr) delete[] cvis;
    if(head != nullptr) delete[] head;
    if(nxt != nullptr) delete[] nxt;
    preprocessed = 1;
    printf("Preprocess done\n");
    return ;
}

void Graph::reColor(){
    Node_Contraction();             // 先缩图保证新图中每个点都没被删
    int* cvis = new int[n];
    int* head = new int[n];
    int* nxt = new int[n];
    int max_degree = 0;             // 所有点的最大度数
    int* degree = new int[n];

    for(int i = 0; i < n; i ++){
        degree[i] = pend[i] - offset[i];
        head[i] = n;
    }
    for(int i = 0; i < n; i ++){
        nxt[i] = head[degree[i]];
		head[degree[i]] = i;
        if(degree[i] > max_degree) max_degree = degree[i];
    }
    if(degree != nullptr) delete[] degree;

    // 进行贪心法的图染色
    for(int i = 0; i < n; i ++) cvis[i] = 0;    // 表示颜色i还没被用过
    for(int i = 0; i < n; i ++) color[i] = n;   // n表示还没有被染色
	// 依次从最大度数开始决定每个点的颜色
	max_color = 0;
	for(int ii = max_degree; ii >= 1; ii --){
		for(int jj = head[ii]; jj != n; jj = nxt[jj]){
			int u = jj;
			for(int j = offset[u]; j < offset[u+1]; j ++) {
                int c = color[edge_list[j]];
                if(c != n) {        // 如果点u的邻居已经被染色了，这个颜色不能用
                    cvis[c] = 1;
                }
			}
			for(int j = 0; ; j ++){     // 找一个没被用过的颜色
                if(!cvis[j]){
                    color[u] = j;       // 将点u染成颜色j
                    if(j > max_color) max_color = j;
                    break;
			    }
            }
			for(int j = offset[u]; j < offset[u + 1]; j ++){
                int c = color[edge_list[j]];
                if(c != n) cvis[c] = 0;     // 复原
			}
		}
	}
    // for(int i = 0; i < n; i ++) printf("%d ", color[i]); puts("");
    max_color ++;
    if(cvis != nullptr) delete[] cvis;
    if(head != nullptr) delete[] head;
    if(nxt != nullptr) delete[] nxt;
    printf("Recolor the Graph done.\n");
    return ;
}

// 用于把已经被删掉的点从一些信息中彻底删除，以节约空间
void Graph::Node_Contraction(){
    int* node_mapper = new int[n];
    int* tmp = new int[n];
    for(int i = 0; i < n; i ++) node_mapper[i] = -1;
    for(int i = 0; i < n; i ++) tmp[i] = to_old_node[i];
    int new_n = 0;
    for(int i = 0; i < n; i ++){
        if(node_cut[i]) continue;
        node_mapper[i] = new_n;
        offset[new_n] = offset[i];
        pend[new_n] = pend[i];
        attribute[new_n] = attribute[i];
        color[new_n] = color[i];

        to_old_node[new_n] = tmp[i];
        new_n ++;
    }
    for(int i = 0; i < m; i ++){
        if(node_mapper[edge_list[i]] == -1) continue;
        edge_list[i] = node_mapper[edge_list[i]];
    }
    // 将node_cut清零
    for(int i = 0; i < n; i ++) node_cut[i] = 0;
    
    n = new_n;

    if(node_mapper != nullptr) delete[] node_mapper;
    if(tmp != nullptr) delete[] tmp;
    return ;
}

void Graph::get_connected_component(int root, int* vis){   // 找到连通分量，vis=0表示该点还剩下
    stack<int> Q;
    Q.push(root);
    vis[root] = 1;
    int maxQ = 0;
    while(!Q.empty()){
        int cur = Q.top(); Q.pop();
        component.push_back(cur);
        for(int i = offset[cur]; i < pend[cur]; i ++){
            if(!vis[edge_list[i]]){
                Q.push(edge_list[i]);
                vis[edge_list[i]] = 1;
            }
        }
    }
}
// 求当前图的Colorful attribute degree
void Graph::get_colorful_attr_degree(){
    color_degree = new int* [n];            // color_degree[i][j]表示点i的属性为j的邻居的颜色数
    int*** colorful_r =  new int** [n];     // colorful_r[i][j][k]表示点i的所有邻居中，属性为j，颜色为k的点的个数
    for(int i = 0; i < n; i ++){
        color_degree[i] = new int[attr_size];
        for(int j = 0; j < attr_size; j ++)
            color_degree[i][j] = 0;
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
                color_degree[i][attribute[neighbor]] ++;
            }
            colorful_r[i][attribute[neighbor]][color[neighbor]] ++;
        }
    }
    if(colorful_r != nullptr) delete[] colorful_r;
    return ;
}
// 求出连通分量的Colorful Degree Ordering
vector<int> Graph::GetColorfulOrdering(){
    int* vis = new int[n];
    for(int i = 0; i < n; i ++) vis[i]=0;
    for(int i = 0; i < component.size(); i ++) vis[component[i]] = 1;

    int*** d;//fairness degree
    int** r;

    int* min_d;//minimum among all attributes
    int* component_idx = new int[n];
    for(int i = 0; i < component.size(); i ++) component_idx[component[i]] = i;
    min_d = new int[n];
    for(int i = 0; i < n; i ++) min_d[i] = n;
    r = new int* [component.size()];
    for(int i = 0; i < component.size(); i ++){
        r[i] = new int[attr_size];
        for(int j = 0; j < attr_size; j ++)
            r[i][j] = 0;
    }
    d = new int** [component.size()];
    for(int i = 0; i < component.size(); i ++){
        d[i] = new int*[attr_size];
        for(int j = 0; j < attr_size; j ++){
            d[i][j] = new int [max_color];
            for(int k = 0; k < max_color; k ++)
                d[i][j][k] = 0;
        }
    }

    for(int i = 0; i < component.size(); i ++){
        int cnow = component[i];
        for(int j = offset[cnow]; j < pend[cnow]; j ++){
            int nei = edge_list[j];
            if(vis[nei] == 1)   // 在当前这个连通分量中
                if((d[i][attribute[nei]][color[nei]] ++) == 0)
                    r[i][attribute[nei]] ++;
        }

        for(int j = 0; j < attr_size; j ++){
            if(r[i][j] < min_d[i]) min_d[i] = r[i][j];
        }
    }

    int* degree_arr = new int[component.size()];

    for(int i = 0; i < component.size(); i ++) degree_arr[i] = min_d[i];
    int* ordered_c = new int[component.size()];
    for(int i = 0; i < component.size(); i ++) ordered_c[i] = i;
    int cnt = 0;

    ListLinearHeap *heap = new ListLinearHeap(component.size(), component.size(), n);
    heap -> init(component.size(), component.size(), ordered_c, degree_arr);
    vector<int> peeling_order;peeling_order.clear();
    for(int i = 0; i < component.size(); i ++){
        int nod, key;
        heap -> pop_min(nod, key);
        nod = component[nod];
        //printf("nod=%d, key=%d\n", nod, key);
        peeling_order.push_back(nod);
        vis[nod] = 0;
        for(int j = offset[nod]; j < pend[nod]; j ++){
            if(vis[edge_list[j]] == 1){
                int idx = component_idx[edge_list[j]];
                if(-- d[idx][attribute[nod]][color[nod]] <= 0){
                    r[idx][attribute[nod]] --;
                    if(min_d[idx] > r[idx][attribute[nod]]){
                        int off = min_d[idx] - r[idx][attribute[nod]];
                        min_d[idx] = r[idx][attribute[nod]];
                        heap -> decrement(idx, off);  // O(1)时间
                    }
                }
            }
        }
    }

    if(d != nullptr) delete[] d;
    if(r != nullptr) delete[] r;
    if(min_d != nullptr) delete[] min_d;
    if(vis != nullptr) delete[] vis;
    if(component_idx != nullptr) delete[] component_idx;
    if(degree_arr != nullptr) delete[] degree_arr;
    if(ordered_c != nullptr) delete[] ordered_c;    

    return peeling_order;
}

void Graph::printMRFC_real(){
    printf("The size of real MRFC: %d\n", MRFC_real.size());
    for(auto u : MRFC_real){
        printf("%d ", to_old_node[u]);
    } puts("");
    return ;
}
