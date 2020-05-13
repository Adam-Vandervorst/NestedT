#include <iostream>
#include <iomanip>
#include <chrono>
#include "N.h"
#include "NS.h"
#include "Rewriting.h"
#include "Nested.h"

using std::cout, std::endl;
using namespace std::chrono;


namespace tests {
    void general_check() {
        auto g = new NS();

        auto p1 = new(g) N("p1"), p2 = new(g) N("p2");
        auto n1 = new(g) N("A"), n2 = new(g) N("B"), n3 = new(g) N("C"), n4 = new(g) N("D");
        auto n5 = new(g) N("BA"), n6 = new(g) N("BB");
        auto n7 = new(g) N("BBB");

        n1->connect(p2, {n4});
        n1->connect(p1, {n2, n3});
        n2->connect(p1, {n5, n6});
        n6->connect(p1, {n7});
        n6->connect(p2, {n7});

        n7->connect(p1, {n2});


        cout << "size p1 DFS spanning tree: " << n1->histo(alg::count(p1)) << endl << endl;

        std::map<N *, int> m;
        n1->histo(alg::reachable(m));
        for (auto &[n, hops] : m)
            cout << hops << " hops to reach " << *n << endl;
        cout << endl;

        std::set<std::tuple<N *, N *, N *>> ts;
        n1->histo(alg::triples(ts));
        for (auto[s, p, o] : ts)
            cout << *s << "  |" << *p << ">  " << *o << endl;
        cout << endl;

        std::vector<vector<N *>> cs;
        n1->histo(alg::cycles(cs));
        for (auto &c : cs) {
            for (auto &n : c) cout << *n << " -> ";
            cout << *c[0] << endl;
        }

        delete g->clear();
    }

    void disconnect_edge() {
        auto g = new NS();
        auto p = new(g) N("p"), q = new(g) N("q"), r = new(g) N("r");
        auto n1 = new(g) N("x"), n2 = new(g) N("y"), n3 = new(g) N("z");

        n1->connect(p, {n2});
        n1->connect(q, {n2, n3});
        n1->connect(r, {n3});

        n1->disconnect(q, {n2, n3});
        n1->disconnect(r, {n3});
        n1->connect(q, {n3});

        std::set<N*> viewed;
        cout << "digraph T {" << endl;
        n1->histo(alg::dot(cout, viewed));
        cout << "}" << endl;

        delete g->clear();
    }

    void select_props() {
        auto g = new NS();

        auto p = new(g) N("p"), q = new(g) N("q"), r = new(g) N("r");
        auto _1 = new(g) N(1), _2 = new(g) N(2), _3 = new(g) N(3), _4 = new(g) N(4), _5 = new(g) N(5);
        _1->connect(p, {_1, _2, _3});
        _1->connect(q, {_1, _4});
        _1->connect(r, {_5});

        for (auto n : (*_1)[{}]) cout << ": " << *n << endl;
        // should not print
        for (auto n : (*_1)[{r}]) cout << ": " << *n << endl;
        cout << endl;
        for (auto n : (*_1)[{p, q}]) cout << ": " << *n << endl;
        cout << endl;
        for (auto n : (*_1)[{q, p}]) cout << ": " << *n << endl;
        
        delete g->clear();
    }

    void descend_props() {
        auto g = new NS();

        auto p = new(g) N("p"), has = new(g) N("has instance");
        auto p1 = new(g) N("p1"), p2 = new(g) N("p2");
        auto n1 = new(g) N("A"), n2 = new(g) N("B"), n3 = new(g) N("C"), n4 = new(g) N("D");
        auto n5 = new(g) N("BA"), n6 = new(g) N("BB");
        auto n7 = new(g) N("BBB");

        p->connect(has, {p1, p2});

        n1->connect(p2, {n4});
        n1->connect(p1, {n2, n3});
        n2->connect(p1, {n5, n6});
        n6->connect(p1, {n7});
        n6->connect(p2, {n7});

        n7->connect(p1, {n2});

        std::map<N*, int> m;
        n1->histo<std::initializer_list<N*>>(alg::reachable(m), {p1});
        for (auto &[n, hops] : m)
            cout << hops << " hops to reach " << *n << endl;
        cout << endl; m.clear();
        n1->histo<std::initializer_list<N*>>(alg::reachable(m), {p2});
        for (auto &[n, hops] : m)
            cout << hops << " hops to reach " << *n << endl;
        cout << endl; m.clear();
        n1->histo(alg::reachable(m), (*p)[has]);
        for (auto &[n, hops] : m)
            cout << hops << " hops to reach " << *n << endl;
        delete g->clear();
    }

    void hylo_performance() {
        //             ana       cata      hylo
        // C++ T      6159555   3626070    9497513
        // Python    33156637  10476905   46028639
        // Python T 697323327 668245419 1467391795
        auto expr = new NS();
        auto left = new(expr) N("Left"), right = new(expr) N("Right");

        auto start = high_resolution_clock::now();
        auto res = (new(expr) N(1000000))->
                ana(coalg::rand_btree(expr, left, right))->
                histo(alg::btree_avg(left, right));
        auto stop = high_resolution_clock::now();

        auto duration = duration_cast<nanoseconds>(stop - start);
        cout << res << ", " << duration.count() << endl;

        delete expr->clear();
    }

