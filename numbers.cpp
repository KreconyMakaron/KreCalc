#include<cstdint>
#include<iostream>
#include<vector>
#include<cstring>
#include<algorithm>
#include<cmath>
#include<bitset> // for testing purposes

class number {
    typedef uint64_t u64;
    typedef __int128 i128;
    
private:
    template<typename T>
    void putBits(T t) {
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

    void removeLeadingZeros() {
        while(*(this->data.end()-1) == 0 && this->data.size() > 2) this->data.pop_back();
    }
    
    number add(const number& other) {
        int n = std::max(data.size(), other.data.size());
        bool carry = 0;
        number a = *this, b = other;
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
    number subtract(const number& other) {
        int n = std::max(data.size(), other.data.size());
        number a = *this, b = other;
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
    number longShiftRight(int count) {
        number temp = *this;
        std::reverse(temp.data.begin(), temp.data.end());
        for(int i = 0; i++ < count;) temp.data.push_back(0);
        std::reverse(temp.data.begin(), temp.data.end());
        return temp;
    }
    number longShiftLeft(int count) {
        number temp = *this;
        std::reverse(temp.data.begin(), temp.data.end());
        for(int i = 0; i++ < count && !temp.data.empty();) temp.data.pop_back();
        std::reverse(temp.data.begin(), temp.data.end());
        while(temp.data.size() < 2) temp.data.push_back(0);
        return temp;
    }
    void reset() {
        data = {0, 0};
        sign = 0;
    }
public:
    std::vector<u64> data = {0, 0}; //64 bits binary expansion
    bool sign = 0;

    template<typename T>
    number(T t){ *this = t; }
    number() {}
    
    template<typename T>
    void operator=(const T t) {
        reset();
        if(std::is_integral_v<T>) putBits(t);
        else if(std::is_floating_point_v<T>) putFloat(t);
    }

    friend std::ostream& operator<<(std::ostream& o, const number& n) {
        for(auto it = n.data.rbegin(); it != n.data.rend()-1; ++it) o << *it << (it == n.data.rend()-2 ? '.' : ' ');
        o << n.data[0];
        return o;
    }

    number operator-() {
        this->sign = !this->sign;
        return *this;
    }

    bool operator==(const number& other) {
        if(sign != other.sign) return 0;
        if(data.size() != other.data.size()) return 0;
        for(int i = data.size()-1; i >= 0; --i) if(data[i] != other.data[i]) return 0;
        return 1;
    }

    bool operator>(const number& other) {
        if(sign != other.sign) return sign < other.sign;
        if(data.size() != other.data.size()) return data.size() > other.data.size();
        for(int i = data.size()-1; i >= 0; ++i) if(data[i] != other.data[i]) return data[i] > other.data[i];
        return 0;
    }

    bool operator>=(const number& other) {
        return *this > other || *this == other;
    }
    
    bool operator<(const number& other) {
        return !(*this >= other);
    }

    bool operator<=(const number& other) {
        return !(*this > other);
    }

    number operator+(const number& other) {
        if(this->sign != other.sign) {
            if(this->sign == 1) return -this->subtract(other);
            return this->subtract(other);
        }
        if(this->sign) return -this->add(other);
        return this->add(other);
    }
    
    number operator-(const number& other) {
        if(this->sign != other.sign) {
            if(this->sign == 1) return -this->add(other);
            return this->add(other);
        }
        if(this->sign) return -this->subtract(other);
        return this->subtract(other);
    }

    void operator-=(const number& other) {
        *this = *this - other;
    }
    
    void operator+=(const number& other) {
        *this = *this + other;
    }

    number operator>>(const int& count) {
        if(count < 0) return *this << (-1 * count);
        number temp = *this;
        temp.longShiftRight(count / 64);
        int rest = count % 64;
        u64 mask = ((1 << rest) - 1);
        
        for(int i = 0; i < (int)data.size(); ++i) {
            temp.data[i] >>= rest;
            u64 x = (i == (int)temp.data.size()-1 ? 0 : mask & temp.data[i+1]);
            temp.data[i] += x << (64 - rest);
        }

        return temp;
    }

    void operator>>=(const int& other) {
        *this = *this >> other;
    }

    number operator<<(const int& count) {
        if(count < 0) return *this >> (-1 * count);
        number temp = *this;
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

    void operator<<=(const int& other) {
        *this = *this << other;
    }
};


#include<bit>

int main() {
    /*
     * TODO: 
     * push new sements when needed in bit shift left
     * remove leading zeros in bit shift right
     *      
     */

    number y = 1;
    y <<= 123;
    std::cout << y;
    std::cout << '\n' << y.data.size();


    return 0;
    number x = 0.34522934172345;
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
