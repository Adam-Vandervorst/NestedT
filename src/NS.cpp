#include "NS.h"

bool NS::add(N* n){
    _os.push_back(n);
    return true;
}

bool NS::add(const vector<N*>& ns){
    _os.insert(_os.end(), ns.begin(), ns.end());
    return true;
}

N* NS::get(CType data) {
    return get((Prep)([data](N* n){return n->data == data;}));
}

N* NS::get(Prep preposition) {
    for (auto n : _os) if (preposition(n)) return n;
    throw std::out_of_range("No matching node found");
}

NS NS::get_all(CType data) {
    return get_all((Prep)[data](N* n){return n->data == data;});
}

NS NS::get_all(Prep preposition) {
    NS res;
    for (auto n : _os) if (preposition(n)) res.add(n);
    return res;
}

NS* NS::clear(){
    for (auto n : _os)
        std::visit([&](auto &&data){
            if constexpr (std::is_same_v<std::decay_t<decltype(data)>, NS*>) delete data->clear();
            delete n;
        }, n->data);
    _os.clear();
    return this;
}

// FIXME does not work with nested graphs yet
NS* NS::clear(Prep preposition){
    for (int i = _os.size(); i >= 0; --i) {
        if (preposition(_os[i])){
            delete _os[i]; _os[i] = _os.back(); _os.pop_back();
            --i;
        }
    }
    return this;
}

void NS::print_all(const char* sep, std::ostream& stream){
    for (auto o : _os) stream << *o << (o == _os.back() ? "\n" : sep);
}

void NS::print_all(Prep preposition, const char* sep, std::ostream& stream){
    for (auto o : _os) if (preposition(o)) stream << *o << (o == _os.back() ? "\n" : sep);
}

bool NS::contains(N* to_find) {
    return std::find(_os.begin(), _os.end(), to_find) != _os.end();
}

nodeit NS::it() {
    return nodeit(_os.begin(), _os.end());
}

NS *NS::from_it(nodeit it) {
    it.foreach([this](N* n){_os.push_back(n);});
    return this;
}
