//
// Created by adamv on 3/3/20.
//

#ifndef EXELIXISBASE_NS_H
#define EXELIXISBASE_NS_H


#include <vector>
#include <functional>
#include <iterator>
#include <algorithm>
#include <numeric>
#include <iostream>
#include "N.h"

using Prep = const function<bool (N*)>&;

class NS {
public:
    vector<N*> _os;
    explicit NS() = default;

    bool add(N* n);
    bool add(const vector<N*>& ns);

    template <typename DataType>
    std::tuple<N*> wrap(DataType item) {
        return std::make_tuple(new(this) N(item));
    }

    template <typename DataType, typename ...Args>
    auto wrap(DataType first, Args  ... rest) {
        return std::tuple_cat(wrap(first), wrap(rest...));
    }

    bool contains(N* to_find);

    N* get(CType data);
    N* get(Prep preposition);

    NS get_all(CType data);
    NS get_all(Prep preposition = [](N*){return true;});

    NS* clear();
    NS* clear(Prep preposition);

    void print_all(const char* sep = ", ", std::ostream& stream = std::cout);
    void print_all(Prep preposition, const char* sep = ", ", std::ostream& stream = std::cout);

    nodeit it();
    NS* from_it(nodeit it);
};



#endif //EXELIXISBASE_NS_H
