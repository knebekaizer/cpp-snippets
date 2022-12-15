#include <cassert>
#include <iostream>
#include <algorithm>
#include <memory>
#include <map>
#include <vector>

#include "trace.hpp"

using namespace std;

struct S {
    S() = delete;
    S(const S& s) : id(s.id) { log_trace << "S copy ctor: " << id; }
    explicit S(int i) : id(i) { log_trace << "S ctor(int): " << id; }
    ~S() { log_trace << "S dtor: " << id; }
    int id;
};
ostream& operator<<(ostream& os, const S& s) { return os << s.id; }

template <typename K, typename V>
ostream& operator<<(ostream& os, const map<K,V>& m) {
    for (auto& p : m) {
        os << '(' << p.first << " : " << p.second << "), ";
    }
    return os;
}

void test_erase() {
    map<int, S> m;
    for (int k=0; k<5; ++k)
        m.emplace(pair(k, S(k)));
//        m.insert(pair(k, S(k)));
    log_trace "Init done.";
    if (auto it = m.find(3); it != m.end())
        m.erase(it);

    TraceX(m);
}

auto test_inserter() {
    struct Size {
        int32_t width;
        int32_t height;
    };
    struct Settings {
        std::string mixerId;
        Size size;
    };
    using LayoutSettings = std::vector<Settings>;

    map<string,  unique_ptr<Size>> m_videoMixers;
    LayoutSettings layoutSettings;
    for (const auto& [id, mixer] : m_videoMixers) {
        if (mixer)
            layoutSettings.emplace_back(Settings{.mixerId = id, .size = {mixer->width, mixer->height}});
    }
    return layoutSettings;
}

}

int main() {
    test_erase();
}