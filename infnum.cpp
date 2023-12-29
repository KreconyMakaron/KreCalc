#include"infnum.h"

namespace infnum {
const infnum BASE = (infnum)1 << 64;

infnum biggest_multiple(u64 limit, infnum query, infnum mult) {
	int l = -1, r = limit+1, mid;

	while(r - l > 1) {
		mid = (l + r) / 2;
		infnum infmid = ((infnum)1 << mid) * mult;
		if(query < infmid) r = mid;
		else l = mid;
	}
	if(l == -1) return -1;
	return (infnum)1 << l;
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

//https://en.wikipedia.org/wiki/Long_division#Algorithm_for_arbitrary_base
infnum infnum::operator/(infnum other) const {
	if(other == 0) throw std::overflow_error("Divide by zero exception");
	bool sign = this->sign ^ other.sign;

	const u64 SCALING_FACTOR = 2;
	infnum scaled_this = this->longShiftLeft(SCALING_FACTOR);

	other.sign = 0;
	scaled_this.sign = 0;

	int k = scaled_this.size(), l = other.size();
	if(l > k) return 0;

	std::vector<infnum> q(k-l+2), r(k-l+2);
	q[0] = 0;
	
	//First l-1 digits of divident
	r[0] = scaled_this.longShiftRight(l-1);

	for(int i = 1; i <= k-l+1; ++i) {
		infnum d = r[i-1] * BASE + scaled_this.data[k-1-(i+l-2)];

		//binary search biggest B so that B*other <= d
		infnum beta = 0;
		while(beta*other != d && d - other * beta >= other) beta += biggest_multiple(d.size() * 64, d-beta*other, other);

		r[i] = d - beta * other;
		q[i] = q[i-1].longShiftLeft(1) + beta;
	}
	infnum res = q[k-l+1];
	res.sign = sign;
	res = res.longShiftRight(2*SCALING_FACTOR);
	if(this->size() > 2) res = res.longShiftRight(this->size()-2);
	res.removeLeadingZeros();
	return res;
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

infnum infnum::operator>>(int count) const{
	if(count < 0) return *this << (-1 * count);
	infnum temp = this->longShiftRight(count / 64);
	int rest = count % 64;
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

u64& infnum::operator[](const std::size_t& index) {
	if(index > this->size()-1) throw std::out_of_range((std::string)"index out of range: index = " + std::to_string(index) + ", size = " + std::to_string(this->size()));
	else return this->data[index];
}

u64 infnum::operator[](const std::size_t& index) const {
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

}

// shit is slow - gotta fix at some point
std::ostream& operator<<(std::ostream& o, infnum::infnum& n) {
	if(n.sign == 1) o << '-';

	infnum::infnum temp = infnum::floor(n);

	std::string integer;
	if(temp == 0) integer = "0";

	while(temp != 0) {
		infnum::infnum div10 = infnum::floor(temp / 10);
		infnum::infnum digit = temp - div10*10;
		integer += '0' + digit.data[1];
		temp = div10;
	}
	std::reverse(integer.begin(), integer.end());

	infnum::u64 mult = 1ULL << 63;
	infnum::u64 hihi = 0;
	for(int i = 0; i < 64; ++i) {
		if(n[0] & mult) hihi += mult;
		mult >>= 1;
	}

	std::string decimal = std::to_string((long double)hihi / (long double)UINT64_MAX);
	decimal.erase(decimal.begin());

	o << integer << decimal;

	return o;
}
