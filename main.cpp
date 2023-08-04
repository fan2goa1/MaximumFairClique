#include "Graph.h"
#include "Utility.h"
#include "Timer.h"
using namespace std;

int main(int argc, char *argv[]){
    if(argc < 6){
        puts("Usage:");
        puts("./main [graph_file_path] [attribute_file_path] threshold delta [base | ub | heur] (ub_type)\n");
        return 0;
    }
    // printf("Graph=%s attribute=%s threshold=%s delta=%s\n", argv[1], argv[2], argv[3], argv[4]);
    Timer tt;
    Graph* graph = new Graph();
    graph -> alg_type = argv[5];
    if(graph -> alg_type == "base") graph -> ub_type = 0;
    else graph -> ub_type = atoi(argv[6]);
    // 读入图
    graph -> ReadGraph(argv[1], argv[2], atoi(argv[3]), atoi(argv[4]));
    // graph -> check();
    cout << endl; cout.flush();
    
    Timer TReduct;
    graph -> Colorful_Degree_Reduction();
    graph -> Enhanced_Colorful_Degree_Reduction();
    graph -> Node_Contraction();
    graph -> reColor();         // 重新染色
    cout << "After degree reduction: ";
    graph -> printGraph();
    // printf("time for enhanced degree: %s (us)\n", Utility::integer_to_string(TReduct.elapsed()).c_str());
    
    graph -> Colorful_Support_Reduction();
    graph -> Node_Contraction();        // 缩图
    graph -> reColor();                 // 重新染色
    cout << "After truss reduction: ";
    // graph -> printGraph();
    // printf("time for truss: %s (us)\n", Utility::integer_to_string(TReduct.elapsed()).c_str());
    
    graph -> Enhanced_Colorful_Support_Reduction();
    graph -> Node_Contraction();        // 缩图
    graph -> reColor();                 // 重新染色
    cout << "After enhanced truss reduction: ";
    graph -> printGraph();
    cout << endl; cout.flush();
    // printf("time for enhanced truss: %s (us)\n", Utility::integer_to_string(TReduct.elapsed()).c_str());
    
    Timer TSearch;
    if(graph -> alg_type == "heur"){
        int ub = graph -> Find_MRFC_Heuristic();     // 启发式算法找MRFC
        printf("upper bound of the MRFC is %d\n\n", ub);
    }

    puts("Start searching real MRFC...");   // ub technology
    graph -> MaxRFClique();
    graph -> printMRFC_real();
    
    printf("tot time: %s (us)\n", Utility::integer_to_string(tt.elapsed()).c_str());
    printf("time for searching MRFC: %s (us)\n", Utility::integer_to_string(TSearch.elapsed()).c_str());

    return 0;
}
