#include"infnum.h"

namespace infnum {
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

infnum exp_taylor(infnum x, u64 n) {
	u64 div = 1;
	infnum res = 1;
	infnum xpower = 1;
	for(int i = 1; i <= n; ++i) {
		xpower *= x;
		div *= i;
		res += xpower / div;
	}
	return res;
}

//nth root of a using s many steps of newton's method 
/* infnum _root(infnum a, infnum n, u64 s) { */
/* 	infnum xk = 1; */
/* 	while(s--) { */
/* 		infnum xk1 = (n-1)*xk+a/ */
/* 	} */
/* } */

const infnum ln2({12786308645202655659ULL, 0ULL});
infnum exp(infnum x) {
	if(x.size() > 2) throw std::overflow_error("big number");
	infnum k = round(x / ln2);

	infnum r = x - k * ln2;
	return ((infnum)1 << k[1]) * exp_taylor(r, 10); //2^k * e^r
}

infnum ln(infnum x) {
	return x;
}
}
