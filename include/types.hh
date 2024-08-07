#pragma once

#include "ankerl/unordered_dense.h"
#include <sys/types.h>
#include <utility>

typedef u_int8_t u8;
typedef u_int16_t u16;
typedef u_int32_t u32;
typedef u_int64_t u64;

typedef int i32;

typedef float fp32;
typedef double fp64;

template <typename T> struct hash : public ankerl::unordered_dense::hash<T> {};

template <typename T, typename U> struct hash<std::pair<T, U>> {
  auto operator()( const std::pair<T, U> &pair ) const noexcept {
    auto h1 = hash<T>{}( pair.first );
    auto h2 = hash<U>{}( pair.second );
    return ankerl::unordered_dense::detail::wyhash::mix( h1, h2 );
  }
};

template <typename K, typename V>
using map = ankerl::unordered_dense::map<K, V, hash<K>>;

template <typename K> using set = ankerl::unordered_dense::set<K, hash<K>>;
