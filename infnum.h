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
	void removeLeadingZeros();
	
	infnum add(infnum other) const;
	infnum subtract(infnum other) const;
	
	infnum longShiftLeft(int count) const;
	infnum longShiftRight(int count) const;
	
public:
	std::vector<u64> data = {0, 0}; //64 bits binary expansion
	bool sign = 0;

	infnum(std::vector<u64> v) {data = v;}

	template<typename T>
	infnum(T t){ *this = t; }
	infnum() {}
	
	template<typename T>
	void operator=(T t) {
		static_assert(std::is_integral_v<T> || std::is_floating_point_v<T>, "No matching conversion to type infnum");

		*this = infnum();
		if(std::is_integral_v<T>) {
			if(t < 0) {
				sign = 1;
				t *= -1;
			}
			std::memcpy(&data[1], &t, sizeof(t));
		}
		else if(std::is_floating_point_v<T>) {
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
	}	

	bool operator==(infnum other) const;
	bool operator!=(infnum other) const;
	bool operator>(infnum other) const;
	bool operator>=(infnum other) const;
	bool operator<(infnum other) const;
	bool operator<=(infnum other) const;
	
	infnum operator+(infnum other) const;
	infnum operator-(infnum other) const;
	infnum operator*(infnum other) const;
	infnum operator/(infnum other) const;
	
	infnum operator-();
	
	void operator+=(infnum other);
	void operator-=(infnum other);
	
	infnum operator>>(int count) const;
	infnum operator<<(int count) const;
	
	void operator>>=(int count);
	void operator<<=(int count);

	std::size_t size() const;
	u64& operator[](const std::size_t& index);
	u64 operator[](const std::size_t& index) const;
};

infnum round(infnum x); 
infnum floor(infnum x); 
infnum ceil(infnum x); 
infnum abs(infnum x); 
}

std::ostream& operator<<(std::ostream& o, infnum::infnum& n);
