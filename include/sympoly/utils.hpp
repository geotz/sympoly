#ifndef _SYMPOLY_UTILS_H_
#define _SYMPOLY_UTILS_H_

#include<cxxabi.h>
#include<string>
#include<cstdlib>
#include<chrono>

namespace sym
{

inline std::string demangle(const std::string& name)
{

    int status; // some arbitrary value to eliminate the compiler warning

    std::unique_ptr<char, void(*)(void*)> res {
        abi::__cxa_demangle(name.c_str(), nullptr, nullptr, &status),
        std::free
    };

    return (status==0) ? res.get() : name ;
}

template<class F>
void benchmark(const std::string& name, const F& pred, int n = 1)
{
    std::cout << "measuring " << name << "... ";
    auto begin = std::chrono::steady_clock::now();
    for (int i = 0; i < n; ++i) pred();
    auto end = std::chrono::steady_clock::now();
    auto diff = end - begin;
    std::cout << std::chrono::duration<double,std::milli>(diff).count() << " ms for " << n << " executions" << std::endl;
}

//template<class RT>
//RT ipow(RT x, int exp)
//{
////    std::cerr << "pow(" << x <<"," <<exp << ")"<<std::endl;
//    RT res = 1;
//    while (exp) {
//        if (exp & 1) res *= x;
//        exp >>= 1;
//        x *= x;
//    }
//    return res;
//}

}

#endif
