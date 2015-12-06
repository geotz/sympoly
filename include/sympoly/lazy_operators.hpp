#ifndef _SYMPOLY_LAZY_OPERATORS_H_
#define _SYMPOLY_LAZY_OPERATORS_H_

namespace sym
{

template<class A,class B>
struct Bin_
{
    typedef A TypeA;
    typedef B TypeB;
    A a;
    B b;
};

// **************************
//      ADDITION
// **************************

template<class A,class B>
struct Add_b: public Bin_<A,B>
{
    Add_b(const A& a, const B& b): Bin_<A,B>{a,b} { }
    template<class FT>
    FT operator()(FT x) const { return this->a(x) + this->b(x); }
};

template<class A,class B>
struct Add_: public Add_b<A,B>
{
    using Add_b<A,B>::Add_b;
    auto expand() const { return add_op(this->a.expand(), this->b.expand()); }
};

template<class A, class B>
auto expr_add(const A& a, const B& b) { return Add_<A,B>{a, b}; }

// **************************
//      MULTIPLICATION
// **************************

template<class A,class B>
struct Mult_b: public Bin_<A,B>
{
    Mult_b(const A& a, const B& b): Bin_<A,B>{a,b} { }
    template<class FT>
    FT operator()(FT x) const { return this->a(x) * this->b(x); }
};

template<class A,class B>
struct Mult_: public Mult_b<A,B>
{
    using Mult_b<A,B>::Mult_b;
    auto expand() const { return mul_op(this->a.expand(), this->b.expand()); }
};

template<class A, class B>
auto expr_mul(const A& a, const B& b) { return Mult_<A,B>{a, b}; }

template<class OS,class A,class B>
OS& operator<<(OS& os, const sym::Mult_b<A,B>& op)
{
    using namespace sym;
    os << "(" << op.a << ") * (" << op.b << ")";
    return os;
}

// **************************
//      OPERATORS
// **************************

template<class OS,class A,class B>
OS& operator<<(OS& os, const sym::Add_b<A,B>& op)
{
    using namespace sym;
    os << "(" << op.a << ") + (" << op.b << ")";
    return os;
}

template<class A, class B>
auto operator+(const A& a, const B& b) { return sym::expr_add(a,b); }

template<class A,class B>
auto operator*(const A& a, const B& b) { return sym::expr_mul(a,b); }

}

#endif

