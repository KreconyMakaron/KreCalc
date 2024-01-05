#include"infnum.h"
#include <cstdint>

namespace infnum {

std::string print(infnum x) {
	std::string res = "";
	for(auto it = x.data.rbegin(); it != x.data.rend(); ++it) res += std::to_string(*it) + ' ';
	return res;
}

u64 highBits(u64 x) {
	return x >> 32ULL;
}

u64 lowBits(u64 x) {
	return x & ((1ULL << 32) - 1);
}

u64 mult_u64(u64 a, u64 b, u64& carry) {
	u64 s0, s1, s2, s3; 
	
	u64 x = lowBits(a) * lowBits(b);
	s0 = lowBits(x);
	
	x = highBits(a) * lowBits(b) + highBits(x);
	s1 = lowBits(x);
	s2 = highBits(x);
	
	x = s1 + lowBits(a) * highBits(b);
	s1 = lowBits(x);
	
	x = s2 + highBits(a) * highBits(b) + highBits(x);
	s2 = lowBits(x);
	s3 = highBits(x);
	
	carry = s3 << 32 | s2;
	return s1 << 32 | s0;
}

void infnum::removeLeadingZeros() {
	while(*(this->data.end()-1) == 0 && this->size() > 2) this->data.pop_back();
}

infnum infnum::add(infnum other) const {
	int n = std::max(this->size(), other.size());
	bool carry = 0;
	infnum a = *this, b = other;
	a.sign = 0; b.sign = 0;
	
	a.data.resize(n); b.data.resize(n);
	u64 t;
	
	for(int i = 0; i < n; ++i) {
		t = b[i] + carry;
		carry = a[i] + carry > UINT64_MAX - b[i] || a[i] > UINT64_MAX - carry;
		a[i] += t;
	}
	if(carry) a.data.push_back(1);
	return a;
}

infnum infnum::subtract(infnum other) const {
	int n = std::max(this->size(), other.size());
	infnum a = *this, b = other;
	a.sign = 0; b.sign = 0;
	if(b > a) {
		std::swap(a, b);
		a.sign = 1;
	}
	a.data.resize(n); b.data.resize(n);
	
	for(int i = 0; i < n; ++i) {
		if(a[i] < b[i]) {
			b[i]--;
			a[i] += UINT_LEAST64_MAX;
			int k = i+1;
			while(k < n) {
				if(a[k]-- > 0) break;
				k++;
			}
		}
		a[i] -= b[i];
	}
	a.removeLeadingZeros();
	return a;
}

infnum infnum::longShiftLeft(int count) const {
	infnum temp = *this;
	std::reverse(temp.data.begin(), temp.data.end());
	for(int i = 0; i++ < count;) temp.data.push_back(0);
	std::reverse(temp.data.begin(), temp.data.end());
	temp.removeLeadingZeros();
	return temp;
}

infnum infnum::longShiftRight(int count) const {
	infnum temp = *this;
	std::reverse(temp.data.begin(), temp.data.end());
	for(int i = 0; i++ < count && !temp.data.empty();) temp.data.pop_back();
	std::reverse(temp.data.begin(), temp.data.end());
	while(temp.size() < 2) temp.data.push_back(0);
	return temp;
}

bool infnum::operator==(infnum other) const {
	if(this->sign != other.sign) return 0;
	if(this->size() != other.size()) return 0;
	for(int i = this->size()-1; i >= 0; --i) if(this->data[i] != other[i]) return 0;
	return 1;
}

bool infnum::operator!=(infnum other) const {
	return !(*this == other);
}

bool infnum::operator>(infnum other) const {
	if(sign != other.sign) return sign < other.sign;
	if(this->size() != other.size()) return this->size() > other.size();
	for(int i = this->size()-1; i >= 0; --i) if(this->data[i] != other[i]) return this->data[i] > other[i];
	return 0;
}

bool infnum::operator>=(infnum other) const {
	return *this > other || *this == other;
}

bool infnum::operator<(infnum other) const {
	return !(*this >= other);
}

bool infnum::operator<=(infnum other) const {
	return !(*this > other);
}

infnum infnum::operator+(infnum other) const {
	if(this->sign != other.sign) {
		if(this->sign == 1) return -this->subtract(other);
		return this->subtract(other);
	}
	if(this->sign) return -this->add(other);
	return this->add(other);
}

infnum infnum::operator-(infnum other) const {
	if(this->sign != other.sign) {
		if(this->sign == 1) return -this->add(other);
		return this->add(other);
	}
	if(this->sign) return -this->subtract(other);
	return this->subtract(other);
}

infnum infnum::operator*(infnum other) const {
	infnum c, d, temp;
	u64 carry = 0;
	for(int i = 0; i < this->size(); ++i) {
		temp = 0;
		for(int j = 0; j < other.size(); ++j) {
			d = carry; 
			d += mult_u64(this->data[i], other[j], carry);
			temp += d.longShiftLeft(j);
			if(j != other.size() - 1 && j >= 2 && d == 0) temp.data.push_back(0);
		}
		if(carry) {
			if(d == 0) temp.data.push_back(0);
			temp.data.push_back(carry);
			carry = 0;
		}
		c += temp.longShiftLeft(i);
	}
	c.sign = this->sign ^ other.sign;
	return c.longShiftRight(2);
}

infnum infnum::operator/(infnum other) const {
	if(other == 0) throw std::logic_error("Divide Operation: Divide by zero exception");

	infnum w = this->longShiftLeft(1);
	infnum res = 0;
	infnum power = (infnum)1 << (w.size() * 64);
	while(w >= other) {
		infnum mult = power * other;
		if(w >= mult) {
			w -= mult;
			res += power;
		}
		power >>= 1;
	}

	res.sign = this->sign ^ other.sign;
	return res.longShiftRight(1);
}

infnum infnum::operator%(infnum other) const {
	if(!isInteger(*this) || !isInteger(other)) throw std::logic_error("Modulo Operation: Number is not an integer");
	if(other == 0) throw std::logic_error("Modulo Operation: Divide by zero exception");
	return *this - (floor(*this/other)*other);
}

infnum infnum::operator-() {
	this->sign = !this->sign;
	return *this;
}

void infnum::operator+=(infnum other) {
	*this = *this + other;
}

void infnum::operator-=(infnum other) {
	*this = *this - other;
}

void infnum::operator*=(infnum other) {
	*this = *this * other;
}

void infnum::operator/=(infnum other) {
	*this = *this / other;
}

void infnum::operator%=(infnum other) {
	*this = *this / other;
}

infnum infnum::operator>>(int count) const{
	if(count < 0) return *this << (-1 * count);
	infnum temp = this->longShiftRight(count / 64);
	int rest = count % 64;
	if(rest == 0) return temp;

	u64 mask = ((1 << rest) - 1);
	
	for(int i = 0; i < (int)this->size(); ++i) {
		temp[i] >>= rest;
		u64 x = (i == (int)temp.size()-1 ? 0 : mask & temp[i+1]);
		temp[i] += x << (64 - rest);
	}

	temp.removeLeadingZeros();
	return temp;
}

infnum infnum::operator<<(int count) const {
	if(count < 0) return *this >> (-1 * count);
	infnum temp = this->longShiftLeft(count / 64);
	int rest = count % 64;
	if(rest == 0) return temp;

	u64 mask = ((1 << rest) - 1);
	mask <<= (64 - rest);

	if(temp.data.back() & mask) temp.data.push_back(temp.data.back() & mask);
	std::reverse(temp.data.begin(), temp.data.end());
	for(int i = 0; i < (int)temp.size(); ++i) {
		temp[i] <<= rest;
		u64 x = (i == (int)temp.size()-1 ? 0 : mask & temp[i+1]);
		temp[i] += x >> (64 - rest);
	}
	std::reverse(temp.data.begin(), temp.data.end());

	return temp;
}

void infnum::operator>>=(int other) {
	*this = *this >> other;
}

void infnum::operator<<=(int other) {
	*this = *this << other;
}

std::size_t infnum::size() const {
	return this->data.size();
}

u64& infnum::operator[](std::size_t index) {
	if(index > this->size()-1) throw std::out_of_range((std::string)"index out of range: index = " + std::to_string(index) + ", size = " + std::to_string(this->size()));
	else return this->data[index];
}

u64 infnum::operator[](std::size_t index) const {
	if(index > this->size()-1) throw std::out_of_range((std::string)"index out of range: index = " + std::to_string(index) + ", size = " + std::to_string(this->size()));
	else return this->data[index];
}

infnum round(infnum x) {
	if(x[0] >= (1ULL << 63)) x += infnum({0, 1});
	x[0] = 0;
	return x;
}

infnum floor(infnum x) {
	x[0] = 0;
	return x;
}

infnum ceil(infnum x) {
	if(x[0] != 0) x += infnum({0, 1});
	x[0] = 0;
	return x;
}


infnum abs(infnum x) {
	x.sign = 0;
	return x;
}

bool isInteger(infnum x) {
	return x[0] == 0;
}

infnum min(infnum x, infnum y) {
	return (x < y ? x : y);
}

infnum min(std::vector<infnum> v) {
	infnum res = v[0];
	for(auto it = v.begin()+1; it != v.end(); ++it) res = min(res, v[1]);
	return res;
}

infnum max(infnum x, infnum y) {
	return (x > y ? x : y);
}

infnum max(std::vector<infnum> v) {
	infnum res = v[0];
	for(auto it = v.begin()+1; it != v.end(); ++it) res = min(res, v[1]);
	return res;
}


}

// shit is slow - gotta fix at some point
std::ostream& operator<<(std::ostream& o, infnum::infnum& n) {
	if(n.sign == 1) o << '-';

	infnum::infnum temp = infnum::floor(n);

	std::string integer;
	if(temp == 0) integer = "0";

	while(temp != 0) {
		infnum::infnum div10 = floor(temp / 10);
		integer += '0' + (temp - div10*10)[1];
		temp = div10;
	}
	std::reverse(integer.begin(), integer.end());

	//This has bad precision gotta change
	infnum::infnum numerator = n[0];
	infnum::infnum res;
	std::string decimal;
	for(int i = 0; i < 19; ++i) {
		numerator *= 10;
		res = floor(numerator >> 64);
		decimal += '0' + res[1];
		numerator -= res << 64;
	}

	while(!decimal.empty() && decimal.back() == '0') decimal.pop_back();

	o << integer << '.' << decimal;
	return o;
}
