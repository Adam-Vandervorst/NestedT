#include "Nested.h"
using namespace std;


#define show(x) reinterpret_cast<uintptr_t>(x)

void gather(NS* root, map<N*, NS*>& all) {
    root->it().foreach([&](N* n){visit([&](auto &&data){
        if constexpr (is_same_v<decay_t<decltype(data)>, NS*>) {
            all.insert({n, data});

            cout << "subgraph cluster" << show(data) << "{" << endl;
            cout << "label=\"\";" << endl;
            cout << show(n) << " [style=invis, shape=point, margin=0];\n";
            gather(data, all);
            cout << "}" << endl;
        } else {
            all.insert({n, nullptr});
            cout << show(n) << " [label=" << *n << "];\n";
        }
    }, n->data);});
}

void draw_nested(NS* root) {
    map<N*, NS*> all;
    gather(root, all);

    for (auto [n, n_graph] : all) for (N* p : n->ps) for (N* o : (*n)[p]) {
        NS* o_graph = all[o];
        if (not n_graph and o_graph and not o_graph->_os.empty()) o = o_graph->_os[0];

#if false // fdp
        if (not n_graph and not o_graph) cout << show(n) << " -> " << show(o);
        else if (n_graph and not o_graph) cout << "cluster" << show(n_graph) << " -> " << show(o);
        else if (not n_graph and o_graph) cout << show(n) << " -> " << "cluster" << show(o_graph);
        else cout << "cluster" << show(n_graph) << " -> " << "cluster" << show(o_graph);
        cout << " [label=" << *p << "];\n";
#else
        cout << show(n) << " -> " << show(o) << " [label=" << *p;
        if (n_graph) cout << ", ltail=cluster" << show(n_graph);
        if (o_graph) cout << ", lhead=cluster" << show(o_graph);
        cout << "];\n";
#endif
    }
}


namespace test_graphs {
    std::tuple<NS*, NS*> simple() {
        auto g = new NS();
        auto sub_g = new NS();
        auto has = new(g) N("has nested"), el = new(g) N("element"), n = new(g) N(sub_g);
        auto p = new(g) N("p"), a = new(g) N("a"), b = new(sub_g) N("b");

        el->connect(has, {n});
        a->connect(p, {b});

        return {g, sub_g};
    }

    std::tuple<NS*> two_nested() {
        auto g0 = new NS(), g1a = new NS(), g1b = new NS();
        auto a = new(g0) N(g1a), b = new(g0) N(g1b);
        auto am = new(g1a) N(0), bm = new(g1b) N(1);

        auto s = new(g0) N("sib");
        auto pa = new(g1a) N("pa");
        auto pb = new(g1b) N("pb");

        a->connect(s, {b}); b->connect(s, {a});
        am->connect(pa, {bm}); bm->connect(pb, {am});


        return {g0,};
    }


    std::tuple<NS*> double_nested() {
        auto g0 = new NS(), g1a = new NS(), g1b = new NS(), g2a = new NS(), g2b = new NS();
        auto a = new(g0) N(g1a), b = new(g0) N(g1b);
        auto x = new(g1a) N(g2a), y = new(g1b) N(g2b);
        auto xm = new(g2a) N(0), ym = new(g2b) N(1);

        auto s = new(g0) N("sib"), c = new(g0) N("child");

        a->connect(s, {b}); b->connect(s, {a});
        x->connect(s, {y}); y->connect(s, {x});

        a->connect(c, {x}); b->connect(c, {y});


        return {g0,};
    }

