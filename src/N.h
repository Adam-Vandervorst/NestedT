//
// Created by adamv on 2/28/20.
//

#ifndef EXELIXISBASE_N_H
#define EXELIXISBASE_N_H

#include <vector>
#include <map>
#include <set>
#include <functional>
#include <variant>

#include "ChainIter.h"

class N;
class NS;


using std::vector, std::function, std::initializer_list;
using CType = std::variant<std::string, int, double, NS*>;
using nodeit = CIt<typename vector<N*>::iterator, N>;


namespace alg {
    template <class B>
    using Alg = function<B (N&, vector<N*>&)>;

    Alg<int> count(N* via_prop);
    Alg<double> btree_avg(N* left, N* right);
    Alg<void> reachable(std::map<N*, int>& ds);
    Alg<void> triples(std::set<std::tuple<N*, N*, N*>>& ts);
    Alg<void> cycles(vector<vector<N*>>& cs);
    Alg<void> dot(std::ostream& os_stream, std::set<N*>& viewed);
}

namespace coalg {
    template <class A>
    using CoAlg = function<N* (A&)>;

    CoAlg<int> rand_btree(NS* g, N* left, N* right);
    CoAlg<std::string> decode_pattern(NS* g, std::map<std::string, N*>& chrs);
}

class N {
    vector<unsigned int> osi;
    NS* g;
public:
    vector<N*> ps;
    vector<N*> os;
    CType data{};

    //explicit N() = default;
    template <class DataType>
    explicit N(DataType data): data(data) {};

    void* operator new(size_t size, NS* graph);
    bool operator==(const N& other) const;

    friend std::ostream& operator<<(std::ostream& os_stream, const N& n);

    nodeit operator[](nodeit ps_);
    nodeit operator[](initializer_list<N*> ps_) {
        vector<N*> v(ps_);
        return (*this)[nodeit(v.begin(), v.end())];
    };
    nodeit operator[](const N* p);
    nodeit all();

    void connect(N* p, initializer_list<N*> ns) {
        if (ns.size() == 0) return;
        int offset = 0, i = 0;
        while (i < ps.size() and ps[i] != p) offset += osi[i++];

        if (i == ps.size()) {ps.push_back(p); osi.push_back(ns.size());}
        else ++osi[i];

        os.insert(os.begin() + offset, ns.begin(), ns.end());
    }

    void connect(N* p, nodeit ns) {
        if (ns.size() == 0) return;
        int offset = 0, i = 0;
        while (i < ps.size() and ps[i] != p) offset += osi[i++];

        if (i == ps.size()) {ps.push_back(p); osi.push_back(ns.size());}
        else ++osi[i];

        os.insert(os.begin() + offset, ns.begin(), ns.end());
    }

    void disconnect(N* p, initializer_list<N*> ns) {
        int offset = 0, i = 0, j = 0;
        while (i < ps.size() and ps[i] != p) offset += osi[i++];
        while (j < osi[i]) {
            if (std::find(ns.begin(), ns.end(), os[offset + j]) != ns.end()) {
                os.erase(os.begin() + offset + j);
                --osi[i];
            } else {
                ++j;
            }
        }
    }

    void disconnect(N* p, nodeit ns) {
        int offset = 0, i = 0, j = 0;
        while (i < ps.size() and ps[i] != p) offset += osi[i++];
        while (j < osi[i]) {
            if (std::find(ns.begin(), ns.end(), os[offset + j]) != ns.end()) {
                os.erase(os.begin() + offset + j);
                --osi[i];
            } else {
                ++j;
            }
        }
    }

    bool same_source(N* other) {
        return this->g == other->g;
    }

    template <class B, class Iterable>
    B histo(const alg::Alg<B>& alg, Iterable descend, vector<N*> line = {}) {
        if (find(line.begin(), line.end(), this) != line.end()) return alg(*this, line);
        line.push_back(this);

        auto r(*this);
        (*this)[descend].map([&, alg, descend, line](N* n){return new (g) N(n->histo(alg, descend, line));}, r[descend]);

        line.pop_back();
        return alg(r, line);
    }

    template <class B>
    B histo(const alg::Alg<B>& alg, vector<N*> line = {}) {
        if (find(line.begin(), line.end(), this) != line.end()) return alg(*this, line);
        line.push_back(this);

        auto r(*this);
        this->all().map([&, alg, line](N* n){return new (g) N(n->histo(alg, line));}, r.all());

        line.pop_back();
        return alg(r, line);
    }

    template<class Iterable>
    void histo(const alg::Alg<void>& alg, Iterable descend, vector<N*> line = {}) {
        if (find(line.begin(), line.end(), this) != line.end()) return alg(*this, line);
        line.push_back(this);

        (*this)[descend].foreach([alg, descend, line](N* n){n->histo(alg, descend, line);});

        line.pop_back();
        return alg(*this, line);
    }

    void histo(const alg::Alg<void>& alg, vector<N*> line = {}) {
        if (find(line.begin(), line.end(), this) != line.end()) return alg(*this, line);
        line.push_back(this);

        this->all().foreach([alg, line](N* n){n->histo(alg, line);});

        line.pop_back();
        return alg(*this, line);
    }

    template <class A>
    N* ana(const coalg::CoAlg<A>& coalg, initializer_list<N*> descend) {
        auto r = coalg(std::get<A>(this->data));

        (*r)[descend].mapped([coalg, descend](N* n){return n->ana(coalg, descend);});

        return r;
    }

    template <class A>
    N* ana(const coalg::CoAlg<A>& coalg) {
        auto r = coalg(std::get<A>(this->data));

        r->all().mapped([coalg](N* n){return n->ana(coalg);});

        return r;
    }
};


#endif //EXELIXISBASE_N_H
