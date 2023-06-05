# Maximum Fair Clique

This paper proposes a new high-order subgraph model for a binary attribute graph, the Relative Fair Clique, based on the concept of a Clique. The purpose of this paper is to find the Maximum Fair Clique in large graphs.

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

| ub_type | corresponding upper-bounding technique |
| ------- | -------------------------------------- |
| 1       | ub_size                                |
| 2       | ub_color                               |
| 3       | ub_attr                                |
| 4       | ub_new                                 |
| 5       | ub_attr_color                          |
| 6       | ub_degeneracy                          |
| 7       | ub_color_degeneracy                    |
| 8       | ub_h_index                             |
| 9       | ub_color_h_index                       |
| 10      | ub_colorful_path                       |
| 11      | ub_colorful_triangle                   |

