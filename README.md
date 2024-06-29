# [Efficient Maximum Fair Clique Search over Large Networks](https://arxiv.org/abs/2312.04088)

This is the repository corresponding to the paper "Efficient Maximum Fair Clique Search over Large Networks". This paper proposes a new high-order subgraph model for a binary attribute graph, the Relative Fair Clique, based on the concept of Clique. The purpose of this paper is to find the Maximum Fair Clique in large graphs.

## Code Structure

The code structure can be listed as follows.

**Timer.h:** Timer class, used for counting the program running time.

**Utility.h:** Utility class, used for operating strings.

**LinearHeap.h:** It is a linear heap that can both insert and check the smallest number in the heap in $O(1)$ time complexity.

**Graph.h:** The header file of Graph class.

**Graph_reduction.cpp:** This file contains several methods for graph reduction, including Algorithms 1 and 2 in the paper, and their enhanced methods, as well as the K-core algorithm.

**Graph_heuristic.cpp:** This file provides heuristic algorithms for finding the Maximum Fair Clique, corresponding to Algorithms 7-9 in the paper.

**Graph_utils.cpp:** This mainly includes interfaces for processing graphs, such as preprocessing, outputting the current graph, recycling graph storage space, coloring, etc.

**Graph_ub.cpp:** This is used for accurately calculating the Maximum Fair Clique using upper bounding technology.

**Graph_baseline.cpp:** This is the precise algorithm for calculating the Maximum Fair Clique (Branch-and-bound solution).

**main.cpp:** The main program.



## How to Run the Code

**Compile command:** ```make```

  In the present study, we employ three distinct algorithms: the Baseline Algorithm, the Upper-Bounding Solution, and the Heuristic Algorithm.

  For the application of the Baseline Algorithm, the ensuing command is necessitated: ```./main path/to/Graph.txt path/to/attribute.txt threshold delta base```

  Conversely, to implement the Upper-Bounding Solution, the required command is as follows:```./main path/to/Graph.txt path/to/attribute.txt threshold delta ub ub_type```. 

  Lastly, the Heuristic Algorithm can be activated via the succeeding command: ```./main path/to/Graph.txt path/to/attribute.txt threshold delta heur ub_type``` 

  The specific corresponding relationship of ub_type is as follows.

| ub_type | corresponding upper-bounding technique | Notation in Paper |
| ------- | -------------------------------------- | ----------------- |
| 1       | ub_size                                | $ub_s$            |
| 2       | ub_color                               | $ub_c$            |
| 3       | ub_attr                                | $ub_a$            |
| 4       | ub_new                                 | $ub_{eac}$        |
| 5       | ub_attr_color                          | $ub_{ac}$         |
| 6       | ub_degeneracy                          | $ub_{\Delta}$  |
| 7       | ub_color_degeneracy                    | $ub_{cd}$         |
| 8       | ub_h_index                             | $ub_{h}$          |
| 9       | ub_color_h_index                       | $ub_{ch}$         |
| 10      | ub_colorful_path                       | $ub_{cp}$         |

## About Dataset

We employ six real-world graphs to evaluate the efficiency of the proposed algorithms. The statistics of these datasets are summarized in table below.

| **Dataset** | $n = \|V\|$ | $m=\|E\|$    | $d_{max}$ | Description       |
| ----------- | --------- | ---------- | --------- | --------------------- |
| Themarker   | 69,414    | 3,289,686  | 8,930     | Social network        |
| Google      | 875,713   | 8,644,102  | 6,332     | Web network           |
| DBLP        | 1,843,615 | 16,700,518 | 2,213     | Collaboration network |
| Flixster    | 2,523,387 | 15,837,602 | 1,474     | Social network        |
| Pokec       | 1,632,803 | 44,603,928 | 14,854    | Social network        |
| Aminer      | 423,469   | 2,462,224  | 712       | Collaboration network |

Among them, Google is a web graph, Aminer and DBLP are collaboration networks, and the remaining datasets are social networks. Aminer is an attributed graph where the attribute indicates the gender of scholars and can be downloaded from [FairLaR](https://github.com/SotirisTsioutsiouliklis/FairLaR/). The other datasets are non-attributed graphs and can be obtained from [networkrepository](networkrepository.com/) and [SNAP](snap.stanford.edu). For these non-attributed graphs,
we construct attribute graphs by randomly assigning attributes to vertices with approximately equal probability to evaluate the efficiency of all algorithms. Some of the attributed graphs can be found in [this repo](https://github.com/fan2goa1/Attributed_Graph_Data).

## Citation
```
@misc{zhang2023efficientmaximumfairclique,
      title={Efficient Maximum Fair Clique Search over Large Networks}, 
      author={Qi Zhang and Rong-Hua Li and Zifan Zheng and Hongchao Qin and Ye Yuan and Guoren Wang},
      year={2023},
      eprint={2312.04088},
      archivePrefix={arXiv},
      primaryClass={cs.DB},
      url={https://arxiv.org/abs/2312.04088}, 
}
```
