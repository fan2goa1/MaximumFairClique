#include "Graph.h"
#include "Utility.h"
#include "Timer.h"
#include "LinearHeap.h"

using namespace std;
// 用于计算多个upper bound，如果被剪枝了，就返回false，否则返回true
bool Graph::calc_ub(vector<int> &R, vector<int>* C){
    int min_ub = MRFC_heu.size();
    if(ub_size(R, C) <= min_ub) return false;
    if(ub_color(R, C) <= min_ub) return false;
    if(ub_attr(R, C) <= min_ub) return false;
    // cout << ub_size(R,C) << " " << ub_color(R,C) << " " << ub_attr(R,C) << endl;

    vector<int>* Sub = new vector<int> [2];         // 分别为R C中属性为0/1的子图的点
    if(ub_attr_color(R, C, Sub) <= min_ub){
        delete [] Sub;
        return false;
    }
    
    // // 构建属性子图需要用到的东西
    int* deg_arr = new int [n];         // Ga/Gb中的点的度数
    int* color_deg_arr = new int [n];   // Ga/Gb中的点的colorful degree
    for(auto u : R) RCvis[u] = 1;       // 标记是否为R C中的点
    for(auto u : C[0]) RCvis[u] = 1;
    for(auto u : C[1]) RCvis[u] = 1;
    // if(ub_degeneracy(R, C, deg_arr) <= min_ub){
    //     for(auto u : R) RCvis[u] = 0;       // recover
    //     for(auto u : C[0]) RCvis[u] = 0;
    //     for(auto u : C[1]) RCvis[u] = 0;
    //     delete[] Sub;
    //     delete[] deg_arr;
    //     delete[] color_deg_arr;
    //     return false;
    // }
    // if(ub_color_degeneracy(R, C, color_deg_arr) <= min_ub){
    //     for(auto u : R) RCvis[u] = 0;       // recover
    //     for(auto u : C[0]) RCvis[u] = 0;
    //     for(auto u : C[1]) RCvis[u] = 0;
    //     delete[] Sub;
    //     delete[] deg_arr;
    //     delete[] color_deg_arr;
    //     return false;
    // }
    
    // if(ub_h_index(Sub, deg_arr) <= min_ub){
    //     for(auto u : R) RCvis[u] = 0;       // recover
    //     for(auto u : C[0]) RCvis[u] = 0;
    //     for(auto u : C[1]) RCvis[u] = 0;
    //     delete[] Sub;
    //     delete[] deg_arr;
    //     delete[] color_deg_arr;
    //     return false;
    // }
    // if(ub_color_h_index(Sub, color_deg_arr) <= min_ub){
    //     for(auto u : R) RCvis[u] = 0;       // recover
    //     for(auto u : C[0]) RCvis[u] = 0;
    //     for(auto u : C[1]) RCvis[u] = 0;
    //     delete[] Sub;
    //     delete[] deg_arr;
    //     delete[] color_deg_arr;
    //     return false;
    // }
    // 先对Sub[0]和Sub[1]中的点按照颜色
    sort(Sub[0].begin(), Sub[0].end(), [&](int a, int b){return color[a] < color[b];});
    sort(Sub[1].begin(), Sub[1].end(), [&](int a, int b){return color[a] < color[b];});
    if(ub_colorful_path(Sub) <= min_ub){
        for(auto u : R) RCvis[u] = 0;       
        for(auto u : C[0]) RCvis[u] = 0;
        for(auto u : C[1]) RCvis[u] = 0;
        delete[] Sub;
        delete[] deg_arr;
        delete[] color_deg_arr;
        return false;
    }
    /* recover */
    for(auto u : R) RCvis[u] = 0;       
    for(auto u : C[0]) RCvis[u] = 0;
    for(auto u : C[1]) RCvis[u] = 0;

    delete[] Sub;
    delete[] deg_arr;
    delete[] color_deg_arr;
    return true;
}

