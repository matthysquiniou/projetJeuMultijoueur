#pragma once
#include <functional>
#include <algorithm> 

template<typename A, typename B>
struct PairKey {
    A a;
    B b;
};

template<typename A, typename B>
struct PairKeyHash {
    std::size_t operator()(const PairKey<A,B>& k) const noexcept {
        return std::hash<A>()(k.a) ^ (std::hash<B>()(k.b) << 1);
    }
};

template<typename A, typename B>
struct PairKeyEqual {
    bool operator()(const PairKey<A,B>& x, const PairKey<A,B>& y) const noexcept {
        return (x.a == y.a && x.b == y.b);
    }
};
