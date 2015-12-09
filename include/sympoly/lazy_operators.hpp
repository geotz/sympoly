#ifndef _SYMPOLY_LAZY_OPERATORS_H_

#ifdef _SYMPOLY_OPERATORS_H_
#error Normal operators (already included) cannot be mixed with requested lazy operators.
#endif

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

template<class T, class FT>
FT eval(const T& expr, FT) { return expr; }

// **************************
//      ADDITION
// **************************

template<class A,class B>
struct Add_: public Bin_<A,B>
{
    Add_(const A& a, const B& b): Bin_<A,B>{a,b} { }
    template<class FT>
    FT operator()(FT x) const { return eval(*this, x); }
};

//template<class A,class B>
//struct Add_: public Add_b<A,B>
//{
//    using Add_b<A,B>::Add_b;
//};

template<class A,class B,class FT>
FT eval(const Add_<A,B>& expr, FT x) { return eval(expr.a, x) + eval(expr.b, x); }

template<class A, class B>
auto expand(const Add_<A,B>& expr) { return add_op(expand(expr.a), expand(expr.b)); }

template<class A, class B>
auto expr_add(const A& a, const B& b) { return Add_<A,B>{a, b}; }

// **************************
//      MULTIPLICATION
// **************************

template<class A,class B>
struct Mult_: public Bin_<A,B>
{
    Mult_(const A& a, const B& b): Bin_<A,B>{a,b} { }
    template<class FT>
    FT operator()(FT x) const { return eval(*this, x); }
};

//template<class A,class B>
//struct Mult_: public Mult_b<A,B>
//{
//    using Mult_b<A,B>::Mult_b;
////    auto expand() const { return mul_op(this->a.expand(), this->b.expand()); }
//};

template<class A,class B, class FT>
FT eval(const Mult_<A,B>& expr, FT x) { return eval(expr.a, x) * eval(expr.b, x); }

template<class A, class B>
auto expand(const Mult_<A,B>& expr) { return mul_op(expand(expr.a), expand(expr.b)); }

template<class A, class B>
auto expr_mul(const A& a, const B& b) { return Mult_<A,B>{a, b}; }

template<class OS,class A,class B>
OS& operator<<(OS& os, const sym::Mult_<A,B>& op)
{
    os << "(" << op.a << ") * (" << op.b << ")";
    return os;
}

// **************************
//      OPERATORS
// **************************

template<class OS,class A,class B>
OS& operator<<(OS& os, const sym::Add_<A,B>& op)
{
    os << "(" << op.a << ") + (" << op.b << ")";
    return os;
}

// WARNING: constructs expressions from everything -- maybe restrict to Monomials/Polynomials
// however, ADL will restrict to sym:: namespace

template<class A, class B>
auto operator+(const A& a, const B& b) { return expr_add(a,b); }

template<class A,class B>
auto operator*(const A& a, const B& b) { return expr_mul(a,b); }

}

#endif

