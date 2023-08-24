#include <bits/stdc++.h>

struct Edge {
    int u, v;
};

int main(int argc, char **argv) {
    srand(time(0));

    if (argc != 5) {
        std::cout << "Error! usage: ./scala [data] [attr] p [type: node/edge]" << std::endl;
    }
    
    std::string data_path = argv[1];
    std::string attr_path = argv[2];
    int p = atoi(argv[3]);

    std::vector<Edge> edges;
    std::unordered_map<int, int> attributes;
    
    std::ifstream fin1(data_path);
    int u, v;
    while (fin1 >> u >> v) {
        edges.push_back({u, v});
    }
    fin1.close();

    std::ifstream fin2(attr_path);
    int a, max_node = 0;
    while (fin2 >> u >> a) {
        attributes[u] = a;
        max_node = std::max(max_node, u);
    }
    fin2.close();

    if(argv[4][0] == 'n') {      // reduce by node
        // using random numbers to cut nodes
        std::vector<int> toDelete(max_node+2, 0);
        for (int u = 0; u <= max_node; u ++) {
            if((rand() % 100) >= p){toDelete[u] = 1;}
            else toDelete[u] = 0;
        }

        // re-number the node_id
        std::unordered_map<int, int> renumber;
        int newIndex = 0;
        for (int u = 0; u <= max_node; u ++) {
            if(toDelete[u] == 1){renumber[u] = -1; continue;}
            else renumber[u] = newIndex ++;
        }

        for (auto &edge : edges) {
            edge.u = renumber[edge.u];
            edge.v = renumber[edge.v];
        }
        
        //output
        std::sort(edges.begin(), edges.end(), [](const Edge& e1, const Edge& e2) {
            if (e1.u == e2.u) {
                return e1.v < e2.v;
            }
            return e1.u < e2.u;
        });

        std::string sp;
        std::string out_data = "pokec_bi_node_" + std::string(argv[3]) + ".txt";
        std::string out_attr = "pokec_attr_node_" + std::string(argv[3]) + ".txt";
        std::ofstream fout1(out_data);
        for (const auto edge : edges) {
            if(edge.u == -1 || edge.v == -1) continue;
            fout1 << edge.u << " " << edge.v << "\n";
        }
        fout1.close();

        std::ofstream fout2(out_attr);
        for (int u = 0; u <= max_node; u ++) {
            // fout2 << renumber[kv.first] << " " << kv.second << "\n";
            if (toDelete[u] == 1) continue;
            // std::cout << u << " " << renumber[u] << std::endl;
            fout2 << renumber[u] << " " << attributes[u] << "\n";
        }
        fout2.close();
    }

    else if (argv[4][0] == 'e') {   // reduce by edge
        
        std::string sp;
        std::string out_data = "pokec_bi_edge_" + std::string(argv[3]) + ".txt";
        std::map<int, std::map<int, int> > del_edge;
        std::vector<Edge> NewEdges;
        NewEdges.clear();

        std::string out_attr = "pokec_attr_edge_" + std::string(argv[3]) + ".txt";
        for (auto edge : edges) {
            if (edge.v <= edge.u) {
                if (del_edge[edge.v][edge.u] > 0) continue;
                else NewEdges.push_back({edge.u, edge.v});
            }
            else {
                if ((rand() % 100) >= p){
                    del_edge[edge.v][edge.u] = del_edge[edge.u][edge.v] = 1;
                    continue;
                }
                else {
                    NewEdges.push_back({edge.u, edge.v});
                }
            }
        }
        
        std::unordered_map<int, int> renumber;
        for (int u = 0; u <= max_node; u ++) renumber[u] = -1;
        int lst = -1, newIndx = 0;
        for (auto edge: NewEdges) {
            if (edge.u != lst) {
                lst = edge.u;
                renumber[lst] = newIndx ++;
            }
        }
        
        for (auto& edge: NewEdges) {
            edge.u = renumber[edge.u];
            edge.v = renumber[edge.v];
        }

        std::sort(NewEdges.begin(), NewEdges.end(), [](const Edge& e1, const Edge& e2) {
            if (e1.u == e2.u) {
                return e1.v < e2.v;
            }
            return e1.u < e2.u;
        });

        std::ofstream fout1(out_data);
        for (auto edge: NewEdges) {
            if (edge.u == -1 || edge.v == -1) continue;
            else fout1 << edge.u << " " << edge.v << "\n";
        }
        fout1.close();

        std::ofstream fout2(out_attr);
        for (int u = 0; u <= max_node; u ++) {
            if (renumber[u] < 0) continue;
            fout2 << renumber[u] << " " << attributes[u] << "\n";
        }
        fout2.close();
    }

    return 0;
}
