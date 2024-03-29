#include "Graph.h"
#include "Utility.h"
#include "Timer.h"
#include "LinearHeap.h"

using namespace std;
bool Graph::calc_base(vector<int> &R, vector<int>* C){
    int* at_cnt = new int [attr_size];
    for(int i = 0; i < attr_size; i ++) at_cnt[i] = 0;
    for(auto u : R) at_cnt[attribute[u]] ++;
    at_cnt[0] += C[0].size(); at_cnt[1] += C[1].size();
    if(at_cnt[0] + at_cnt[1] <= MRFC_real.size()) return false;
    if(at_cnt[0] < threshold || at_cnt[1] < threshold) return false;
    return true;
}

bool Graph::calc_ub(vector<int> &R, vector<int>* C, int type){
    int temp = MRFC_real.size();
    int min_ub = max(temp, threshold*2 - 1);
    if(ub_size(R, C) <= min_ub) return false;
    if(ub_color(R, C) <= min_ub) return false;
    if(ub_attr(R, C) <= min_ub) return false;
    if(ub_new(R, C) <= min_ub) return false;
    // cout << ub_size(R,C) << " " << ub_color(R,C) << " " << ub_attr(R,C) << endl;

    vector<int>* Sub = new vector<int> [2];        
    if(ub_attr_color(R, C, Sub) <= min_ub){
        delete [] Sub;
        return false;
    }

    if(type >= 0 && type <= 5) return true;

    int* deg_arr = new int [n];         
    int* color_deg_arr = new int [n];   
    for(auto u : R) RCvis[u] = 1;       
    for(auto u : C[0]) RCvis[u] = 1;
    for(auto u : C[1]) RCvis[u] = 1;
    if(type == 6 && ub_degeneracy(R, C, deg_arr, 1) <= min_ub){      // ub6
        for(auto u : R) RCvis[u] = 0;       // recover
        for(auto u : C[0]) RCvis[u] = 0;
        for(auto u : C[1]) RCvis[u] = 0;
        delete[] Sub;
        delete[] deg_arr;
        delete[] color_deg_arr;
        return false;
    }

    if(type == 7 && ub_color_degeneracy(R, C, color_deg_arr, 1) <= min_ub){ // ub7
        for(auto u : R) RCvis[u] = 0;       // recover
        for(auto u : C[0]) RCvis[u] = 0;
        for(auto u : C[1]) RCvis[u] = 0;
        delete[] Sub;
        delete[] deg_arr;
        delete[] color_deg_arr;
        return false;
    }

    if(type == 8){
        // int nonsense = ub_degeneracy(R, C, deg_arr, 0); // ub8
        if(ub_h_index(Sub, deg_arr) <= min_ub){      // ub8
            for(auto u : R) RCvis[u] = 0;       // recover
            for(auto u : C[0]) RCvis[u] = 0;
            for(auto u : C[1]) RCvis[u] = 0;
            delete[] Sub;
            delete[] deg_arr;
            delete[] color_deg_arr;
            return false;
        }
    }

    if(type == 9){
        int nonsense1 = ub_color_degeneracy(R, C, color_deg_arr, 0); // ub9
        if(ub_color_h_index(Sub, color_deg_arr) <= min_ub){  // ub9
            for(auto u : R) RCvis[u] = 0;       // recover
            for(auto u : C[0]) RCvis[u] = 0;
            for(auto u : C[1]) RCvis[u] = 0;
            delete[] Sub;
            delete[] deg_arr;
            delete[] color_deg_arr;
            return false;
        }
    }

    if(type == 10 && ub_colorful_path(R, C) <= min_ub){        // ub10
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

// 1.Size based upper bound
int Graph::ub_size(vector<int> &R, vector<int>* C){
    int ub = 0;
    ub = R.size() + C[0].size() + C[1].size();
    return ub;
}
// 2.Color based upper bound
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
// 3.Attribute based upper bound
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
// 4.Attribute Color based upper bound
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
    if(color_a != nullptr) delete[] color_a;
    if(color_b != nullptr) delete[] color_b;
    return ub;
}
// 5.new UB
int Graph::ub_new(vector<int> &R, vector<int>* C){
    int* color_a = new int [max_color];
    int* color_b = new int [max_color];
    for(int i = 0; i < max_color; i ++) color_a[i] = color_b[i] = 0;
    for(auto u : R){
        if(attribute[u] == 0) color_a[color[u]] = 1;
        else color_b[color[u]] = 1;
    }
    for(auto u : C[0]) color_a[color[u]] = 1;
    for(auto u : C[1]) color_b[color[u]] = 1;

    int cnta = 0, cntb = 0, mix = 0;
    for(int i = 0; i < max_color; i ++){
        if(color_a[i] == 1 && color_b[i] == 1){     
            mix ++;
        }
        else if(color_a[i] == 1) cnta ++;
        else if(color_b[i] == 1){
            cntb ++;
        }
    }
    if(color_a != nullptr) delete[] color_a;
    if(color_b != nullptr) delete[] color_b;

    int ub = 0;
    if((min(cnta, cntb) + mix) < (max(cnta, cntb) - delta)){
        ub = 2 * (min(cnta, cntb)+mix) + delta;
    }
    else ub = cnta + cntb + mix;
    return ub;
}

// 6.Degeneracy based upper bound
int Graph::ub_degeneracy(vector<int> &R, vector<int>* C, int* degree_arr, int flagt){
    int sigma[2]; sigma[0] = sigma[1] = 0;
    int totn = 0;
    int* ordered_c = new int [n];        
    int* vis = new int [n];              
    for(int attr = 0; attr < 2; attr ++){
        totn = 0;
        for(auto u : R){           
            if(attribute[u] == attr){  
                degree_arr[u] = 0;
                ordered_c[totn ++] = u;
                vis[u] = 1;
                for(int i = offset[u]; i < pend[u]; i ++){
                    int v = edge_list[i];
                    if(!RCvis[v] || attribute[v] != attr) continue;  
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
                if(!RCvis[v] || attribute[v] != attr) continue; 
                degree_arr[u] ++;
            }
        }
        if(!flagt) continue;

        ListLinearHeap *heap = new ListLinearHeap(totn, totn, n);
        heap -> init(totn, totn, ordered_c, degree_arr);
        for(int i = 0; i < totn; i ++){
            int nod, key;
            heap -> pop_min(nod, key);
            sigma[attr] = max(sigma[attr], key);        
            if(vis[nod]){       
                vis[nod] = 0;
                for(int j = offset[nod]; j < pend[nod]; j ++){
                    int v = edge_list[j];
                    if(!RCvis[v] || attribute[v] != attr) continue; 
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
// 7.colorful Degeneracy based upper bound
int Graph::ub_color_degeneracy(vector<int> &R, vector<int>* C, int* degree_arr, int flagt){
    int sigma[2]; sigma[0] = sigma[1] = 0;
    int totn = 0;
    int* ordered_c = new int [n];        
    int* vis = new int [n];              
    int** color_arr = new int* [n];      
    for(int attr = 0; attr < 2; attr ++){
        totn = 0;
        for(auto u : R){
            if(attribute[u] == attr){
                color_arr[u] = new int [max_color];
                for(int i = 0; i < max_color; i ++) color_arr[u][i] = 0;    
                degree_arr[u] = 0;        
                
                ordered_c[totn ++] = u;
                vis[u] = 1;
                for(int i = offset[u]; i < pend[u]; i ++){
                    int v = edge_list[i];
                    if(!RCvis[v] || attribute[v] != attr) continue; 
                    if(color_arr[u][color[v]] == 0) degree_arr[u] ++;
                    color_arr[u][color[v]] ++;
                }
            }
        }
        for(auto u : C[attr]){
            color_arr[u] = new int [max_color];
            for(int i = 0; i < max_color; i ++) color_arr[u][i] = 0;    
            degree_arr[u] = 0;        

            ordered_c[totn ++] = u;
            vis[u] = 1;
            for(int i = offset[u]; i < pend[u]; i ++){
                int v = edge_list[i];
                if(!RCvis[v] || attribute[v] != attr) continue;
                if(color_arr[u][color[v]] == 0) degree_arr[u] ++;
                color_arr[u][color[v]] ++;
            }
        }
        if(!flagt){
            for(auto u : R){
                if(attribute[u] == attr){
                    delete[] color_arr[u];
                }
            }
            for(auto u : C[attr]){
                delete[] color_arr[u];
            }
            continue;
        }

        ListLinearHeap *heap = new ListLinearHeap(totn, totn, n);
        heap -> init(totn, totn, ordered_c, degree_arr);
        for(int i = 0; i < totn; i ++){
            int nod, key;
            heap -> pop_min(nod, key);
            sigma[attr] = max(sigma[attr], key);     
            if(vis[nod]){       
                vis[nod] = 0;
                for(int j = offset[nod]; j < pend[nod]; j ++){
                    int v = edge_list[j];
                    if(!RCvis[v] || attribute[v] != attr) continue; 
                    if(vis[v] == 1){
                        color_arr[v][color[nod]] --;
                        if(color_arr[v][color[nod]] == 0){
                            heap -> decrement(v, 1);
                        }
                    }
                }
            }
        }
	    
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
// 8.H-index based upper bound
int Graph::ub_h_index(vector<int>* Sub, int* degree_arr){
    int h = 0;
    int tot_sz = Sub[0].size() + Sub[1].size();
    int* buc_cnt = new int [n];     
    for(int attr = 0; attr < 2; attr ++){
        for(int i = 0; i < Sub[attr].size(); i ++) buc_cnt[i] = 0;  
        for(auto u : Sub[attr]) buc_cnt[min(pend[u]-offset[u], tot_sz)] ++;
    }

    for(int i = tot_sz; i >= 0; i --){
        if(buc_cnt[i] >= i+1){        
            h = i+1;
            break;
        }
        if(i > 0) buc_cnt[i-1] += buc_cnt[i]; 
        else h = 1;
    }

    delete[] buc_cnt;

    int ub = h;
    return ub;
}
// 9.Colorful H-index based upper bound
int Graph::ub_color_h_index(vector<int>* Sub, int* degree_arr){
    int h[2]; h[0] = h[1] = 0;
    int* buc_cnt = new int [n];     
    for(int attr = 0; attr < 2; attr ++){
        for(int i = 0; i < Sub[attr].size(); i ++) buc_cnt[i] = 0;  
        for(auto u : Sub[attr]) buc_cnt[degree_arr[u]] ++;
        for(int i = Sub[attr].size()-1; i >= 0; i --){
            if(buc_cnt[i] >= i+1){        
                h[attr] = i+1;
                break;
            }
            if(i > 0) buc_cnt[i-1] += buc_cnt[i]; 
            else h[attr] = 1;
        }
    }

    if(buc_cnt != nullptr) delete[] buc_cnt;

    int ub = 2*min(h[0], h[1]) + delta;

    return ub;
}
// 10.colorful_path
int Graph::ub_colorful_path(vector<int> &R, vector<int>* C){
    int* f = new int [n];
    int maxu = 0;
    vector<int> ss[max_color];
    for(auto u : R) ss[color[u]].push_back(u);
    for(auto u : C[0]) ss[color[u]].push_back(u);
    for(auto u : C[1]) ss[color[u]].push_back(u);

    for(int i = 0; i < max_color; i ++){
        for(auto u : ss[i]){
            f[u] = 1;
            for(int j = offset[u]; j < pend[u]; j ++){
                int v = edge_list[j];
                if(color[v] >= color[u] || !RCvis[v]) continue; 
                f[u] = max(f[u], f[v] + 1);
            }
            maxu = max(maxu, f[u]);
        }
    }
    delete[] f;
    return maxu;
}
