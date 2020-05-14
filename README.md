# Nested T-graph
A prototype for nested graphs where edges are tagged with other nodes.

This concept is explained on [my research blog](https://research.adamv.be/#Unifyingrichgraphswithedgestoedges).

This implementation is competing with [HierarchyT](https://github.com/Adam-Vandervorst/HierarchyT).
## Motivation
The basis is extremely small and flexible. T-Graphs contain a vector of nodes and nodes can store atoms (strings, integers, doubles and functions) or other T-Graphs. A node is connected to a node *via* another node. This allows you to associate arbitrary information with an edge.

Aside of this there are (for now) two ways to do computations intrinsic to the graph: graph rewriting and recursion schemes.
Both are explained and motivated beautifully [here](https://blog.opencog.org/2013/03/24/why-hypergraphs/) and [here](https://jtobin.io/time-traveling-recursion) respectively.

## Usage
This is an **extremely experimental** implementation and has absolutely **no guarantees**. 

- T-Graphs are created with `NS* g = new NS();` and destroyed with `delete g->clear();`
- Nodes are spawned with `auto p = new(g) N("p")` and can contain `std::string, int, double, NS*`
- If you have a lot of nodes to define, you can use `auto [_1, _2, _3] = g->wrap(1, 2, 3);`
- A node can be connected to other nodes by writing `_1->connect(p, {_2. _3});`
- Similarly  can disconnect the edge with property `p` from `a` to `b` by `a.disconnect(p, {b});`
- `n[{p1, p2}]` gives you an iterator to all the nodes that are connect with properties `p1` and `p2`.

## TODO's
Open to pull-requests.

- [ ] Add documentation and code comments in the frequent code paths
- [ ] Implement a [NEAT](http://nn.cs.utexas.edu/downloads/papers/stanley.ec02.pdf)-like neural-network
- [ ] Construct more elaborate benchmarks including ones for graph-rewriting
- [ ] Add a function-type to the allowed node storage types
- [ ] Allow for more heterogeneous computation, using node-centric operations
- [ ] Add more TODO's