    void ns_features() {
        auto g = new NS();

        auto p = new(g) N("p"), q = new(g) N("q"), r = new(g) N("r");
        auto _1 = new(g) N(1), _2 = new(g) N(2), _3 = new(g) N(3), _4 = new(g) N(4), _5 = new(g) N(5);
        auto z0 = new(g) N(0), z1 = new(g) N(0), to_delete = new(g) N(0);
        _1->connect(p, {_1, _2, _3});
        _1->connect(q, {_1, _4});
        _1->connect(r, {_5});
        _2->connect(r, {_5});
        _4->connect(p, {_1});

        g->print_all();
        cout << *g->get(1) << endl;
        g->get_all([](N* n){return n->data.index() == 0;}).print_all();
        g->clear([to_delete](N* n){return n == to_delete;});
        g->print_all([](N* n){if (auto v = get_if<int>(&n->data)) return *v == 0; return false;});

        auto h = new NS();

        auto a = new(h) N("a"), b = new(h) N("b"), c = new(h) N("c"), _h = new(h) N(0);

        cout << g->contains(a) << ", " << g->contains(z0);

        delete h->clear();
        delete g->clear();
    }

    void variadic_wrap() {
        auto g = new NS();

        auto [n1, n2] = g->wrap(4, "Test");
        cout << *n1 << ", " << *n2 << endl;

        delete g->clear();
    }

    void parse_regex() {
        // TODO WIP, if you're interested, I have this working in Python
        auto pattern = new NS();

        std::map<std::string, N *> chrs = {{"t", new(pattern) N("t")},
                                           {"e", new(pattern) N("e")},
                                           {"0", new(pattern) N("0")},
                                           {"1", new(pattern) N("1")},
        };
        /*for(int a = 0; a < 256; ++a) {
            auto c = std::string(1, a);
            if (std::isprint(a)) chrs[c] = new(pattern) N(c);
        }*/

        auto test_pattern = new(pattern) N("t(0|1)");
        auto res = test_pattern->ana(coalg::decode_pattern(pattern, chrs));
        auto test_to_match = new(pattern) N("t");

        std::set<std::tuple<N *, N *, N *>> ts;
        res->histo(alg::triples(ts));
        for (auto[s, p, o] : ts)
            cout << std::setw(3) << *s << "  |" << *p << ">  " << *o << endl;
        cout << endl;


        delete pattern->clear();
    }

    void to_dot() {
        auto g = new NS();

        auto p = new(g) N("p"), has = new(g) N("has instance");
        auto p1 = new(g) N("p1"), p2 = new(g) N("p2");
        auto n1 = new(g) N("A"), n2 = new(g) N("B"), n3 = new(g) N("C"), n4 = new(g) N("D");
        auto n5 = new(g) N("BA"), n6 = new(g) N("BB");
        auto n7 = new(g) N("BBB");

        p->connect(has, {p1, p2});

        n1->connect(p2, {n4});
        n1->connect(p1, {n2, n3});
        n2->connect(p1, {n5, n6});
        n6->connect(p1, {n7});
        n6->connect(p2, {n7});

        n7->connect(p1, {n2});

        std::set<N*> viewed;
        cout << "digraph T {" << endl;
        n1->histo(alg::dot(cout, viewed));
        p->histo(alg::dot(cout, viewed));
        cout << "}" << endl;

        delete g->clear();
    }

    void nest() {
        auto [g, to_match, _] = test_graphs::nat_has();

        delete g->clear(), to_match->clear();
    }

    void nested_to_dot() {
        auto [f, _] = test_graphs::simple();
        auto [g] = test_graphs::two_nested();
        auto [h] = test_graphs::double_nested();

        cout << "digraph g {" << endl;
        cout << "compound=true;" << endl;
        cout << "overlap = false;\nsplines = true;" << endl;
        draw_nested(f);
        draw_nested(g);
        draw_nested(h);
        cout << "}" << endl;

        delete f->clear(), g->clear(), h->clear();
    }

    void rewrite() {
        NS* rewrite_context = new NS();
        N* m = new (rewrite_context) N("match");
        auto [g, to_match] = test_graphs::nat_has_nested();

        std::vector<N*> selected;
        for (N* possible_placeholder : to_match->it())
            if (not possible_placeholder->ps.empty())
                selected.push_back(possible_placeholder);

        for (N* placeholder : selected)
            for (N* node : g->it())
                if (select_possible(placeholder, node, m, false))
                    placeholder->connect(m, {node});

        for (N* placeholder : selected)
            placeholder->histo<nodeit>(alg::checks_out(m), to_match->it());

        auto mappings = possible_mappings(to_match, m);

        for (N* placeholder : to_match->_os)
            placeholder->disconnect(m, (*placeholder)[m]);

        apply_mapping(g, mappings[0]);

#if false
        for (auto &mapping : possible_mappings(to_match, m)) {
            for (auto [from, to] : mapping)
                cout << *from << " -> " << *to << endl;
            cout << endl;
        }
#else
        std::set<N*> viewed; std::vector<N*> empty ={};
        cout << "digraph g {" << endl;
        cout << "compound=true;" << endl;
        draw_nested(g);
        cout << "edge [color=red];" << endl;
        cout << "node [color=red];" << endl;
        draw_nested(to_match);
        cout << "}" << endl;
#endif

        delete to_match->clear();
        delete g->clear();
        delete rewrite_context->clear();
    }
}


int main() {
    tests::general_check();
    return 0;
}
