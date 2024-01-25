#include"infnum.h"

namespace infnum {

const u64 EXP_PRECISION = 10;
const u64 LNX_PRECISION = 30;
const infnum LN2({12786308645202655659ULL, 0ULL});

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

infnum exp_taylor(infnum x) {
	u64 div = 1;
	infnum res = 1;
	infnum xpower = 1;
	for(int i = 1; i <= EXP_PRECISION; ++i) {
		xpower *= x;
		div *= i;
		res += xpower / div;
	}
	return res;
}

infnum exp(infnum x) {
	infnum k = round(x / LN2);
	infnum r = x - k * LN2;
	return ((infnum)1 << k[1]) * exp_taylor(r); //2^k * e^r
}

infnum ln_newton(infnum x) {
	if(x <= 0) throw std::logic_error("Logarithm of non-positive numbers is undefined");
	infnum res = 1;
	for(int i = 1; i <= LNX_PRECISION; ++i) res = res + x / exp(res) - 1;
	return res;
}

infnum ln_taylor(infnum x) {
	infnum res = 0;
	infnum mult = 1;
	for(int i = 1; i <= LNX_PRECISION; ++i) {
		mult *= (x - 1);
		infnum term = mult / i;
		if(i % 2 == 0) term = -term;
		res += term;
	}
	return res;
}

infnum ln(infnum x) {
	if(x <= 0) throw std::logic_error("Logarithm of non-positive numbers is undefined");
	i64 n = mostSignificantBit(x);
	if(n > 0) return LN2 * n + ln_taylor(x >> n);
	else return ln_taylor(x);
}

i64 mostSignificantBit(infnum x) {
	i64 res = 0;
	u64 i;
	if(x.size() == 2 && x[1] == 0) { i = 0; }
	else {
		i = x.size() - 1;
		res = 64 * (x.size() - 1);
	}

	while(x[i] > 0) {
		res++;
		x[i] >>= 1;
	}
	return res-65;
}
}
