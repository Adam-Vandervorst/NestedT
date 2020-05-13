#ifndef EXELIXISBASE_REWRITING_H
#define EXELIXISBASE_REWRITING_H

#include "NS.h"
#include "N.h"


void apply_mapping(NS* graph, std::map<N*, N*>& mapping);

vector<std::map<N*, N*>> possible_mappings(NS* subgraph, N* m);

bool select_possible(N* placeholder, N* node, N*, bool);

namespace alg {
    alg::Alg<void> checks_out(N* match_edge);
}

namespace test_graphs {
    std::tuple<NS*, NS*, NS*> nat_has(bool = true);
    std::tuple<NS*, NS*> nat_has_nested();
    std::tuple<NS*, NS*, NS*, NS*> nested_rewrite_example();
}

#endif //EXELIXISBASE_REWRITING_H
