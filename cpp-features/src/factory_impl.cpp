#include "factory.hpp"


template<> class Sample<42> : public Base {
public:
    const static auto kind = 42;
    Sample(int&&, std::string&&) {}
    void foo() const override { log_trace << __PRETTY_FUNCTION__; }
    static Factory::Registrar<Sample<42>> selfRegister_;
};
Factory::Registrar<Sample<42>> Sample<42>::selfRegister_;

// OK too
//template <class T> Factory::Registrar<T> selfRegister;
//static auto regMe = selfRegister<Sample<42>>;


class Derived : public Base {
public:
    Derived(int&&, std::string&&) {}
    void foo() const override { log_trace << __PRETTY_FUNCTION__ ; }
    static Factory::Registrar<Derived> selfRegister_;
};
Factory::Registrar<Derived> Derived::selfRegister_(12);

//AutoRegister<Derived> Derived::selfRegister_;