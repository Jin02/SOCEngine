//
//  SOCHashMap.h
//  Rendering
//
//  Created by Jiman Jeong on 13. 8. 17..
//  Copyright (c) 2013년 SOC. All rights reserved.
//

#ifndef HashMap_h
#define HashMap_h

#if defined(WIN32)

#include <hash_map>

template<typename key, typename value>
class SOCHashMap : public std::hash_map<key, value> {};

#elif defined(__APPLE__)

#ifndef __DEPRECATED
#include <ext/hash_map>
template <typename key, typename value>
class SOCHashMap : public __gnu_cxx::hash_map<key, value> {};
#else
#include <unordered_map>
template <typename key, typename value>
class SOCHashMap : public std::unordered_map<key, value> {};
#endif

#endif

#endif
