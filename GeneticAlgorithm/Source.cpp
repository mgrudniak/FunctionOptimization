#include"GA.h"
#include<vector>

using namespace std;

double function(vector<double>& x) {
	return (10 * (sin(0.2*(x[0] - 10)) + 2) * (sin(0.5*(x[0] - 10))) * (sin(0.5*(x[0] - 10))) / abs(x[0] - 10)) + 1;
}

int main() {
	GA ga;
	ga.set_range(-100, 100);
	ga.optimize(function);
	ga.show_best();

	cin.get();
	return 0;
}