#include"infnum.h"
#include<bitset>

namespace infnum {
void infnum::removeLeadingZeros() {
    while(*(this->data.end()-1) == 0 && this->size() > 2) this->data.pop_back();
}

u64 highBits(const u64& x) {
    return x >> 32ULL;
}

u64 lowBits(const u64& x) {
    return x & ((1ULL << 32) - 1);
}

infnum infnum::add(const infnum& other) {
    int n = std::max(size(), other.data.size());
    bool carry = 0;
    infnum a = *this, b = other;
    a.sign = 0; b.sign = 0;
    
    a.data.resize(n); b.data.resize(n);
    u64 t;
    
    for(int i = 0; i < n; ++i) {
        t = b[i] + carry;
        carry = UINT_LEAST64_MAX-a[i]-carry < b[i];
        a[i] += t;
    }
    if(carry) a.data.push_back(1);
    return a;
}

infnum infnum::subtract(const infnum& other) {
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
    return a;
}

void infnum::longShiftLeft(int count) {
    std::reverse(data.begin(), data.end());
    for(int i = 0; i++ < count;) data.push_back(0);
    std::reverse(data.begin(), data.end());
}

void infnum::longShiftRight(int count) {
    std::reverse(data.begin(), data.end());
    for(int i = 0; i++ < count && !data.empty();) data.pop_back();
    std::reverse(data.begin(), data.end());
    while(size() < 2) data.push_back(0);
}

bool infnum::operator==(const infnum& other) {
    if(sign != other.sign) return 0;
    if(size() != other.data.size()) return 0;
    for(int i = size()-1; i >= 0; --i) if(data[i] != other[i]) return 0;
    return 1;
}

bool infnum::operator>(const infnum& other) {
    if(sign != other.sign) return sign < other.sign;
    if(size() != other.data.size()) return data.size() > other.data.size();
    for(int i = size()-1; i >= 0; ++i) if(data[i] != other[i]) return data[i] > other[i];
    return 0;
}

bool infnum::operator>=(const infnum& other) {
    return *this > other || *this == other;
}

bool infnum::operator<(const infnum& other) {
    return !(*this >= other);
}

bool infnum::operator<=(const infnum& other) {
    return !(*this > other);
}

infnum infnum::operator+(const infnum& other) {
    if(this->sign != other.sign) {
        if(this->sign == 1) return -this->subtract(other);
        return this->subtract(other);
    }
    if(this->sign) return -this->add(other);
    return this->add(other);
}

infnum infnum::operator-(const infnum& other) {
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

bool infnum::hasLeadingZero() {
    return (this->size() == 2) && (this->data[1] == 0);
}

infnum infnum::operator*(const infnum& other) {
    infnum c, d, temp;
    u64 carry = 0;
    for(int i = 0; i < this->size(); ++i) {
        temp = 0;
        for(int j = 0; j < other.size(); ++j) {
            d = carry; d += mult_u64(this->data[i], other[j], carry);
            temp += d << (64 * j);
        }
        if(carry) {
            temp.data.push_back(carry);
            carry = 0;
        }
        c += temp << (64 * i);
    }
    return c >> (2 - this->hasLeadingZero() - other.size()) * 64;
}

infnum infnum::operator/(const infnum& other) {
    throw "Not implemented";
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

infnum infnum::operator>>(const int& count) {
    if(count < 0) return *this << (-1 * count);
    infnum temp = *this;
    temp.longShiftRight(count / 64);
    int rest = count % 64;
    u64 mask = ((1 << rest) - 1);
    
    for(int i = 0; i < (int)size(); ++i) {
        temp[i] >>= rest;
        u64 x = (i == (int)temp.size()-1 ? 0 : mask & temp[i+1]);
        temp[i] += x << (64 - rest);
    }

    temp.removeLeadingZeros();
    return temp;
}

infnum infnum::operator<<(const int& count) {
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
}

std::ostream& operator<<(std::ostream& o, const infnum::infnum& n) {
    if(n.sign == 1) o << '-';
    for(auto it = n.data.rbegin(); it != n.data.rend()-1; ++it) o << *it << (it == n.data.rend()-2 ? '.' : ' ');
    o << n[0];
    return o;
}


/*
 * TODO: 
 * make ostream overload output in decimal
 */

/* infnum y = -2; */
/* std::cout << y; */
/* y = 4; */

/* return 0; */
/* // Decimal expansion */
/* infnum x = 0.34522934172345; */
/* typedef u64 u64; */
/* u64 mult = 1ULL << 63; */
/* u64 hihi = 0; */
/* for(int i = 0; i < 64; ++i) { */
/*     if(x[0] & mult) hihi += mult; */
/*     mult >>= 1; */
/* } */
/* std::cout << hihi << '/' << UINT64_MAX; */
/* std::cout << '\n' << (long double)hihi / (long double)UINT64_MAX; */
