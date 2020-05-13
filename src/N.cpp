#include <map>
#include <set>
#include <string>
#include <experimental/random>
#include "N.h"
#include "NS.h"

using namespace std;
using std::experimental::randint;


namespace alg {
    #define show(x) reinterpret_cast<uintptr_t>(x)

    Alg<int> count(N* via_prop) {
        return [via_prop](N& n, vector<N*>& line){
            if (find(line.begin(), line.end(), &n) != line.end()) return 0;
            else return n[via_prop].fold([](int a, N *b) { return a + get<int>(b->data); }, 1);
        };
    }

    Alg<double> btree_avg(N* left, N*right) {
        return [left, right](N& n, vector<N*>& line){
            if (n.data.index() == 1) return (double)get<int>(n.data);
            return (get<double>((*n[left])->data) + get<double>((*n[right])->data))/2.;
        };
    }

    Alg<void> reachable(map<N*, int>& ds) {
        return [&](N& n, vector<N*>& line){
            int d = line.size();
            ds[&n] = ds.contains(&n) ? min(ds[&n], d) : d;
        };
    }

    Alg<void> triples(set<tuple<N*, N*, N*>>& ts) {
        return [&](N& s, vector<N*>& line){
            for (N* p : s.ps) for (N* o : s[p]) ts.emplace(&s, p, o);
        };
    }

    Alg<void> cycles(vector<vector<N*>>& cs) {
        return [&](N& n, vector<N*>& line){
            auto occ = find(line.begin(), line.end(), &n);
            if (occ != line.end()) cs.emplace_back(occ, line.end());
        };
    }

    Alg<void> dot(ostream& os_stream, set<N*>& viewed) {
        return [&](N& s, vector<N*>& line){
            if (!viewed.insert(&s).second) return;
            os_stream << show(&s) << " [label=" << s << "];\n";
            for (N* p : s.ps) for (N* o : s[p])
                os_stream << show(&s) << " -> " << show(o) << " [label=" << *p << "];\n";
        };
    }
}

namespace coalg {
    CoAlg<int> rand_btree(NS* g, N* left, N* right) {
        return [g, left, right](int total){
            if (total > 1) {
                int l = randint(1, total - 1), r = total - l;

                auto n = new(g) N("Mul");
                n->connect(left, {new(g) N(l)});
                n->connect(right, {new(g) N(r)});
                return n;
            } else return new(g) N(randint(1, 10));
        };
    }

    CoAlg<string> decode_pattern(NS* g, map<std::string, N*>& chrs) {
        return [g, chrs](string& p){
            if (p.empty()) return new(g) N("");
            else if (std::isalnum(p[0])) {
                const auto c = string(1, p[0]);
                auto cur = new(g) N("term-" + c);
                cur->connect(chrs.at(c), {new (g) N(p.substr(1))});
                return cur;
            } else if (p[0] == '.') {
                auto cur = new(g) N("any");
                for (auto& [c, n] : chrs)
                    cur->connect(n, {new (g) N(p.substr(1))});
                return cur;
            } else if (p[0] == '(' and p.find(')') != string::npos) {
                auto k = p.find_first_of(')');
                auto cur = new(g) N("group-" + p.substr(1, k - 1));
                return cur;
            } else {
                return new (g) N("Error");
            }
        };
    }
}


void* N::operator new(size_t size, NS* graph) {
    auto n = malloc(size);
    auto c_n = static_cast<N*>(n);
    c_n->g = graph; graph->add(c_n);
    return n;
}

bool N::operator==(const N& other) const {
    return this == &other;
}

ostream& operator<<(ostream& os_stream, const N& n) {
    visit([&](auto &&data){
        using T = decay_t<decltype(data)>;
        if constexpr (is_same_v<T, int>) os_stream << data;
        else if constexpr (is_same_v<T, double>) os_stream << data;
        else if constexpr (is_same_v<T, std::string>) os_stream << '"' << data << '"';
        else if constexpr (is_same_v<T, NS*>) os_stream << "\"Nested\"";
    }, n.data);

    return os_stream;
}

nodeit N::operator[](nodeit ps_) {
    if (ps_.size() == 0) return nodeit(os.end(), os.end());

    auto start_it = (*this)[*ps_.begin()];

    auto last = &start_it;
    for (auto p : ps_)
        if (p != *ps_.begin())
            last = last->chain((*this)[p]);

    return start_it;
};

nodeit N::operator[](const N* p) {
    if (p == nullptr) return (*this).all();

    int offset = 0, i = 0;
    while (i < ps.size() and ps[i] != p) offset += osi[i++];

    if (offset == os.size()) return nodeit(os.end(), os.end());
    return nodeit(os.begin() + offset, os.begin() + offset + osi[i]);
}

nodeit N::all() {
    return nodeit(os.begin(), os.end());
}
