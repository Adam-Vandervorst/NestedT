//
// Created by adamv on 3/23/20.
//

#ifndef EXELIXISBASE_CHAINITER_H
#define EXELIXISBASE_CHAINITER_H

#include <iterator>
#include <numeric>


template <class It, class A>
class CIt{
private:
    It _start;
    It _end;
    It _curr;
    CIt* next = nullptr;
public:
    typedef A* value_type;
    typedef std::ptrdiff_t difference_type;
    typedef A** pointer;
    typedef A*& reference;
    typedef std::input_iterator_tag iterator_category;

    explicit CIt(It start, It end) : _start(start), _end(end), _curr(start) {}
    A* operator*() const { return *_curr; }
    bool operator==(const CIt& other) const { return _curr == other._curr and next == other.next; }
    bool operator!=(const CIt& other) const { return !(*this == other); }

    CIt* chain(CIt next_it) {
        next = new CIt(next_it._start, next_it._end);
        return next;
    }

    CIt& operator++() {
        ++_curr;
        if (next != nullptr and _curr == _end) *this = *next;
        return *this;
    }

    unsigned int size() {
        auto s = std::distance(_start, _end);
        CIt* nex = this;
        while (nex->next != nullptr) {nex = nex->next; s += std::distance(nex->_start, nex->_end);}
        return s;
    }

    CIt begin() {
        CIt cpy(*this);
        cpy._curr = cpy._start;
        return cpy;
    }

    CIt end() {
        CIt* nex = this;
        while (nex->next != nullptr) nex = nex->next;
        auto cpy(*nex);
        cpy._curr = cpy._end;
        return cpy;
    }

    template <class F>
    A reduce(F f) {
        return std::accumulate(_start, _end, *_start, f);
    }

    template <class T, class F>
    T fold(F f, T initial) {
        return std::accumulate(_start, _end, initial, f);
    }

    template <class F>
    CIt& foreach(F f) {
        std::for_each(_start, _end, f);
        return *this;
    }

    template <class F>
    CIt map(F f, CIt output) {
        for (auto *v : *this) {*output._curr = f(v); ++output;}
        return output.begin();
    }

    template <class F>
    CIt& mapped(F f) { // eq to map(f, *this)
        for (auto *v : *this) *v = *f(v);
        return *this;
    }
};

#endif //EXELIXISBASE_CHAINITER_H
