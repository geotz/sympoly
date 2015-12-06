#ifndef _SYMPOLY_MONOMIAL_HPP_
#define _SYMPOLY_MONOMIAL_HPP_

#include<iostream>
#include<cmath>

namespace sym
{

//template<int B,int E>
//struct Power
//{
//    static constexpr int result =
//            E ? ( (E & 1) ? B*Power<B,((E-1)>>1)>::result * Power<B,((E-1)>>1)>::result:
//                            Power<B,(E>>1)>::result * Power<B,(E>>1)>::result ) : 1;
//};

template<int E>
struct _
{
    static constexpr int value = E;
};

// StackOverflow
template<class T>
constexpr T cpow(const T base, const int exponent)
{
    return (exponent == 0)     ? 1 :
           (exponent & 1) == 0 ? cpow(base, exponent>>1)*cpow(base, exponent>>1) :
           base * cpow(base, (exponent-1)>>1) * cpow(base, (exponent-1)>>1);
}

template<class RT, int DEG = 0, class FT = double>
class Monom
{
public:
    typedef RT ring_type;
    typedef FT field_type;
    RT coeff;

    Monom( RT c = RT(1) ): coeff(c) {}

    static constexpr int degree = DEG;
    bool is_zero() const { return coeff == RT(0); }
    FT operator()(FT x, int content=0) const { return is_zero() ? 0 : coeff * power(x,content); }
    FT power(FT x, int content=0) const {
        std::cerr << "pow x = " << x << " DEG = " << degree << " content = " << content << std::endl;
        return std::pow(x, DEG-content);
    }
    const Monom& expand() const { return *this; }
    template<class OS>
    OS& dump(OS& os) const {
        if (degree == 0) return os << coeff;
        if (is_zero()) return os << "0";
        if (coeff == RT(-1)) {
            os << "-";
        } else if (coeff != RT(1) ) {
            os << coeff;
        }
        os << "x";
        if (degree > 1) os << "^" << degree;
        return os;
    }
};

// TODO: assert unsigned?
template<int N,class RT,int DEG>
auto pow_op(const Monom<RT,DEG>& a)
{
    return Monom<RT,DEG*N>(cpow(a.coeff,N));
}

template<class RT, int DEG>
Monom<RT,DEG> add_op(const Monom<RT,DEG>& a, const Monom<RT,DEG>& b)
{
    return Monom<RT,DEG>(a.coeff + b.coeff);
}

template<class RT, int DEG1, int DEG2>
Monom<RT,DEG1+DEG2> mul_op(const Monom<RT,DEG1>& a, const Monom<RT,DEG2>& b)
{
    return Monom<RT,DEG1+DEG2>(a.coeff * b.coeff);
}

// **************************
//      OPERATORS
// **************************

template<class OS, class RT, int DEG>
OS& operator<<(OS& os, const sym::Monom<RT,DEG>& m) { return m.dump(os); }

// trivial arithmetic operators are always defined

template<class RT, int DEG>
auto operator+(const sym::Monom<RT,DEG>& a, const sym::Monom<RT,DEG>& b) { return sym::add_op(a,b); }

template<class RT, int DEG>
auto operator-(const sym::Monom<RT,DEG>& a, const sym::Monom<RT,DEG>& b)
{
    return sym::Monom<RT,DEG>(a.coeff - b.coeff);
}

template<class RT, int DEG1, int DEG2>
auto operator*(const sym::Monom<RT,DEG1>& a, const sym::Monom<RT,DEG2>& b) { return sym::mul_op(a, b); }

template<class RT, int DEG, int N>
auto operator^(const sym::Monom<RT,DEG>& a, const sym::_<N>& ) { return sym::pow_op<N>(a); }

}

#endif
