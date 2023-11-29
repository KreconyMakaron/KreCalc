#include"infnum.h"
#include <strings.h>

namespace infnum {
    
void infnum::removeLeadingZeros() {
    while(*(this->data.end()-1) == 0 && this->size() > 2) this->data.pop_back();
}

infnum infnum::add(const infnum& other) {
    int n = std::max(size(), other.size());
    bool carry = 0;
    infnum a = *this, b = other;
    a.sign = 0; b.sign = 0;
    
    a.data.resize(n); b.data.resize(n);
    u64 t;
    
    for(int i = 0; i < n; ++i) {
        t = b.data[i] + carry;
        carry = U64_MAX-a.data[i]-carry < b.data[i];
        a.data[i] += t;
    }
    if(carry) a.data.push_back(1);
    return a;
}

infnum infnum::subtract(const infnum& other) {
    int n = std::max(size(), other.size());
    infnum a = *this, b = other;
    a.sign = 0; b.sign = 0;
    if(b > a) {
        std::swap(a, b);
        a.sign = 1;
    }
    a.data.resize(n); b.data.resize(n);
    
    for(int i = 0; i < n; ++i) {
        if(a.data[i] < b.data[i]) {
            a.data[i] += U64_MAX;
            int k = i+1;
            while(k < n) {
                if(a.data[k]-- > 0) break;
                k++;
            }
        }
        a.data[i] -= b.data[i];
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
    if(size() != other.size()) return 0;
    for(int i = size()-1; i >= 0; --i) if(data[i] != other.data[i]) return 0;
    return 1;
}

bool infnum::operator>(const infnum& other) {
    if(sign != other.sign) return sign < other.sign;
    if(size() != other.size()) return size() > other.size();
    for(int i = size()-1; i >= 0; ++i) if(data[i] != other.data[i]) return data[i] > other.data[i];
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

// This is a bit hacky as to avoid using __int128,
infnum infnum::operator*(const infnum& other) {
    infnum a = *this, b = other, result;
    u64 a1, a2, b1, b2, c;
    u64 mult, carry;

    int n = std::max(a.size(), b.size());
    a.data.resize(n); b.data.resize(n); result.data.resize(n);

    for(int i = 0; i < n; ++i) {
        a1 = a[i] & ((1ULL << 32) - 1); // Lower 32 Bits
        a2 = a[i] - a1;                 // Upper 32 Bits
        b1 = b[i] & ((1ULL << 32) - 1);
        b2 = b[i] - b1;

        mult = a2 * b2;
        result[i] = carry + a1 * b1;
        result += a1 * b1;
        
        carry = mult >> 32;
    }
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
        temp.data[i] >>= rest;
        u64 x = (i == (int)temp.size()-1 ? 0 : mask & temp.data[i+1]);
        temp.data[i] += x << (64 - rest);
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
        temp.data[i] <<= rest;
        u64 x = (i == (int)temp.size()-1 ? 0 : mask & temp.data[i+1]);
        temp.data[i] += x >> (64 - rest);
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

u64& infnum::operator[](const std::size_t& index) {
    if(index > this->size()-1) throw std::out_of_range((std::string)"Index out of range: index = " + std::to_string(index) + ", size = " + std::to_string(this->size()));
    else return this->data[index];
}

std::size_t infnum::size() const {
    return this->data.size();
}


std::ostream& operator<<(std::ostream& o, const infnum& n) {
    if(n.sign == 1) o << '-';
    for(auto it = n.data.rbegin(); it != n.data.rend()-1; ++it) o << *it << (it == n.data.rend()-2 ? '.' : ' ');
    o << n.data[0];
    return o;
}
}



/*
 * TODO: 
 * add multiplication
 * make ostream overload output in decimal
 */

/* infnum y = -2; */
/* std::cout << y; */
/* y = 4; */

/* return 0; */
/* // Decimal expansion */
/* infnum x = 0.34522934172345; */
/* typedef uint64_t u64; */
/* u64 mult = 1ULL << 63; */
/* u64 hihi = 0; */
/* for(int i = 0; i < 64; ++i) { */
/*     if(x.data[0] & mult) hihi += mult; */
/*     mult >>= 1; */
/* } */
/* std::cout << hihi << '/' << UINT64_MAX; */
/* std::cout << '\n' << (long double)hihi / (long double)UINT64_MAX; */
