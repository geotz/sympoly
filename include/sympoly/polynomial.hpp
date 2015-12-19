#ifndef _SYMPOLY_POLYNOMIAL_H_
#define _SYMPOLY_POLYNOMIAL_H_

#include"internal/monomial.hpp"
#include"functional_support.h"

namespace sym
{

template<class Monomial,class Polynomial>
class PolynomRep
{
public:
    Monomial front;
    Polynomial tail;
    typedef typename Monomial::field_type FT;

    PolynomRep( const Monomial& m, const Polynomial& p): front(m.coeff), tail(p) { }
//    static constexpr int content = Monomial::degree;
    static constexpr int degree = Polynomial::degree;
    static constexpr char symbol = Monomial::symbol;
    bool is_zero() const { return front.is_zero() && tail.is_zero(); }
    template<class OS>
    OS& dump(OS& os) const {
        os << front << " + ";
        tail.dump(os);
        return os;
    }
};

template<class RT, class Supp, class FT=double, char SYM='x'>
class Polynom: public PolynomRep<Monom<RT,Supp::head,FT,SYM>, Polynom<RT,typename Supp::Tail,FT,SYM>>
{
public:
    using Tail = typename Supp::Tail;
    typedef Monom<RT,Supp::head,FT,SYM> Monomial;
    typedef Polynom<RT,Tail,FT,SYM> Polynomial;
    typedef PolynomRep<Monomial, Polynomial> Base;

    Polynom( const Monomial& m, const Polynomial& p): Base(m, p) { }
    const Polynom& expand() const { return *this; }
    FT operator()(FT x, int content=0) const { return eval(*this, x, content); }
};

template<class RT, int DEG1, int DEG2, class FT, char SYM>
class Polynom<RT,Support<DEG1,DEG2>,FT,SYM>: public PolynomRep<Monom<RT,DEG1,FT,SYM>, Monom<RT,DEG2,FT,SYM>>
{
public:
    static_assert( DEG1<DEG2, "degree sequence should be increasing" );
    typedef Monom<RT,DEG1,FT,SYM> Monomial1;
    typedef Monom<RT,DEG2,FT,SYM> Monomial2;
    typedef Monomial1 Monomial;
    typedef Monomial2 Polynomial;
    typedef PolynomRep<Monomial1, Monomial2> Base;

