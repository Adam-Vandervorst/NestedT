#include "Rewriting.h"


void apply_mapping(NS* base, std::map<N*, N*>& base_to_template) {
    std::map<N*, N*> template_to_base;
    for (auto&& p : base_to_template)
        template_to_base[p.second] = p.first;


    std::map<N*, N*> nested_to_template;
    for (auto [template_n, base_n] : base_to_template) {
        if (NS** g = std::get_if<NS*>(&template_n->data)) {
            if ((*g)->_os.size() == 1){
                nested_to_template[(*g)->_os[0]] = template_n;
            }
        }
    }


    for (auto [template_n, base_n] : base_to_template) {
        if (NS** g = std::get_if<NS*>(&template_n->data)) {
            if ((*g)->_os.size() != 1) throw std::runtime_error("Node creation/deletion in rewriting is not yet implemented");
            N* nested_n = (*g)->_os[0];
            std::vector<std::pair<N*, N*>> template_connections, nested_connections;
            std::set<std::pair<N*, N*>> deleted, added;
            for (N* p : template_n->ps) for (N* o : (*template_n)[p]) template_connections.emplace_back(p, o);
            for (N* p : nested_n->ps) for (N* o : (*nested_n)[p]) nested_connections.emplace_back(p, o);
            std::set_difference(template_connections.begin(), template_connections.end(), nested_connections.begin(), nested_connections.end(), std::inserter(deleted, deleted.begin()));
            std::set_difference(nested_connections.begin(), nested_connections.end(), template_connections.begin(), template_connections.end(), std::inserter(added, added.begin()));
            //for (auto [p, o] : deleted) std::cout << "delete " << *base_n << " |" << *p << "> " << *o << std::endl;
            for (auto [p, o] : deleted) base_n->disconnect(p, {template_to_base[nested_to_template[o]]});
            for (auto [p, o] : added) base_n->connect(p, {template_to_base[nested_to_template[o]]});
        }
    }
}

vector<std::map<N*, N*>> possible_mappings(NS *subgraph, N *m) {
    vector<std::map<N*, N*>> mappings;
    std::map<N*, N*> last_map;
    bool encountered, save;

    // check if there is a possible mapping
    for (auto n : subgraph->it()) if ((*n)[m].size() == 0) return mappings;

    do { // extract mapping
        last_map.clear(); encountered = false;
        for (auto placeholder : subgraph->it()) {
            if (not encountered and (*placeholder)[m].size() > 1){
                encountered = true;
                auto item = *(*placeholder)[m];
                last_map[placeholder] = item;
                placeholder->disconnect(m, {item});
            } else {
                last_map[placeholder] = *(*placeholder)[m];
            }
        }

        // store if mapping is injective
        std::set<N*> trace; save = true;
        for (auto [_, v] : last_map) {
            if (trace.contains(v)) {save = false; break;}
            trace.insert(v);
        }

        if (save) mappings.push_back(last_map);
    } while (encountered);

    return mappings;
}

bool select_possible(N* placeholder, N* node, N* match_edge, bool has_match_edge=true) {
    auto ps = node->ps;
    if (placeholder->ps.size() - has_match_edge > ps.size()) return false;

    vector<N*> pps(placeholder->ps);
    pps.erase(std::remove_if(pps.begin(), pps.end(), [ps, match_edge](N* pp){
        return (pp == match_edge) or (std::find(ps.begin(), ps.end(), pp) != ps.end());
    }), pps.end());

    for (auto pp : pps)
        if (pp->same_source(node))
            return false;

    return true;
}


alg::Alg<void> alg::checks_out(N* match_edge) {
    return [match_edge](N& placeholder, vector<N*> &line) {
        if (find(line.begin(), line.end(), &placeholder) != line.end()) return;
        //std::cout << placeholder << std::endl;
        for (auto node : placeholder[match_edge]){
            if (not select_possible(&placeholder, node, match_edge)) continue;
            for (auto p : node->ps) {
                for (auto next_placeholder : placeholder[p])
                    next_placeholder->connect(match_edge, (*node)[p]);

                for (auto generic_p : placeholder.ps){
                    if (not generic_p->same_source(&placeholder) or (generic_p == match_edge)) continue;
                    for (auto next_placeholder : placeholder[generic_p])
                        next_placeholder->connect(match_edge, (*node)[p]);
                }
            }
        }
    };
}

