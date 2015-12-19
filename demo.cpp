#include<sympoly/polynomial.hpp>
#include<sympoly/operators.hpp>
//#include<sympoly/lazy_operators.hpp>
#include<sympoly/debug.hpp>

template<class T>
void dump(const T& a, const std::string& name)
{
    std::cout << name << " = " << a << std::endl;
    std::cout << "typeid(" << name << ") = " << sym::demangle(typeid(a).name()) << std::endl;
    std::cout << "sizeof(" << name << ") = " << sizeof(a) << std::endl;
}

// exponents
sym::_<2> _2;
sym::_<3> _3;
sym::_<4> _4;
sym::_<5> _5;

int main()
{
    sym::Monom<int,0,double,'y'> x1;
//    dump(x1, "x1");
    sym::Monom<int,2,double,'y'> x2(2);
    sym::Monom<int,3,double,'y'> x3(3);
    sym::Monom<int,4,double,'y'> x4(4);
    auto a = (x1+x2)*(x3+x4)*(x1+x2+x3+x4);
    auto a2 = 2*a+a;
//    auto a = x1^_2;
    dump(a,"a");
    dump(a2,"a2");

    auto b = expand(a2);
    dump(b,"b");

    std::cout << a2(2.0) << std::endl;
    std::cout << b(2) << std::endl;
}
