#pragma once
#include<cstdint>
#include<iostream>
#include<vector>
#include<cstring>
#include<algorithm>
#include<cmath>

class infnum {
    typedef uint64_t u64;
    typedef __int128 i128;

private:
    template<typename T>
    void putBits(T t);
    
    template<typename T>
    void putFloat(T t);
    
    void removeLeadingZeros();
    
    infnum add(const infnum& other);
    infnum subtract(const infnum& other);
    
    void longShiftLeft(int count);
    void longShiftRight(int count);
    
public:
    std::vector<u64> data = {0, 0}; //64 bits binary expansion
    bool sign = 0;

    template<typename T>
    infnum(T t){ *this = t; }
    infnum() {}
    
    template<typename T>
    void operator=(const T t);
    bool operator==(const infnum& other);
    
    bool operator>(const infnum& other);
    bool operator>=(const infnum& other);
    bool operator<(const infnum& other);
    bool operator<=(const infnum& other);
    
    infnum operator-(const infnum& other);
    infnum operator+(const infnum& other);
    
    infnum operator-();
    
    void operator+=(const infnum& other);
    void operator-=(const infnum& other);
    
    infnum operator>>(const int& count);
    infnum operator<<(const int& count);
    
    void operator>>=(const int& count);
    void operator<<=(const int& count);
};