std::tuple<NS*, NS*, NS*> test_graphs::nat_has(bool replacement) {
    auto g = new NS();
    auto n1 = new(g) N("Box"), n2 = new(g) N("Pyramid"), n3 = new(g) N("Ball");
    auto has = new(g) N("Left"), is = new(g) N("Right");
    auto top = new(g) N("Top"), bot = new(g) N("Bottom");
    auto opp = new(g) N("opposite");

    n1->connect(has, {n2});
    n1->connect(top, {n3});
    has->connect(opp, {is});
    is->connect(opp, {has});
    top->connect(opp, {bot});
    bot->connect(opp, {top});

    auto to_match = new NS();
    auto x = new(to_match) N("Object 1"), y = new(to_match) N("Object 2");
    auto p = new(to_match) N("generic property"), np = new(to_match) N("generic anti property");
    x->connect(p, {y});
    p->connect(opp, {np});

    auto changed = new NS();
    auto add_e = new(changed) N("new");
    auto m = new(changed) N("match");

    if (replacement) {
        y->connect(np, {x});
        //y->connect(add_e, {x});
    }

    return {g, to_match, changed};
}

std::tuple<NS*, NS*> test_graphs::nat_has_nested() {
    auto g = new NS();
    auto n1 = new(g) N("Nat"), n2 = new(g) N(0), n3 = new(g) N(1);
    auto has = new(g) N("has"), is = new(g) N("is");
    auto opp = new(g) N("opposite");

    n1->connect(has, {n2});
    n1->connect(has, {n3});
    has->connect(opp, {is});
    is->connect(opp, {has});

    auto to_match = new NS(), x_g = new NS(), y_g = new NS();
    auto x = new(to_match) N(x_g), y = new(to_match) N(y_g);
    auto p = new(to_match) N("p"), np = new(to_match) N("np");
    x->connect(p, {y});
    p->connect(opp, {np});

    auto x_ = new(x_g) N("x"), y_ = new(y_g) N("y");
    y_->connect(np, {x_});

    return {g, to_match};
}

std::tuple<NS*, NS*, NS*, NS*> test_graphs::nested_rewrite_example() {
    auto context = new NS();
    auto neighbour = new(context) N("");
    auto one_neighbour = new NS(), two_neighbours = new NS(), four_neighbours = new NS();
    auto f1 = new NS(), t1 = new NS(), g1 = new NS(), g2 = new NS(), t3 = new NS(), h1 = new NS(), h2 = new NS(), h3 = new NS(), h4 = new NS(), t5 = new NS();
    auto [f1n, t1n] = one_neighbour->wrap(f1, t1); auto [g1n, g2n, t3n] = two_neighbours->wrap(g1, g2, t3); auto [h1n, h2n, h3n, h4n, t5n] = four_neighbours->wrap(h1, h2, h3, h4, t5);

    t1n->connect(neighbour, {f1n});
    auto t1s = new(f1) N("Lonely citizen");

    t3n->connect(neighbour, {g1n, g2n});
    auto mother = new(g1) N("Mother");
    auto father = new(g2) N("Father");
    auto child = new(g1) N("Child");
    child->connect(neighbour, {mother, father});

    t5n->connect(neighbour, {h1n, h2n, h3n, h4n});
    auto x = new(h1) N("Citizen");
    auto y = new(h2) N("Citizen");
    auto z = new(h3) N("Citizen");
    auto w = new(h4) N("Citizen");

    return {context, one_neighbour, two_neighbours, four_neighbours};
}

/*
digraph g {
compound=true;
subgraph cluster_1 {
label="rewrite step";
box [label="Box"];
pyr [label="Pyramid"];
bal [label="Ball"];
l [label="Left"];
r [label="Right"];
t [label="Top"];
b [label="Bottom"];
o [label="opposite"];
box -> pyr [label="Left"];
box -> bal [label="Top"];
l -> r [label="opposite"];
r -> l [label="opposite"];
t -> b [label="opposite"];
b -> t [label="opposite"];

obj1 [label="Object 1"];
obj2 [label="Object 2"];
gp [label="generic property"];
gap [label="generic anti property"];
obj1 -> obj2 [label="generic property"];
gp -> gap [label="opposite"];
edge [color=red];
gap -> gp [label="generic anti property"];
edge [color=blue];
obj1 -> box [label="match"];
obj2 -> pyr [label="match"];
gp -> l [label="match"];
gap -> r [label="match"];
obj2 -> bal [label="match"];
gp -> t [label="match"];
gap -> b [label="match"];
 }

subgraph cluster_2 {
label="resulting graph";
94254909779664 [label="Box"];
94254909779824 [label="Pyramid"];
94254909779984 [label="Ball"];
94254909780160 [label="Left"];
94254909780288 [label="Right"];
94254909780496 [label="Top"];
94254909780624 [label="Bottom"];
94254909780752 [label="opposite"];
94254909779664 -> 94254909779824 [label="Left"];
94254909779664 -> 94254909779984 [label="Top"];
94254909780160 -> 94254909780288 [label="opposite"];
94254909780288 -> 94254909780160 [label="opposite"];
94254909780496 -> 94254909780624 [label="opposite"];
94254909780624 -> 94254909780496 [label="opposite"];
edge [color=red];
94254909779984 -> 94254909779664 [label="Bot"];
94254909779824 -> 94254909779664 [label="Right"];
}
}

 */