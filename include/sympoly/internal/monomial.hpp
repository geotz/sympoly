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

template<class RT, int DEG = 0, class FT = double, char SYM = 'x'>
class Monom
{
public:
    typedef RT ring_type;
    typedef FT field_type;
    RT coeff;

    Monom( RT c = RT(1) ): coeff(c) {}

    static constexpr int degree = DEG;
    static constexpr char symbol = SYM;
    bool is_zero() const { return coeff == RT(0); }
    FT operator()(FT x, int content=0) const { return eval(*this, x, content); }
    FT power(FT x, int content=0) const { return std::pow(x, DEG-content); }
//    const Monom& expand() const { return *this; }
    template<class OS>
    OS& dump(OS& os) const {
        if (degree == 0) return os << coeff;
        if (is_zero()) return os << "0";
        if (coeff == RT(-1)) {
            os << "-";
        } else if (coeff != RT(1) ) {
            os << coeff;
        }
        os << symbol;
        if (degree > 1) os << "^" << degree;
        return os;
    }
};

template<class RT, int DEG, class FT, char SYM>
FT eval(const Monom<RT,DEG,FT,SYM>& a, FT x, int content=0)
{
    return a.is_zero() ? 0 : a.coeff * a.power(x,content);
}

template<class T>
auto expand(const T& expr) { return expr; }

// TODO: assert unsigned?
template<int N,class RT,int DEG,class FT,char SYM>
auto pow_op(const Monom<RT,DEG,FT,SYM>& a)
{
    return Monom<RT,DEG*N,FT,SYM>(cpow(a.coeff,N));
}

template<class RT, int DEG, class FT, char SYM>
Monom<RT,DEG,FT,SYM> add_op(const Monom<RT,DEG,FT,SYM>& a, const Monom<RT,DEG,FT,SYM>& b)
{
    return Monom<RT,DEG,FT,SYM>(a.coeff + b.coeff);
}

template<class RT, int DEG1, int DEG2, class FT, char SYM>
Monom<RT,DEG1+DEG2,FT,SYM> mul_op(const Monom<RT,DEG1,FT,SYM>& a, const Monom<RT,DEG2,FT,SYM>& b)
{
    return Monom<RT,DEG1+DEG2,FT,SYM>(a.coeff * b.coeff);
}

template<class RT, int DEG, class FT, char SYM>
Monom<RT,DEG,FT,SYM> mul_op(const RT& a, const Monom<RT,DEG,FT,SYM>& b)
{
    return Monom<RT,DEG,FT,SYM>(a * b.coeff);
}

template<class RT, int DEG, class FT, char SYM>
auto mul_op(const Monom<RT,DEG,FT,SYM>& a, const RT& b) { return mul_op(b, a); }

// **************************
//      OPERATORS
// **************************

template<class OS, class RT, int DEG, class FT, char SYM>
OS& operator<<(OS& os, const Monom<RT,DEG,FT,SYM>& m) { return m.dump(os); }

// trivial arithmetic operators are always defined

template<class RT,int DEG, class FT, char SYM>
auto operator*(const Monom<RT,DEG,FT,SYM>& a, const RT& b) { return mul_op(a,b); }

template<class RT,int DEG, class FT, char SYM>
auto operator*(const RT& a, const Monom<RT,DEG,FT,SYM>& b) { return mul_op(a,b); }

template<class RT, int DEG, class FT, char SYM>
auto operator+(const Monom<RT,DEG,FT,SYM>& a, const Monom<RT,DEG,FT,SYM>& b) { return add_op(a,b); }

template<class RT, int DEG, class FT, char SYM>
auto operator-(const Monom<RT,DEG,FT,SYM>& a, const Monom<RT,DEG,FT,SYM>& b)
{
    return Monom<RT,DEG>(a.coeff - b.coeff);
}

template<class RT, int DEG1, int DEG2, class FT, char SYM>
auto operator*(const Monom<RT,DEG1,FT,SYM>& a, const Monom<RT,DEG2,FT,SYM>& b) { return mul_op(a, b); }

template<class RT, int DEG, int N, class FT, char SYM>
auto operator^(const Monom<RT,DEG,FT,SYM>& a, const _<N>& ) { return pow_op<N>(a); }

}

#endif
