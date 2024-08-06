#pragma once

#include "ankerl/unordered_dense.h"
#include <sys/types.h>

typedef u_int8_t u8;
typedef u_int16_t u16;
typedef u_int32_t u32;
typedef u_int64_t u64;

typedef int i32;

typedef float fp32;
typedef double fp64;

template <typename K, typename V>
using map = ankerl::unordered_dense::map<K, V>;

template <typename K> using set = ankerl::unordered_dense::set<K>;