// 1.求Size based upper bound
int Graph::ub_size(vector<int> &R, vector<int>* C){
    int ub = 0;
    ub = R.size() + C[0].size() + C[1].size();
    return ub;
}
// 2.求Color based upper bound
int Graph::ub_color(vector<int> &R, vector<int>* C){
    int ub = 0;
    int* color_cnt = new int[max_color];
    for(int i = 0; i < max_color; i++) color_cnt[i] = 0;
    for(int attr = 0; attr < 2; attr ++){
        for(auto u : C[attr]){
            if(color_cnt[color[u]] == 0) ub ++;
            color_cnt[color[u]] ++;
        }
    }
    ub = ub + R.size();
    if(color_cnt != nullptr) delete[] color_cnt;
    return ub;
}
// 3.求Attribute based upper bound
int Graph::ub_attr(vector<int> &R, vector<int>* C){
    int ub = 0;
    int* attr_cnt = new int[attr_size];
    for(int i = 0; i < attr_size; i ++) attr_cnt[i] = 0;
    for(auto u : R) attr_cnt[attribute[u]] ++;
    attr_cnt[0] += C[0].size(); attr_cnt[1] += C[1].size();
    if(abs(attr_cnt[0] - attr_cnt[1]) <= delta) ub = attr_cnt[0] + attr_cnt[1];
    else ub = min(attr_cnt[0], attr_cnt[1])*2 + delta;
    if(attr_cnt != nullptr) delete[] attr_cnt;
    return ub;
}
// 4.求Attribute Color based upper bound
int Graph::ub_attr_color(vector<int> &R, vector<int>* C, vector<int>* Sub){
    int ub = 0;
    int cnta = 0, cntb = 0;
    int* color_a = new int[max_color];
    int* color_b = new int[max_color];
    for(int i = 0; i < max_color; i ++) color_a[i] = color_b[i] = 0;
    
    for(auto u : R){
        if(attribute[u] == 0){      // 属性为a
            Sub[0].push_back(u);
            if(color_a[color[u]] == 0) cnta ++;
            color_a[color[u]] ++;
        }
        else {                      // 属性为b
            Sub[1].push_back(u);
            if(color_b[color[u]] == 0) cntb ++;
            color_b[color[u]] ++;
        }
    }
    for(auto u : C[0]){
        Sub[0].push_back(u);
        if(color_a[color[u]] == 0) cnta ++;
        color_a[color[u]] ++;
    }
    for(auto u : C[1]){
        Sub[1].push_back(u);
        if(color_b[color[u]] == 0) cntb ++;
        color_b[color[u]] ++;
    }
    if(abs(cnta - cntb) <= delta) ub = cnta + cntb;
    else ub = min(cnta+1, cntb+1)*2 + delta;
    // cout << ub << endl;
    if(color_a != nullptr) delete[] color_a;
    if(color_b != nullptr) delete[] color_b;
    return ub;
}

// 5.求Degeneracy based upper bound
int Graph::ub_degeneracy(vector<int> &R, vector<int>* C, int* degree_arr){
    int sigma[2]; sigma[0] = sigma[1] = 0;
    int totn = 0;
    int* ordered_c = new int [n];        // ordered_c为备选点下标
    int* vis = new int [n];              // vis[i]=1表示点i还没有被删除
    for(int attr = 0; attr < 2; attr ++){
        totn = 0;
        for(auto u : R){
            if(attribute[u] == attr){   // u在R C中且属性一致
                degree_arr[u] = 0;
                ordered_c[totn ++] = u;
                vis[u] = 1;
                for(int i = offset[u]; i < pend[u]; i ++){
                    int v = edge_list[i];
                    if(!RCvis[v] || attribute[v] != attr) continue;  // 说明v不是R C中的点或者属性不对
                    degree_arr[u] ++;
                }
            }
        }
        for(auto u : C[attr]){
            degree_arr[u] = 0;
            ordered_c[totn ++] = u;
            vis[u] = 1;
            for(int i = offset[u]; i < pend[u]; i ++){
                int v = edge_list[i];
                if(!RCvis[v] || attribute[v] != attr) continue; // 说明v不是R C中的点或者属性不对
                degree_arr[u] ++;
            }
        }

        ListLinearHeap *heap = new ListLinearHeap(totn, totn, n);
        heap -> init(totn, totn, ordered_c, degree_arr);
        for(int i = 0; i < totn; i ++){
            int nod, key;
            heap -> pop_min(nod, key);
            sigma[attr] = max(sigma[attr], key);        // 取退化度的最大值
            if(vis[nod]){       // 说明还没被删
                vis[nod] = 0;
                for(int j = offset[nod]; j < pend[nod]; j ++){
                    int v = edge_list[j];
                    if(!RCvis[v] || attribute[v] != attr) continue; // 说明v不是R C中的点或者属性不对
                    if(vis[v] == 1){
                        heap -> decrement(v, 1);
                    }
                }
            }
        }
        delete heap;
    }
	if(ordered_c != nullptr) delete[] ordered_c;
	if(vis != nullptr) delete[] vis;
    
    int ub = 2 * min(sigma[0]+1, sigma[1]+1) + delta;

    return ub;
}

