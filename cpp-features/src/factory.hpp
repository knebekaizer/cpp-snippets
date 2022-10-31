#pragma once

#include "trace.hpp"
#include <map>
//#include <array>

class Base {
public:
    Base() = default;
    Base(int, std::string) {}
    virtual void foo() const { log_trace << "Generic"; }
    virtual ~Base() = default;
};

template<typename Ret, typename ...Args>
class FactoryTmpl;

template<typename Ret, typename ...Args>
class FactoryTmpl<Ret(Args...)> {
public:
    using Key = unsigned; // enum class Key
    using createMethodType = Ret (*)(Args&& ...);

    FactoryTmpl() = default;
    FactoryTmpl(const FactoryTmpl&) = delete;
    FactoryTmpl(FactoryTmpl&&) = delete;
    explicit FactoryTmpl(createMethodType genericImpl) : defaultCreateMethod_(genericImpl) {}

    static FactoryTmpl& instance();

    template <Key key>
    static Ret create(Args&&... args) {
        return instance().template create_<key>(std::forward<Args>(args)...);
    }

    template <class Sample>
    class Registrar {
    public:
        static Ret create(Args&&... args) { return new Sample(std::forward<Args>(args)...); }
        Registrar() : Registrar(Sample::kind) {}
        explicit Registrar(FactoryTmpl::Key kind) {
            TraceX(kind); FactoryTmpl::register_(kind, create);
        }
    };

	static void register_(Key key, createMethodType creator) {
		instance().factoryMap_[key] = creator;
	}

private:
	template <Key key>
    Ret create_(Args&&... args) {
		TraceX(key);
		if (auto it = factoryMap_.find(key); it != factoryMap_.end())
			return it->second(std::forward<Args>(args)...);
		return defaultCreateMethod_(std::forward<Args>(args)...);
	}

        //    using Map = std::array<createMethodType, 64>;
    using Map = std::map<Key, createMethodType>;
    Map factoryMap_;
    const createMethodType defaultCreateMethod_ = {};
};

//using Factory = FactoryTmpl<Base*, int, std::string>;
using Factory = FactoryTmpl<Base* (int, std::string)>;
template<Factory::Key> class Sample;
