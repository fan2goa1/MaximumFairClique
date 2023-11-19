#include "Graph.h"
#include "Utility.h"
#include "Timer.h"
#include "LinearHeap.h"

using namespace std;

void Graph::update_map(){
    unordered_edge.clear();
    for(int u = 0; u < n; u ++){
        for(int i = offset[u]; i < pend[u]; i ++){
            int v = edge_list[i]; if(v <= u) continue;
            unordered_edge[u][v] = 1;
        }
    }
    return ;
}

void Graph::MaxRFClique(){
    update_map();
    MRFC_real.clear();
    for(auto u : MRFC_heu) MRFC_real.push_back(u);      
    int* vis = new int[n];
    nvis = new int[n];
    for(int i = 0; i < n; i ++) nvis[i] = 0;
    for(int i = 0; i < n; i ++) vis[i] = 0;
    vector<int> R;
    vector<int> candidates[attr_size];
    vector<int> att_cnt;
    for(int i = 0; i < attr_size; i ++) att_cnt.push_back(0);
    int candidate_siz = 0;
    
    for(int i = 0; i < n; i ++){
        if(!vis[i]){
            component.clear();
            att_cnt[0] = att_cnt[1] = 0;
            get_connected_component(i, vis);
            R.clear(); candidates[0].clear(); candidates[1].clear();
            
            component = GetColorfulOrdering(); 
            
            for(int j = 0; j < component.size(); j ++){
                candidates[attribute[component[j]]].push_back(component[j]);
                candidate_siz ++;
            }

            bool cut_flag = true;
            if(ub_type > 5 && (this->alg_type == "heur" || this->alg_type == "ub")){     // ub tech
                cut_flag = calc_ub(R, candidates, ub_type);
            }
            if(cut_flag == false) continue;
            Branch(R, candidates, candidate_siz, att_cnt, -1, 0);
        }
    }
}

void Graph::Branch(vector<int> &R, vector<int>* candidates, int candidate_siz, vector<int> att_cnt, int attr_max, int tar_attr){
    if(attr_max == -1 && candidates[tar_attr].empty()){     
        attr_max = delta + att_cnt[tar_attr];
        Branch(R, candidates, candidate_siz, att_cnt, attr_max, 1-tar_attr);
        return ;
	}
	if(candidate_siz == 0 || (attr_max != -1 && att_cnt[tar_attr] >= attr_max)){ 
	    int toPrint = true;
	    if(toPrint && R.size() > MRFC_real.size() || !have_ans){        // update
	            MRFC_real = R;
	            have_ans = 1;
		}
        return ;
	}

    if(candidates[tar_attr].size() == 0){  
        Branch(R, candidates, candidate_siz, att_cnt, attr_max, 1-tar_attr);
        return ;
    }

	int* newCnt = new int[attr_size];

	for(int i = 0; i < candidates[tar_attr].size(); i ++){
		int cur = candidates[tar_attr][i];

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
            if(j == tar_attr){  
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
                        newCnt[j] ++;
                        candidate_siz ++;
                    }
                }
            }
		}

        bool cut_flag = true;
        if(this->alg_type == "ub" || this->alg_type == "heur"){     // ub tech

            cut_flag = calc_ub(R, newC, 0);
        }
        else if(this->alg_type == "base"){  // baseline, only examine the size
            cut_flag = calc_base(R, newC);
        }

        if(cut_flag == false){       // Pruned
            // printf("Cut! Now size: %d\n", R.size());
            // newR.pop_back();
            R.pop_back();
            for(int j = offset[cur]; j < pend[cur]; j ++){
                nvis[edge_list[j]]=0;
            }
            continue;
        }
		
		/**recover**/
		for(int j = offset[cur]; j < pend[cur]; j ++){
            nvis[edge_list[j]] = 0;
		}
		/****/
		
        att_cnt[tar_attr] ++;
        if(attr_max != -1 && newC[1-tar_attr].empty()) {
            Branch(R, newC, candidate_siz, att_cnt, attr_max, tar_attr);
        }
        else {
            Branch(R, newC, candidate_siz, att_cnt, attr_max, 1-tar_attr);
        }
        att_cnt[tar_attr] --;
        R.pop_back();
	}
	if(newCnt != nullptr) delete[] newCnt;

    return ;
}