int Graph::ub_color_degeneracy(vector<int> &R, vector<int>* C, int* degree_arr){
    int sigma[2]; sigma[0] = sigma[1] = 0;
    int totn = 0;
    int* ordered_c = new int [n];        // ordered_c为备选点下标
    int* vis = new int [n];              // vis[i]=1表示点i还没有被删除
    int** color_arr = new int* [n];      // color_arr[i][j]表示i点的颜色j的个数
    for(int attr = 0; attr < 2; attr ++){
        totn = 0;
        for(auto u : R){
            if(attribute[u] == attr){
                color_arr[u] = new int [max_color];
                for(int i = 0; i < max_color; i ++) color_arr[u][i] = 0;    // 初始化
                degree_arr[u] = 0;        // colorful degree初始化
                
                ordered_c[totn ++] = u;
                vis[u] = 1;
                for(int i = offset[u]; i < pend[u]; i ++){
                    int v = edge_list[i];
                    if(!RCvis[v] || attribute[v] != attr) continue; // 说明v不是R C中的点或者属性不对
                    if(color_arr[u][color[v]] == 0) degree_arr[u] ++;
                    color_arr[u][color[v]] ++;
                }
            }
        }
        for(auto u : C[attr]){
            color_arr[u] = new int [max_color];
            for(int i = 0; i < max_color; i ++) color_arr[u][i] = 0;    // 初始化
            degree_arr[u] = 0;        // colorful degree初始化

            ordered_c[totn ++] = u;
            vis[u] = 1;
            for(int i = offset[u]; i < pend[u]; i ++){
                int v = edge_list[i];
                if(!RCvis[v] || attribute[v] != attr) continue;
                if(color_arr[u][color[v]] == 0) degree_arr[u] ++;
                color_arr[u][color[v]] ++;
            }
        }

        ListLinearHeap *heap = new ListLinearHeap(totn, totn, n);
        heap -> init(totn, totn, ordered_c, degree_arr);
        for(int i = 0; i < totn; i ++){
            int nod, key;
            heap -> pop_min(nod, key);
            sigma[attr] = max(sigma[attr], key);     // 取退化度的最大值
            if(vis[nod]){       // 说明还没被删
                vis[nod] = 0;
                for(int j = offset[nod]; j < pend[nod]; j ++){
                    int v = edge_list[j];
                    if(!RCvis[v] || attribute[v] != attr) continue; // 说明v不是R C中的点或者属性不对
                    if(vis[v] == 1){
                        color_arr[v][color[nod]] --;
                        if(color_arr[v][color[nod]] == 0){
                            heap -> decrement(v, 1);
                        }
                    }
                }
            }
        }
        // 释放指针
        for(auto u : R){
            if(attribute[u] == attr){
                delete[] color_arr[u];
            }
        }
        for(auto u : C[attr]){
            delete[] color_arr[u];
        }
        delete heap;
    }

    if(ordered_c != nullptr) delete[] ordered_c;
    if(vis != nullptr) delete[] vis;
    if(color_arr != nullptr) delete[] color_arr;

    int ub = 2 * min(sigma[0]+1, sigma[1]+1) + delta;

    return ub;
}

