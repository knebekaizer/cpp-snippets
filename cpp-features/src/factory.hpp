#pragma once

#include "trace.hpp"
#include <map>
//#include <array>

class Base {
public:
    virtual void foo() const { log_trace << "Generic"; }
    virtual ~Base() = default;
};

class Factory {
public:
    using Key = unsigned; // enum class Key
    using createMethodType = Base* (*)();

    Factory() = default;
    explicit Factory(createMethodType genericImpl) : defaultCreateMethod(genericImpl) {}

    static Factory& instance();

    static void register_(Key key, createMethodType creator) {
        instance().factoryMap[key] = creator;
    }
    template <Key key>
    static Base* create() { return instance().create_<key>(); }

    template <Key key>
    Base* create_() {
        TraceX(key);
        if (auto it = factoryMap.find(key); it != factoryMap.end())
            return it->second();
        return defaultCreateMethod();
    }

    template <class Sample>
    class Registrar {
    public:
        static Base* create() { return new Sample; }
        Registrar() : Registrar(Sample::kind) {}
        explicit Registrar(Factory::Key kind) {
            TraceX(kind); Factory::register_(kind, create);
        }
    };

private:
//    using Map = std::array<createMethodType, 64>;
    using Map = std::map<Key, createMethodType>;
    Map factoryMap;
    const createMethodType defaultCreateMethod = {};
};

template<Factory::Key> class Sample;