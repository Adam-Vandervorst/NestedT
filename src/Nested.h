#ifndef EXELIXISBASE_NESTED_H
#define EXELIXISBASE_NESTED_H

#include "NS.h"
#include "N.h"

#define get_sub(n) std::get<NS*>(n->data)

void draw_nested(NS* root_ns);

namespace test_graphs {
    std::tuple<NS*, NS*> simple();
    std::tuple<NS*> two_nested();
    std::tuple<NS*> double_nested();
    std::tuple<NS*> exelixis_python_solution();
}

#endif //EXELIXISBASE_NESTED_H
