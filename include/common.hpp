#pragma once

#include <map>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <unordered_map>

using u8 = unsigned char;
using u16 = unsigned short;
using u32 = unsigned int;
using u64 = unsigned long long;

using i8 = signed char;
using i16 = signed short;
using i32 = signed int;
using i64 = signed long long;

using f32 = float;
using f64 = double;
using f80 = long double;

using usize = unsigned long long;
using isize = signed long long;

using string = std::string;
using string_ref = std::string &;

template <typename T> using vec = std::vector<T>;
template <typename T1, typename T2> using map = std::map<T1, T2>;
