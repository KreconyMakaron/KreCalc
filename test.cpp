#include"infnum.h"
#include<iostream>

std::string print(infnum::infnum x) {
	std::string res = "";
	for(auto it = x.data.rbegin(); it != x.data.rend(); ++it) res += std::to_string(*it) + ' ';
	return res;
}

int main() {
	infnum::infnum x = "1212908347109234809213840918390481208947178902364091823740981237434.2341023";
	/* x = "343.234"; */
	std::cout << x;
}