int Graph::ub_h_index(vector<int>* Sub, int* degree_arr){
    int h[2]; h[0] = h[1] = 0;
    int* buc_cnt = new int [n];     // buc_cnt[i]表示degree=i的点的个数
    // cout << Sub[0].size() << " " << Sub[1].size() << endl;
    for(int attr = 0; attr < 2; attr ++){
        for(int i = 0; i < Sub[attr].size(); i ++) buc_cnt[i] = 0;  // 初始化
        for(auto u : Sub[attr]) buc_cnt[degree_arr[u]] ++;
        // for(int i = 0; i < Sub[attr].size(); i ++) printf("%d ", buc_cnt[i]); puts("");
        for(int i = Sub[attr].size()-1; i >= 0; i --){
            if(buc_cnt[i] >= i){        // 说明至少有i个点的degree>=i
                h[attr] = i;
                break;
            }
            if(i > 0) buc_cnt[i-1] += buc_cnt[i]; // 否则，将degree=i的点的个数加到degree=i-1的点的个数上
            else h[attr] = 0;
        }
    }

    delete[] buc_cnt;

    int ub = 2*min(h[0]+1, h[1]+1) + delta;
    // cout << ub << " " << Sub[0].size() << " " << Sub[1].size()<< endl;
    return ub;
}

int Graph::ub_color_h_index(vector<int>* Sub, int* degree_arr){
    int h[2]; h[0] = h[1] = 0;
    int* buc_cnt = new int [n];     // buc_cnt[i]表示degree=i的点的个数
    for(int attr = 0; attr < 2; attr ++){
        for(int i = 0; i < Sub[attr].size(); i ++) buc_cnt[i] = 0;  // 初始化
        for(auto u : Sub[attr]) buc_cnt[degree_arr[u]] ++;
        for(int i = Sub[attr].size()-1; i >= 0; i --){
            if(buc_cnt[i] >= i){        // 说明至少有i个点的degree>=i
                h[attr] = i;
                break;
            }
            if(i > 0) buc_cnt[i-1] += buc_cnt[i]; // 否则，将degree=i的点的个数加到degree=i-1的点的个数上
            else buc_cnt[attr] = 0;
        }
    }

    if(buc_cnt != nullptr) delete[] buc_cnt;

    int ub = 2*min(h[0]+1, h[1]+1) + delta;
    return ub;
}
// 求colorful_path的上界
int Graph::ub_colorful_path(vector<int>* Sub){
    int* f = new int [n];
    // 先分开考虑两个属性
    int* maxf = new int [2]; maxf[0] = maxf[1] = 0;
    for(int attr = 0; attr < 2; attr ++){
        for(auto u : Sub[attr]){
            f[u] = 1;
            for(int i = offset[u]; i < pend[u]; i ++){
                int v = edge_list[i];
                if(color[v] >= color[u] || attribute[v] != attr || !RCvis[v]) continue;  // 只找color value更小的点
                f[u] = max(f[u], f[v] + 1);
            }
            maxf[attr] = max(maxf[attr], f[u]);
        }
    }
    

    int ub = 2*min(maxf[0], maxf[1]) + delta;
    delete[] f;
    delete[] maxf;
    
    // 将两个属性合在一起考虑🤔️
    f = new int [n];
    int maxu = 0;
    vector<int> ss;
    for(auto u : Sub[0]) ss.push_back(u);
    for(auto u : Sub[1]) ss.push_back(u);
    sort(ss.begin(), ss.end(), [&](int a, int b){return color[a] < color[b];});
    for(int i = 0; i < ss.size(); i ++){
        int u = ss[i];
        f[u] = 1;
        for(int j = offset[u]; j < pend[u]; j ++){
            int v = edge_list[j];
            if(color[v] >= color[u] || !RCvis[v]) continue;  // 只找color value更小的点
            f[u] = max(f[u], f[v] + 1);
        }
        maxu = max(maxu, f[u]);
    }
    // if(maxu < ub) puts("all");
    // else if(maxu == ub) puts("equal");
    // else puts("repective");
    
    return min(ub, maxu);
}