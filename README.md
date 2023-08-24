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

## About Dataset

You can find and download these datasets in [this repo](https://github.com/fan2goa1/Attributed_Graph_Data).

**dblp_aminer:**

dblp_aminer is a real dataset. It includes tens of thousands scholars and pictures their social network.

The binary attribute is about gender, while 0 represents female and 1 represents male.

**dblp_DB/AI:**

This is a dataset extracted from dblp data. 

This undirected graph represents the cooperative relationship between scholars. If two scholars have published the same paper, there is an edge between the two corresponding points, otherwise there is no.

The binary attribute is about the number of published papers, while if a scholar has published more papers in the area of database than that of AI, the corresponding attribute is 1, otherwise is 0.

**dblp_DB/DM:**

The graph is totally the same with dblp_DB/DM. The only difference is attribute. If a scholar has published more papers in the area of database than that of DM, the corresponding attribute is 1, otherwise is 0.

**dblp_year:**

The graph is totally the same with dblp_DB/DM. The only difference is attribute. If a scholar published his first paper before 2008, the corresponding attribute is 1, meaning he/she is a senior scholar, otherwise is 0.

**NBA:**

This is a real dataset. This undirected graph represents the cooperative relationship between NBA players. The binary attribute is about nationalities. If a player is from America, the corresponding attribute is 0, otherwise is 1.

**IMDB:**

This is a real dataset.
