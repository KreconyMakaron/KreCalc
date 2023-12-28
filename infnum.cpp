#include"infnum.h"
#include<iomanip>

namespace infnum {
const infnum BASE = (infnum)1 << 64;

void infnum::removeLeadingZeros() {
	while(*(this->data.end()-1) == 0 && this->size() > 2) this->data.pop_back();
}

u64 highBits(const u64& x) {
	return x >> 32ULL;
}

u64 lowBits(const u64& x) {
	return x & ((1ULL << 32) - 1);
}

infnum infnum::add(const infnum& other) const {
	int n = std::max(this->size(), other.data.size());
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

infnum infnum::subtract(const infnum& other) const {
	int n = std::max(size(), other.data.size());
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

void infnum::longShiftLeft(int count) {
	std::reverse(data.begin(), data.end());
	for(int i = 0; i++ < count;) data.push_back(0);
	std::reverse(data.begin(), data.end());
	removeLeadingZeros();
}

void infnum::longShiftRight(int count) {
	std::reverse(data.begin(), data.end());
	for(int i = 0; i++ < count && !data.empty();) data.pop_back();
	std::reverse(data.begin(), data.end());
	while(size() < 2) data.push_back(0);
}

bool infnum::operator==(const infnum& other) const {
	if(this->sign != other.sign) return 0;
	if(this->size() != other.size()) return 0;
	for(int i = this->size()-1; i >= 0; --i) if(this->data[i] != other[i]) return 0;
	return 1;
}

bool infnum::operator!=(const infnum& other) const {
	return !(*this == other);
}

bool infnum::operator>(const infnum& other) const {
	if(sign != other.sign) return sign < other.sign;
	if(this->size() != other.size()) return this->size() > other.size();
	for(int i = this->size()-1; i >= 0; --i) if(this->data[i] != other[i]) return this->data[i] > other[i];
	return 0;
}

bool infnum::operator>=(const infnum& other) const {
	return *this > other || *this == other;
}

bool infnum::operator<(const infnum& other) const {
	return !(*this >= other);
}

bool infnum::operator<=(const infnum& other) const {
	return !(*this > other);
}

infnum infnum::operator+(const infnum& other) const {
	if(this->sign != other.sign) {
		if(this->sign == 1) return -this->subtract(other);
		return this->subtract(other);
	}
	if(this->sign) return -this->add(other);
	return this->add(other);
}

infnum infnum::operator-(const infnum& other) const {
	if(this->sign != other.sign) {
		if(this->sign == 1) return -this->add(other);
		return this->add(other);
	}
	if(this->sign) return -this->subtract(other);
	return this->subtract(other);
}

u64 mult_u64(const u64& a, const u64& b, u64& carry) {
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

infnum infnum::operator*(const infnum& other) const {
	infnum c, d, temp;
	u64 carry = 0;
	for(int i = 0; i < this->size(); ++i) {
		temp = 0;
		for(int j = 0; j < other.size(); ++j) {
			d = carry; 
			d += mult_u64(this->data[i], other[j], carry);
			d.longShiftLeft(j);
			/* std::cout << temp << " + " << d; */
			temp += d;
			if(j != other.size() - 1 && j >= 2 && d == 0) temp.data.push_back(0);
			/* std::cout << " = " << temp << '\n'; */
			/* std::cout << "i: " << i << ", j: " << j << ", d: " << d << ", carry: " << carry << '\n'; */
		}
		if(carry) {
			if(d == 0) temp.data.push_back(0);
			temp.data.push_back(carry);
			carry = 0;
		}
		/* std::cout << temp << '\n'; */
		temp.longShiftLeft(i);
		c += temp;
		/* std::cout << "chuj: " << c << '\n'; */
	}
	c.longShiftRight(2);
	c.sign = this->sign ^ other.sign;
	return c;
}

//https://en.wikipedia.org/wiki/Long_division#Algorithm_for_arbitrary_base
infnum infnum::operator/(infnum other) const {
	if(other == 0) throw std::overflow_error("Divide by zero exception");
	bool sign = this->sign ^ other.sign;

	const u64 SCALING_FACTOR = 2;
	infnum scaled_this = *this;
	scaled_this.longShiftLeft(SCALING_FACTOR);

	other.sign = 0;
	scaled_this.sign = 0;

	int k = scaled_this.size(), l = other.size();
	if(l > k) return 0;

	std::vector<infnum> q(k-l+2), r(k-l+2);
	q[0] = 0;
	
	//First l-1 digits of divident
	infnum temp = scaled_this;
	temp.longShiftRight(l-1);
	r[0] = temp;

	/* r[0] = scaled_this >> (l-1)*64; */
	/* std::cout << "a: " << scaled_this << ", b: " << other << '\n'; */
	/* std::cout << "r0: " << r[0] << ", q0: " << q[0] << '\n'; */

	for(int i = 1; i <= k-l+1; ++i) {
		infnum d = r[i-1] * BASE + scaled_this.data[k-1-(i+l-2)];

		//binary search biggest B so that B*other <= d
		infnum beta = 0;
		while(beta*other != d && d - other * beta >= other) {
			/* std::cout << "MULTIPLE (size: " << d.size() << " )START... "; */
			infnum jajco = biggest_multiple(d.size() * 64, d-beta*other, other);
			/* std::cout << "END\n"; */
			beta += jajco;
		}

		r[i] = d - beta * other;
		q[i-1].longShiftLeft(1);
		q[i] = q[i-1] + beta;

		/* std::cout << "di: " << d << ", beta: " << beta << '\n'; */
		/* std::cout << "i: " << i << ", ri: " << r[i] << ", qi: " << q[i] << '\n'; */
		/* std::cout << "\n"; */
	}
	infnum res = q[k-l+1];
	res.sign = sign;
	res.longShiftRight(2*SCALING_FACTOR);
	if(this->size() > 2) res.longShiftRight(this->size()-2);
	res.removeLeadingZeros();
	return res;
}

infnum infnum::operator-() {
	this->sign = !this->sign;
	return *this;
}

void infnum::operator+=(const infnum& other) {
	*this = *this + other;
}

void infnum::operator-=(const infnum& other) {
	*this = *this - other;
}

infnum infnum::operator>>(const int& count) const{
	if(count < 0) return *this << (-1 * count);
	infnum temp = *this;
	temp.longShiftRight(count / 64);
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

infnum infnum::operator<<(const int& count) const {
	if(count < 0) return *this >> (-1 * count);
	infnum temp = *this;
	temp.longShiftLeft(count / 64);
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

void infnum::operator>>=(const int& other) {
	*this = *this >> other;
}

void infnum::operator<<=(const int& other) {
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

infnum biggest_multiple(const u64 limit, const infnum query, const infnum mult) {
	int l = -1, r = limit+1, mid;

	while(r - l > 1) {
		mid = (l + r) / 2;
		infnum infmid = ((infnum)1 << mid) * mult;
		if(query < infmid) r = mid;
		else l = mid;
	}
	if(l == -1) return -1;
	/* std::cout << "L: " << l << ", JAJCO: " << ((infnum)1 << l) << '\n'; */
	return (infnum)1 << l;
}

infnum abs(infnum x) {
	x.sign = 0;
	return x;
}

}

std::string pinrt(infnum::infnum x) {
	std::string out;
	for(auto it = x.data.rbegin(); it != x.data.rend()-1; ++it) {
		out += std::to_string(*it);
		if(it != x.data.rend() - 2) out += ' ';
	}
	infnum::u64 mult = 1ULL << 63;
	infnum::u64 hihi = 0;
	for(int i = 0; i < 64; ++i) {
		if(x[0] & mult) hihi += mult;
		mult >>= 1;
	}

	std::string decimal = std::to_string((long double)hihi / (long double)UINT64_MAX);
	decimal.erase(decimal.begin());

	out += decimal;
	return out;
}

std::ostream& operator<<(std::ostream& o, infnum::infnum& n) {
	/* o << pinrt(n); */
	/* return o; */


	if(n.sign == 1) o << '-';

	infnum::infnum temp = infnum::floor(n);

	std::string integer;
	if(temp == 0) integer = "0";

	while(temp != 0) {
		infnum::infnum div10 = infnum::floor(temp / 10);
		infnum::infnum digit = temp - div10*10;
		/* std::cout << div10.size() << ' ' << digit.size() << ' ' << temp.size() << '\n'; */
		/* std::cout << "DEBUG: " << digit.data[1] << '\n'; */
		/* std::cout << pinrt(temp) << '\n'; */
		/* std::cout << temp.size() << '\n'; */
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

