#include "Graph.h"
#include "Utility.h"
#include "Timer.h"
#include "LinearHeap.h"

using namespace std;

void Graph::MaxRFClique(){
    MRFC_real = MRFC_heu;       // 用于存储真正的MRFC
    int* vis = new int[n];
    nvis = new int[n];
    for(int i = 0; i < n; i ++) nvis[i] = 0;
    for(int i = 0; i < n; i ++) vis[i] = 0; // 0表示还没被枚举
    vector<int> R, X;
    vector<int> candidates[attr_size];
    vector<int> att_cnt;
    for(int i = 0; i < attr_size; i ++) att_cnt.push_back(0);
    int candidate_siz = 0;
    
    for(int i = 0; i < n; i ++){
        if(!vis[i]){
            component.clear();
            get_connected_component(i, vis);
            R.clear(); X.clear();
            
            component = GetColorfulOrdering();
            
            for(int j = 0; j < component.size(); j ++){
                candidates[attribute[component[j]]].push_back(component[j]);
                candidate_siz ++;
            }
            // for(auto u : candidates[0]) printf("%d ", u); puts("");
            // for(auto u : candidates[1]) printf("%d ", u); puts("");
            Branch(R, candidates, candidate_siz, X, att_cnt, -1, 0);
        }
    }
}

void Graph::Branch(vector<int> &R, vector<int>* candidates, int candidate_siz, vector<int> X, vector<int> att_cnt, int attr_max, int tar_attr){
    if(attr_max == -1 && candidates[tar_attr].size() == 0){     // 说明该属性无法继续扩展，设置attr_max
		attr_max = delta + att_cnt[tar_attr];
        Branch(R, candidates, candidate_siz, X, att_cnt, attr_max, tar_attr ^ 1);
        return ;
	}
    // cout << "Now R size: " << R.size() << endl;
	if(candidate_siz == 0 || (attr_max != -1 && att_cnt[tar_attr] >= attr_max)){ // 候选集为空或者封顶了
		int toPrint = true;
		if(X.size() != 0){
			//如果加一个进去，却不会破坏它的公平性，就不是答案
			int attr_min = attr_max - delta;
			for(int i = 0; i < X.size(); i ++){
				if(att_cnt[attribute[X[i]]]==attr_min || att_cnt[attribute[X[i]]] + 1 <= attr_max){
					toPrint = false;
                    break;
				}
			}
		}
		if(toPrint){
            // printf("Found one! Size: %d; Now max size: %d\n", R.size(), MRFC_real.size());
            if(R.size() >= MRFC_real.size()) MRFC_real = R;
		}
        return ;
	}

    if(candidates[tar_attr].size() == 0){   // 说明该属性无法继续扩展，同时attr_max已经被设置，且没有达到封顶
        Branch(R, candidates, candidate_siz, X, att_cnt, attr_max, tar_attr ^ 1);
        return ;
    }

    // vector<int> newR = R;
	int* newCnt = new int[attr_size];

	for(int i = 0; i < candidates[tar_attr].size(); i ++){
		int cur = candidates[tar_attr][i];
		// newR.push_back(cur);
        R.push_back(cur);
		for(int j = offset[cur]; j < pend[cur]; j ++){
            nvis[edge_list[j]] = 1;
        }
		// construct new candidates
		vector<int> newC[attr_size]; 
		for(int j = 0; j < attr_size; j ++) newCnt[j] = 0;
		candidate_siz = 0;
		for(int j = 0; j < attr_size; j ++){
            if(attr_max != -1 && att_cnt[j] >= attr_max){
                continue;
            }
            if(j == tar_attr){  // 当前属性，只能选更后面的点
                for(int k = i + 1; k<candidates[j].size(); k ++){
                    if(nvis[candidates[j][k]]){
                        newC[j].push_back(candidates[j][k]);
                        newCnt[j] ++;
                        candidate_siz ++;
                    }
                }
            }
            else{
                for(int k = 0; k < candidates[j].size(); k++){
                    if(nvis[candidates[j][k]]){
                        newC[j].push_back(candidates[j][k]);
                        newCnt[j]++;
                        candidate_siz++;
                    }
                }
            }
		}

        // bool cut_flag = calc_ub(newR, newC);
        bool cut_flag = calc_ub(R, newC);
        // cout << cut_flag << endl;
        if(cut_flag == false){       // 被剪枝
            // printf("Cut! Now size: %d\n", R.size());
            // newR.pop_back();
            R.pop_back();
            for(int j = offset[cur]; j < pend[cur]; j ++){
                nvis[edge_list[j]]=0;
            }
            continue;
        }
		
        vector<int> newX; newX.clear();
		for(int j = 0; j < X.size(); j ++){
            if(nvis[X[j]]) newX.push_back(X[j]);
		}
		/**recover**/
		for(int j = offset[cur]; j < pend[cur]; j ++){
            nvis[edge_list[j]] = 0;
		}
		/****/
		// Branch(newR, newC, candidate_siz, newX, att_cnt, attr_max, tar_attr ^ 1);
        Branch(R, newC, candidate_siz, newX, att_cnt, attr_max, tar_attr ^ 1);
		X.push_back(cur);
		// newR.pop_back();
        R.pop_back();
	}
	if(newCnt != nullptr) delete[] newCnt;

    return ;
}