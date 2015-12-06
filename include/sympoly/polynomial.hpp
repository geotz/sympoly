#ifndef _SYMPOLY_POLYNOMIAL_H_
#define _SYMPOLY_POLYNOMIAL_H_

// ##################
// TODO: add FT !!
// ###################

#include<type_traits>

#include"internal/monomial.hpp"

namespace sym
{

template<int DEG, int... DEGs>
struct Support
{
    typedef Support<DEG,DEGs...> Self;
    typedef Support<DEGs...> Tail;
    static constexpr int head = DEG;
    static constexpr int degree = Tail::degree;
    static constexpr int size = 1 + Tail::size;
    static constexpr bool contains( int k ) { return DEG == k || Tail::contains(k); }

    static_assert( Tail::head > head, "support sequence should be increasing" );
    static_assert( DEG >= 0, "degree should be non-negative" );
};

template<int DEG>
struct Support<DEG>
{
    typedef Support<DEG> Self;
    static constexpr int head = DEG;
    static constexpr int degree = DEG;
    static constexpr int size = 1;
    static constexpr bool contains( int k ) { return DEG == k; }

    static_assert( DEG >= 0, "degree should be non-negative" );
};

template<int, class>
struct Append;

template<int DEG, int... DEGs>
struct Append<DEG,Support<DEGs...>>
{
    typedef Support<DEG,DEGs...> Result;
};

template<int, class>
struct Add;

template<int K, int DEG1>
struct Add<K,Support<DEG1>>
{
    typedef Support<K+DEG1> Result;
};

template<int K, int DEG1, int... DEGs>
struct Add<K,Support<DEG1,DEGs...>>
{
    typedef typename Append<K+DEG1,
                            typename Add<K,Support<DEGs...>>::Result>::Result Result;
};

template<class S1, class S2>
struct Union;

template<int DEG1, int DEG2>
struct Union<Support<DEG1>,Support<DEG2>>
{
    typedef typename std::conditional< DEG1 < DEG2,
                Support<DEG1,DEG2>,
                typename std::conditional< DEG1 == DEG2,
                        Support<DEG2>,
                        Support<DEG2,DEG1>>::type
              >::type Result;
};

template<int DEG1,int... DEG1s, int DEG2, int... DEG2s>
struct Union<Support<DEG1,DEG1s...>,Support<DEG2,DEG2s...>>
{
    typedef typename std::conditional< DEG1 < DEG2,
                typename Append<DEG1,
                       typename Union<Support<DEG1s...>, Support<DEG2,DEG2s...>>::Result
                       >::type,
                 typename std::conditional<DEG1 == DEG2,
                        typename Append<DEG1,typename Union<Support<DEG1s...>,Support<DEG2s...>>::Result>::Result,
                        typename Append<DEG2,
                               typename Union<Support<DEG1,DEG1s...>, Support<DEG2s...>>::Result>::Result
                        >::type
              >::type Result;
};

template<int DEG1,int DEG2, int... DEG2s>
struct Union<Support<DEG1>,Support<DEG2,DEG2s...>>
{
    typedef typename std::conditional< DEG1 < DEG2,
                typename Append<DEG1,
                       Support<DEG2,DEG2s...>
                       >::Result,
                typename std::conditional<DEG1 == DEG2,
                        Support<DEG2,DEG2s...>,
                        typename Append<DEG2,
                               typename Union<Support<DEG1>, Support<DEG2s...>>::Result
                               >::Result
                          >::type
              >::type Result;
};

template<int DEG1,int... DEG1s, int DEG2>
struct Union<Support<DEG1,DEG1s...>,Support<DEG2>>
{
    typedef typename Union<Support<DEG2>,Support<DEG1,DEG1s...>>::Result Result;
};

template<class Monomial,class Polynomial>
class PolynomRep
{
public:
    Monomial front;
    Polynomial tail;
    typedef typename Monomial::field_type FT;

    PolynomRep( const Monomial& m, const Polynomial& p): front(m.coeff), tail(p) { }
    static constexpr int content = Monomial::degree;
    static constexpr int degree = Polynomial::degree;
    bool is_zero() const { return front.is_zero() && tail.is_zero(); }
    template<class OS>
    OS& dump(OS& os) const {
        os << front << " + ";
        tail.dump(os);
        return os;
    }
    FT operator()(FT x, int content=0) const {
        if (x == FT(0) ) return 0; // short-circuit ?
        return front.power(x,content) * ( front.coeff + tail(x,content + this->content) );
    }
};

template<class RT, class Supp, class FT = double>
class Polynom: public PolynomRep<Monom<RT,Supp::head,FT>, Polynom<RT,typename Supp::Tail,FT>>
{
public:
    using Tail = typename Supp::Tail;
    typedef Monom<RT,Supp::head,FT> Monomial;
    typedef Polynom<RT,Tail,FT> Polynomial;
    typedef PolynomRep<Monomial, Polynomial> Base;

