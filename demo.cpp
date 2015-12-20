#include<sympoly/polynomial.hpp>
#include<sympoly/operators.hpp>
//#include<sympoly/lazy_operators.hpp>
#include<sympoly/utils.hpp>

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
    sym::Monom<int,0,double,'t'> x1;
//    dump(x1, "x1");
    sym::Monom<int,2,double,'t'> x2(2);
    sym::Monom<int,3,double,'t'> x3(3);
    sym::Monom<int,4,double,'t'> x4(4);
    auto a = (x1+x2)*(x3+x4)+(x3+x4);
    auto a2 = 0*a;
//    auto a = x1^_2;
    dump(a,"a");
    auto da5 = !!!!!a;
    dump(da5, "a'5");
    sym::benchmark("degree(a)", [a] { a.degree(); }, 1000000);
    std::cout << "deg(a'5) = " << da5.degree() << std::endl;
    for (int i = 0; i <= da5.degree(); ++i) {
        std::cout << "coeff(a'5," << i <<") = " << da5.coeff(i) << std::endl;
    }
    dump(a2,"a2");

    auto b = expand(a2);
    dump(b,"b");

    std::cout << a2(2.0) << std::endl;
    std::cout << b(2) << std::endl;
}
