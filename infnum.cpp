#include"infnum.h"
#include<bitset> // for testing purposes

template<typename T>
void infnum::putBits(T t) {
    if(t < 0) {
        sign = 1;
        t *= -1;
    }
    std::memcpy(&data[1], &t, sizeof(t));
}

template<typename T>
void infnum::putFloat(T t) {
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

void infnum::removeLeadingZeros() {
    while(*(this->data.end()-1) == 0 && this->data.size() > 2) this->data.pop_back();
}

infnum infnum::add(const infnum& other) {
    int n = std::max(data.size(), other.data.size());
    bool carry = 0;
    infnum a = *this, b = other;
    a.sign = 0; b.sign = 0;
    
    a.data.resize(n); b.data.resize(n);
    u64 t;
    
    for(int i = 0; i < n; ++i) {
        t = b.data[i] + carry;
        carry = UINT_LEAST64_MAX-a.data[i]-carry < b.data[i];
        a.data[i] += t;
    }
    if(carry) a.data.push_back(1);
    return a;
}

infnum infnum::subtract(const infnum& other) {
    int n = std::max(data.size(), other.data.size());
    infnum a = *this, b = other;
    a.sign = 0; b.sign = 0;
    if(b > a) {
        std::swap(a, b);
        a.sign = 1;
    }
    a.data.resize(n); b.data.resize(n);
    
    for(int i = 0; i < n; ++i) {
        if(a.data[i] < b.data[i]) {
            a.data[i] += UINT_LEAST64_MAX;
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
    while(data.size() < 2) data.push_back(0);
}

template<typename T>
void infnum::operator=(const T t) {
    std::cout << sign;
    *this = infnum();
    std::cout << sign;
    if(std::is_integral_v<T>) putBits(t);
    else if(std::is_floating_point_v<T>) putFloat(t);
}

infnum infnum::operator-() {
    this->sign = !this->sign;
    return *this;
}

bool infnum::operator==(const infnum& other) {
    if(sign != other.sign) return 0;
    if(data.size() != other.data.size()) return 0;
    for(int i = data.size()-1; i >= 0; --i) if(data[i] != other.data[i]) return 0;
    return 1;
}

bool infnum::operator>(const infnum& other) {
    if(sign != other.sign) return sign < other.sign;
    if(data.size() != other.data.size()) return data.size() > other.data.size();
    for(int i = data.size()-1; i >= 0; ++i) if(data[i] != other.data[i]) return data[i] > other.data[i];
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

void infnum::operator-=(const infnum& other) {
    *this = *this - other;
}

void infnum::operator+=(const infnum& other) {
    *this = *this + other;
}

infnum infnum::operator>>(const int& count) {
    if(count < 0) return *this << (-1 * count);
    infnum temp = *this;
    temp.longShiftRight(count / 64);
    int rest = count % 64;
    u64 mask = ((1 << rest) - 1);
    
    for(int i = 0; i < (int)data.size(); ++i) {
        temp.data[i] >>= rest;
        u64 x = (i == (int)temp.data.size()-1 ? 0 : mask & temp.data[i+1]);
        temp.data[i] += x << (64 - rest);
    }

    temp.removeLeadingZeros();
    return temp;
}

void infnum::operator>>=(const int& other) {
    *this = *this >> other;
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
    for(int i = 0; i < (int)temp.data.size(); ++i) {
        temp.data[i] <<= rest;
        u64 x = (i == (int)temp.data.size()-1 ? 0 : mask & temp.data[i+1]);
        temp.data[i] += x >> (64 - rest);
    }
    std::reverse(temp.data.begin(), temp.data.end());

    return temp;
}

void infnum::operator<<=(const int& other) {
    *this = *this << other;
}

std::ostream& operator<<(std::ostream& o, const infnum& n) {
    if(n.sign == 1) o << '-';
    for(auto it = n.data.rbegin(); it != n.data.rend()-1; ++it) o << *it << (it == n.data.rend()-2 ? '.' : ' ');
    o << n.data[0];
    return o;
}

int main() {
    /*
     * TODO: 
     * make ostream overload output in decimal
     * add multiplication
     */

    infnum y = -2;
    std::cout << y;
    y = 4;

    return 0;
    // Decimal expansion
    infnum x = 0.34522934172345;
    typedef uint64_t u64;
    u64 mult = 1ULL << 63;
    u64 hihi = 0;
    for(int i = 0; i < 64; ++i) {
        if(x.data[0] & mult) hihi += mult;
        mult >>= 1;
    }
    std::cout << hihi << '/' << UINT64_MAX;
    std::cout << '\n' << (long double)hihi / (long double)UINT64_MAX;
}

