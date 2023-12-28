#pragma once

#include<cstdint>
#include<iostream>
#include<vector>
#include<cstring>
#include<algorithm>
#include<cmath>

namespace infnum {
typedef uint64_t u64;

class infnum {

private:
	template<typename T>
	void putInteger(T t) {
		if(t < 0) {
			sign = 1;
			t *= -1;
		}
		std::memcpy(&data[1], &t, sizeof(t));
	}

	template<typename T>
	void putFloat(T t) {
		int exp;
		T val = std::frexp(t, &exp);
		
		for(int i = 63; i >= 0; --i) {
			val *= 2;
			if(val >= 1) {
				data[0] += (1ULL << i);
				val -= 1;
			}
		}
		*this <<= exp;
	}

	void removeLeadingZeros();
	
	infnum add(const infnum& other) const;
	infnum subtract(const infnum& other) const;
	
	void longShiftLeft(int count);
	void longShiftRight(int count);
	
public:
	std::vector<u64> data = {0, 0}; //64 bits binary expansion
	bool sign = 0;

	infnum(std::vector<u64> v) {data = v;}
	template<typename T>
	infnum(T t){ *this = t; }
	infnum() {}
	
	template<typename T>
	void operator=(const T t) {
		*this = infnum();
		if(std::is_integral_v<T>) putInteger(t);
		else if(std::is_floating_point_v<T>) putFloat(t);
	}	
	bool operator==(const infnum& other) const;
	bool operator!=(const infnum& other) const;
	bool operator>(const infnum& other) const;
	bool operator>=(const infnum& other) const;
	bool operator<(const infnum& other) const;
	bool operator<=(const infnum& other) const;
	
	infnum operator+(const infnum& other) const;
	infnum operator-(const infnum& other) const;
	infnum operator*(const infnum& other) const;
	infnum operator/(infnum other) const;
	
	infnum operator-();
	
	void operator+=(const infnum& other);
	void operator-=(const infnum& other);
	
	infnum operator>>(const int& count) const;
	infnum operator<<(const int& count) const;
	
	void operator>>=(const int& count);
	void operator<<=(const int& count);

	std::size_t size() const;
	u64& operator[](const std::size_t& index);
	u64 operator[](const std::size_t& index) const;
};

u64 highBits(const u64& x);
u64 lowBits(const u64& x);
u64 mult_u64(const u64& x, const u64& y, u64& result);

infnum round(infnum x); 
infnum floor(infnum x); 
infnum ceil(infnum x); 
infnum abs(infnum x); 
infnum biggest_multiple(const u64 limit, const infnum query, const infnum mult);

//remove
}

std::ostream& operator<<(std::ostream& o, infnum::infnum& n);
std::string pinrt(infnum::infnum x);