    std::tuple<NS*> exelixis_python_solution() {
        auto problem_context_g = new NS();
        auto [digitc, operatorc, parenthesec, symbolc] =
         problem_context_g->wrap("Digit", "Operator", "Parenthese", "Symbol");
        auto [hasf, hass, hasi] =
         problem_context_g->wrap("has function", "has symbol", "has instance");
        auto [splus, smin, stimes, sdiv, sneg, s0, s1, s2, s3, s4, s5, s6, s7, s8, s9, slparen, srparen, sempty] =
         problem_context_g->wrap("+", "-", "*", "/", "-", "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "(", ")", " ");
        auto [fint, fadd, fsub, fmul, fdiv] =
         problem_context_g->wrap("int", "add", "sub", "mul", "div");

        //digitc->connect(hasi, {s0, s1, s2, s3, s4, s5, s6, s7, s8, s9});
        operatorc->connect(hasi, {splus, smin, stimes, sdiv});
        parenthesec->connect(hasi, {slparen, srparen});
        //symbolc->connect(hasi, {splus, smin, stimes, sdiv, sneg, s0, s1, s2, s3, s4, s5, s6, s7, s8, s9, slparen, srparen, sempty});
        splus->connect(hasf, {fadd}); smin->connect(hasf, {fsub}); stimes->connect(hasf, {fmul}); sdiv->connect(hasf, {fdiv});

        auto solution_context_g = new NS(), parser_ops_g = new NS(), evaluator_ops_g = new NS();
        auto [parser_ops_n, evaluator_ops_n] =
         solution_context_g->wrap(parser_ops_g, evaluator_ops_g);

        auto [firsta, seconda, thirda, anyofa, arga] =
         solution_context_g->wrap("first", "second", "third", "any of", "arg");
        auto [seqo, alto, mulo, somo] =
         parser_ops_g->wrap("Sequence", "Either", "ZeroOrMore", "OneOrMore");
        auto [chaino, heado, joino, filtero, matcheso, mapo, fixo, applyo, swapo] =
         evaluator_ops_g->wrap("Chain", "Head", "Join", "Filter", "Matches", "Map", "Fix", "Apply", "Swap");

        auto parser_g = new NS();
        auto [ps0, ps1, ps2, ps3, ps4, parser] =
         parser_g->wrap("Integer", "Digit", "Factor", "Term", "Expression", "parser");
        auto [ps00, ps01, ps02, ps10, ps20, ps21, ps30, ps31, ps32, ps33, ps40, ps41, ps42, ps43] =
         parser_g->wrap("seq", "alt", "som", "alt", "alt", "seq", "seq", "mul", "seq", "alt", "seq", "mul", "seq", "alt");

        seqo->connect(hasi, {ps00, ps21, ps30, ps32, ps40, ps42});
        alto->connect(hasi, {ps01, ps10, ps20, ps33, ps43});
        somo->connect(hasi, {ps02});
        mulo->connect(hasi, {ps31, ps41});

        ps0->connect(parser, {ps00});
        ps00->connect(firsta, {ps01});
        ps01->connect(anyofa, {sneg, sempty});
        ps00->connect(seconda, {ps02});
        ps02->connect(arga, {ps1});

        ps1->connect(parser, {ps10});
        ps10->connect(anyofa, {s0, s1, s2, s3, s4, s5, s6, s7, s8, s9});

        ps2->connect(parser, {ps20});
        ps20->connect(anyofa, {ps21, ps0});
        ps21->connect(firsta, {slparen});
        ps21->connect(seconda, {ps4});
        ps21->connect(thirda, {srparen});

        ps3->connect(parser, {ps30});
        ps30->connect(firsta, {ps2});
        ps30->connect(seconda, {ps31});
        ps31->connect(arga, {ps32});
        ps32->connect(firsta, {ps33});
        ps32->connect(seconda, {ps2});
        ps33->connect(anyofa, {stimes, sdiv});

        ps4->connect(parser, {ps40});
        ps40->connect(firsta, {ps3});
        ps40->connect(seconda, {ps41});
        ps41->connect(arga, {ps42});
        ps42->connect(firsta, {ps43});
        ps42->connect(seconda, {ps3});
        ps43->connect(anyofa, {splus, smin});

        auto solver_g = new NS();
        auto [solver] =
         solver_g->wrap("solver");
        auto [fs00, fs01, fs10, fs20, fs21, fs22, fs23,
              fs30, fs31, fs32, fs33, fs34, fs35, fs36, fs37, fs38, fs39,
              fs40, fs41, fs42, fs43, fs44, fs45, fs46, fs47, fs48, fs49] =
         solver_g->wrap("chain", "join", "head", "chain", "filter", "filter", "head",
                 "chain", "map", "chain", "matches", "matches", "fix", "chain", "swap", "apply", "head",
                 "chain", "map", "chain", "matches", "matches", "fix", "chain", "swap", "apply", "head");

        chaino->connect(hasi, {fs00, fs20, fs30, fs40, fs32, fs42, fs36, fs46});
        heado->connect(hasi, {fs10, fs23, fs39, fs49});
        joino->connect(hasi, {fs01,});
        filtero->connect(hasi, {fs21, fs22,});
        matcheso->connect(hasi, {fs33, fs43, fs34, fs44});
        mapo->connect(hasi, {fs31, fs41,});
        fixo->connect(hasi, {fs35, fs45});
        applyo->connect(hasi, {fs38, fs48});
        swapo->connect(hasi, {fs37, fs47});

        ps0->connect(solver, {fs00});
        fs00->connect(firsta, {fs01});
        fs00->connect(seconda, {fint});
        fs01->connect(arga, {sempty});

        ps1->connect(solver, {fs10});

        ps2->connect(solver, {fs20});
        fs20->connect(firsta, {fs21});
        fs20->connect(seconda, {fs22});
        fs20->connect(thirda, {fs23});
        fs21->connect(arga, {slparen});
        fs22->connect(arga, {srparen});

        ps3->connect(solver, {fs30});
        fs30->connect(firsta, {fs31});
        fs30->connect(seconda, {fs35});
        fs30->connect(thirda, {fs39});
        fs31->connect(arga, {fs32});
        fs32->connect(firsta, {fs33});
        fs32->connect(seconda, {fs34});
        fs33->connect(firsta, {stimes});
        fs33->connect(seconda, {fmul});
        fs34->connect(firsta, {sdiv});
        fs34->connect(seconda, {fdiv});
        fs35->connect(arga, {fs36});
        fs36->connect(firsta, {fs37});
        fs36->connect(seconda, {fs38});

        ps4->connect(solver, {fs40});
        fs40->connect(firsta, {fs41});
        fs40->connect(seconda, {fs45});
        fs40->connect(thirda, {fs49});
        fs41->connect(arga, {fs42});
        fs42->connect(firsta, {fs43});
        fs42->connect(seconda, {fs44});
        fs43->connect(firsta, {splus});
        fs43->connect(seconda, {fadd});
        fs44->connect(firsta, {smin});
        fs44->connect(seconda, {fsub});
        fs45->connect(arga, {fs46});
        fs46->connect(firsta, {fs47});
        fs46->connect(seconda, {fs48});

        auto universe = new NS();
        /*
        auto [arithmetic, p1, p2, p3, p4] =
         universe->wrap("Arithmetic", "1+1", "2*(10-7)", "(1+2)-(3*4)", "-100/25 + 2*2*2");
        auto [hasc, hasp] =
         universe->wrap("has context", "has problem");*/
        auto [problem_context_n, solution_context_n, parser_n, solver_n] =
         universe->wrap(problem_context_g, solution_context_g, parser_g, solver_g);
        /*
        arithmetic->connect(hasc, {problem_context_n, solution_context_n});
        arithmetic->connect(hasp, {p1, p2, p3, p4});
        arithmetic->connect(parser, {parser_n});
        arithmetic->connect(solver, {solver_n});*/

        return universe;
    }
}