    Polynom( const Monomial1& m, const Monomial2& p): Base(m, p) { }
    const Polynom& expand() const { return *this; }
    FT operator()(FT x, int content=0) const { return eval(*this, x, content); }
};

template<class RT, class Supp, class FT, char SYM>
FT eval(const Polynom<RT,Supp,FT,SYM>& a, FT x, int content=0)
{
    if (x == FT(0) ) return 0; // short-circuit ?
    return a.front.power(x, content) * ( a.front.coeff + a.tail(x, a.front.degree) );
}

template<class RT,class Supp,class FT,char SYM>
struct Anynom
{
    typedef typename std::conditional<Supp::size == 1,
        Monom<RT,Supp::head,FT,SYM>,
        Polynom<RT,Supp,FT,SYM>>::type Type;
};

template<class R,bool>
struct MonoMono;

template<class R>
struct MonoMono<R,true>
{
    template<class A1, class A2>
    R operator()(const A1& a1, const A2& a2) { return R(a1,a2); }
};

template<class R>
struct MonoMono<R,false>
{
    template<class A1, class A2>
    R operator()(const A1& a1, const A2& a2) { return R(a2,a1); }
};

template<class RT,int DEG1, int DEG2,class FT,char SYM>
auto add_op(const Monom<RT,DEG1,FT,SYM>& a, const Monom<RT,DEG2,FT,SYM>& b)
{
    typedef typename Union<Support<DEG1>,Support<DEG2>>::Result Supp;
    typedef Polynom<RT,Supp,FT,SYM> Result;
    return MonoMono<Result,(DEG1<DEG2)>()(a, b);
}

template<class RT, int DEG1, int... DEGs,class FT,char SYM>
auto mul_op(const Monom<RT,DEG1,FT,SYM>& a, const Polynom<RT,Support<DEGs...>,FT,SYM>& b)
{
    typedef typename Translate<DEG1,Support<DEGs...>>::Result Supp;
    typedef Monom<RT,Supp::head,FT,SYM> Monomial;
    typedef typename Anynom<RT,typename Supp::Tail,FT,SYM>::Type Polynomial;
    typedef Polynom<RT,Supp,FT,SYM> ResultPolynomial;

    return ResultPolynomial(add_op(mul_op(a, b.front), mul_op(a, b.tail)));
}

template<class RT, int DEG1, int... DEGs,class FT, char SYM>
auto mul_op(const Polynom<RT,Support<DEGs...>,FT,SYM>& a, const Monom<RT,DEG1,FT,SYM>& b)
{
    return mul_op(b, a);
}

// maybe optimize instead of passing through monomial...
template<class RT, int... DEGs, class FT, char SYM>
auto mul_op(const RT& a, const Polynom<RT,Support<DEGs...>,FT,SYM>& b)
{
    return mul_op(Monom<RT,0,FT,SYM>(a), b);
}

template<class RT, int... DEGs, class FT, char SYM>
auto mul_op(const Polynom<RT,Support<DEGs...>,FT,SYM>& a, const RT& b)
{
    return mul_op(a, Monom<RT,0,FT,SYM>(b));
}

template<class R,class M1,class P1,class M2,class P2,int>
struct MonoPoly;

template<class R,class M1,class P1,class M2,class P2>
struct MonoPoly<R,M1,P1,M2,P2,-1>
{
    R operator()(const M1& a, const P1& b) {
        return R(a.coeff, b);
    }
};

template<class R,class M1,class P1,class M2,class P2>
struct MonoPoly<R,M1,P1,M2,P2,0>
{
    R operator()(const M1& a, const P1& b) {
        return R(M2( a.coeff + b.front.coeff ), b.tail);
    }
};

template<class R,class M1,class P1,class M2,class P2>
struct MonoPoly<R,M1,P1,M2,P2,1>
{
    R operator()(const M1& a, const P1& b) {
        return R(M2{b.front.coeff}, add_op(a, b.tail));
    }
};

template<class RT, int DEG1, int... DEGs, class FT, char SYM>
auto add_op(const Monom<RT,DEG1,FT,SYM>& a, const Polynom<RT,Support<DEGs...>,FT,SYM>& b)
{
    typedef Support<DEGs...> Support2;
    typedef typename Union<Support<DEG1>,Support<DEGs...>>::Result Supp;
    typedef Monom<RT,Supp::head,FT,SYM> Monomial;
    typedef typename Anynom<RT,typename Supp::Tail,FT,SYM>::Type Polynomial;
    typedef Polynom<RT,Supp,FT,SYM> ResultPolynomial;

    constexpr int k = DEG1 < Support2::head ? -1 : DEG1 != Support2::head;
    return MonoPoly<ResultPolynomial,decltype(a),decltype(b),Monomial,Polynomial,k>()(a, b);
}

template<class RT, int DEG1, int... DEGs, class FT, char SYM>
auto add_op(const Polynom<RT,Support<DEGs...>,FT,SYM>& a, const Monom<RT,DEG1,FT,SYM>& b)
{
    return add_op(b, a);
}

template<class RT, int... DEG1s, int... DEG2s, class FT, char SYM>
auto add_op(const Polynom<RT,Support<DEG1s...>,FT,SYM>& a, const Polynom<RT,Support<DEG2s...>,FT,SYM>& b)
{
    // chop-off
    return add_op(a.front, b.front, a.tail,b.tail); // avoid messing with operator+
}

template<class RT, int... DEG1s, int... DEG2s, class FT, char SYM>
auto mul_op(const Polynom<RT,Support<DEG1s...>,FT,SYM>& a, const Polynom<RT,Support<DEG2s...>,FT,SYM>& b)
{
    // gauss/karatsuba, fft ?
    return add_op(mul_op(a.front,b.front), mul_op(a.front,b.tail), mul_op(b.front,a.tail), mul_op(a.tail,b.tail));
}

template<class A1, class A2, class A3, class... As>
auto add_op(const A1& a1, const A2& a2, const A3& a3, const As&... as)
{
    return add_op(add_op(a1, a2), a3, as...);
}

template<class A1, class A2, class A3, class... As>
auto mul_op(const A1& a1, const A2& a2, const A3& a3, const As&... as)
{
    return mul_op(mul_op(a1, a2), a3, as...);
}

//template<int N,class RT,int... DEGs,class FT,char SYM>
//auto pow_op(const Polynom<RT,Support<DEGs...>,FT,SYM>& a)
//{

//}

// **************************
//      OPERATORS
// **************************

template<class OS, class RT, class Supp, class FT, char SYM>
OS& operator<<(OS& os, const sym::Polynom<RT,Supp,FT,SYM>& m) { return m.dump(os); }

}

#endif