    Polynom( const Monomial& m, const Polynomial& p): Base(m, p) { }
    const Polynom& expand() const { return *this; }
};

template<class RT, int DEG1, int DEG2, class FT>
class Polynom<RT,Support<DEG1,DEG2>,FT>: public PolynomRep<Monom<RT,DEG1,FT>, Monom<RT,DEG2,FT>>
{
public:
    static_assert( DEG1<DEG2, "degree sequence should be increasing" );
    typedef Monom<RT,DEG1,FT> Monomial1;
    typedef Monom<RT,DEG2,FT> Monomial2;
    typedef Monomial1 Monomial;
    typedef Monomial2 Polynomial;
    typedef PolynomRep<Monomial1, Monomial2> Base;

    Polynom( const Monomial1& m, const Monomial2& p): Base(m, p) { }
    const Polynom& expand() const { return *this; }
};

template<class RT,class Supp>
struct Anynom
{
    typedef typename std::conditional<Supp::size == 1,
        Monom<RT,Supp::head>,
        Polynom<RT,Supp>>::type Type;
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

template<class RT,int DEG1, int DEG2>
auto add_op(const Monom<RT,DEG1>& a, const Monom<RT,DEG2>& b)
{
    typedef typename Union<Support<DEG1>,Support<DEG2>>::Result Supp;
    typedef Polynom<RT,Supp> Result;
    return MonoMono<Result,(DEG1<DEG2)>()(a, b);
}

template<class RT, int DEG1, int... DEGs>
auto mul_op(const Monom<RT,DEG1>& a, const Polynom<RT,Support<DEGs...>>& b)
{
    typedef typename Add<DEG1,Support<DEGs...>>::Result Supp;
    typedef Monom<RT,Supp::head> Monomial;
    typedef typename Anynom<RT,typename Supp::Tail>::Type Polynomial;
    typedef Polynom<RT,Supp> ResultPolynomial;

    return ResultPolynomial(add_op(mul_op(a, b.front), mul_op(a, b.tail)));
}

template<class RT, int DEG1, int... DEGs>
auto mul_op(const Polynom<RT,Support<DEGs...>>& a, const Monom<RT,DEG1>& b)
{
    return mul_op(b, a);
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

template<class RT, int DEG1, int... DEGs>
auto add_op(const Monom<RT,DEG1>& a, const Polynom<RT,Support<DEGs...>>& b)
{
    typedef Support<DEGs...> Support2;
    typedef typename Union<Support<DEG1>,Support<DEGs...>>::Result Supp;
    typedef Monom<RT,Supp::head> Monomial;
    typedef typename Anynom<RT,typename Supp::Tail>::Type Polynomial;
    typedef Polynom<RT,Supp> ResultPolynomial;

    constexpr int k = DEG1 < Support2::head ? -1 : DEG1 != Support2::head;
    return MonoPoly<ResultPolynomial,decltype(a),decltype(b),Monomial,Polynomial,k>()(a, b);
}

template<class RT, int DEG1, int... DEGs>
auto add_op(const Polynom<RT,Support<DEGs...>>& a, const Monom<RT,DEG1>& b)
{
    return add_op(b, a);
}

template<class RT, int... DEG1s, int... DEG2s>
auto add_op(const Polynom<RT,Support<DEG1s...>>& a, const Polynom<RT,Support<DEG2s...>>& b)
{
    // chop-off
    return add_op(a.front, b.front, a.tail,b.tail); // avoid messing with operator+
}

template<class RT, int... DEG1s, int... DEG2s>
auto mul_op(const Polynom<RT,Support<DEG1s...>>& a, const Polynom<RT,Support<DEG2s...>>& b)
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

//template<int N,class RT,int... DEGs>
//auto pow_op(const Polynom<RT,Support<DEGs...>>& a)
//{
//}

// **************************
//      OPERATORS
// **************************

template<class OS, class RT, class Supp>
OS& operator<<(OS& os, const sym::Polynom<RT,Supp>& m) { return m.dump(os); }

}

#endif

