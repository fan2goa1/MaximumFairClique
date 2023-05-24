# Maximum Fair Clique
The code implementation about Efficient Algorithms for Maximum Fair Clique Search over Large Graphs.

本篇论文基于Clique提出了一个新的二属性图的高阶子图模型——Relative Fair Clique。本文致力于求出大型图中的Maximum Fair Clique。

程序分为X个部分，具体如下：

**Graph.h:**Graph类的头文件。
**Graph_reduction.cpp:**本文件含有多种归约图的方法，包括论文中的算法1、2，及其enhanced方法；K-core算法。
**Graph_heuristic.cpp: **本文件为求解Maximum Fair Clique的启发式算法，对应论文的算法7-9。
**Graph_utils.cpp:**主要为与处理图相关的接口，如预处理、输出当前图，图存储空间回收、染色等。
**Graph_ub.cpp:**用于准确计算Maximum Fair Clique时的upper bounding technology。

****

**编译命令：**g++ -std=c++11 -w main.cpp Graph_utils.cpp Graph_reduction.cpp Graph_heuristic.cpp -o main

**运行命令：**./main path/to/Graph.txt path/to/attribute.txt threshold delta
