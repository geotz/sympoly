#ifndef _SYMPOLY_OPERATORS_H_

#ifdef _SYMPOLY_LAZY_OPERATORS_H_
#error Lazy operators (already included) cannot be mixed with requested normal operators.
#endif

#define _SYMPOLY_OPERATORS_H_

#include "polynomial.hpp"

namespace sym
{

template<class A, class B>
auto operator+(const A& a, const B& b) { return add_op(a,b); }

template<class A,class B>
auto operator*(const A& a, const B& b) { return mul_op(a,b); }

}

#endif

