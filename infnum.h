#pragma once

#include<cstdint>
#include<iostream>
#include<vector>
#include<cstring>
#include<algorithm>
#include<cmath>

namespace infnum {
typedef uint64_t u64;
typedef int64_t i64;

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

	template<typename T>
	infnum(T t){
		if(std::is_integral_v<T>) {
			if(t < 0) {
				sign = 1;
				t *= -1;
			}
			std::memcpy(&data[1], &t, sizeof(t));
		}
		else if(std::is_floating_point_v<T>) {
			int exp;
			double val = std::frexp(t, &exp);
			
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

	infnum() {}
	infnum(std::initializer_list<u64> v) {data = v;}
	infnum(std::string str);
	infnum(const char* str);

	template<typename T>
	void operator=(T t) { *this = infnum(t); }
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
	infnum operator%(infnum other) const;
	infnum operator^(infnum other) const;
	
	infnum operator-();
	
	void operator+=(infnum other);
	void operator-=(infnum other);
	void operator*=(infnum other);
	void operator/=(infnum other);
	void operator%=(infnum other);
	void operator^=(infnum other);
	
	infnum operator>>(int count) const;
	infnum operator<<(int count) const;
	
	void operator>>=(int count);
	void operator<<=(int count);

	std::size_t size() const;
	u64& operator[](std::size_t index);
	u64 operator[](std::size_t index) const;

};

infnum round(infnum x); 
infnum floor(infnum x); 
infnum ceil(infnum x); 

infnum abs(infnum x); 

bool isInteger(infnum x); 

infnum min(infnum x, infnum y);
infnum min(std::vector<infnum> v);
infnum max(infnum x, infnum y);
infnum max(std::vector<infnum> v);
infnum exp(infnum x);
infnum ln(infnum x);

i64 mostSignificantBit(infnum x);

const infnum PI({2611923443488327889ULL, 3ULL});
const infnum E({13249961062380153449ULL, 2ULL});
}

std::ostream& operator<<(std::ostream& o, const infnum::infnum& n);

