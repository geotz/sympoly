#ifndef _SYMPOLY_FUNCTIONAL_SUPPORT_H_
#define _SYMPOLY_FUNCTIONAL_SUPPORT_H_

#include<type_traits>

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
struct Compose;

template<int DEG, int... DEGs>
struct Compose<DEG,Support<DEGs...>>
{
    typedef Support<DEG,DEGs...> Result;
};

template<int, class>
struct Translate;

template<int K, int DEG1>
struct Translate<K,Support<DEG1>>
{
    typedef Support<K+DEG1> Result;
};

template<int K, int DEG1, int... DEGs>
struct Translate<K,Support<DEG1,DEGs...>>
{
    typedef typename Compose<K+DEG1,
                            typename Translate<K,Support<DEGs...>>::Result>::Result Result;
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
                typename Compose<DEG1,
                       typename Union<Support<DEG1s...>, Support<DEG2,DEG2s...>>::Result
                       >::type,
                 typename std::conditional<DEG1 == DEG2,
                        typename Compose<DEG1,typename Union<Support<DEG1s...>,Support<DEG2s...>>::Result>::Result,
                        typename Compose<DEG2,
                               typename Union<Support<DEG1,DEG1s...>, Support<DEG2s...>>::Result>::Result
                        >::type
              >::type Result;
};

template<int DEG1,int DEG2, int... DEG2s>
struct Union<Support<DEG1>,Support<DEG2,DEG2s...>>
{
    typedef typename std::conditional< DEG1 < DEG2,
                typename Compose<DEG1,
                       Support<DEG2,DEG2s...>
                       >::Result,
                typename std::conditional<DEG1 == DEG2,
                        Support<DEG2,DEG2s...>,
                        typename Compose<DEG2,
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

}

#endif
