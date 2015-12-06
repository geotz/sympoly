#ifndef _SYMPOLY_DEBUG_H_
#define _SYMPOLY_DEBUG_H_

#include<cxxabi.h>
#include<string>
#include<cstdlib>

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
