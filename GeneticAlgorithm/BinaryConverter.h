#include<iostream>
#include<vector>

using namespace std;

struct Binary {
	vector<int> bits;
	int accuracy = 10;			// decimal places
	int last_int_bit{ int((bits.size() - 1) - accuracy) };

	Binary() :bits(18) {}

	int num_of_bits() { return bits.size(); }

	double to_double();
	void to_binary(double);
};
double Binary::to_double() {
	if (bits.size() > 0) {
		double result{ double(bits[1]) };
		for (int i = 1; i < last_int_bit; ++i) {
			if (bits[i] == 0 || bits[i] == 1) {
				result *= 2;
				result += bits[i + 1];
			}
		}

		for (int i = last_int_bit + 1; i < bits.size(); ++i) {
			if (bits[i] != 0) {
				int exponent{ -(i - last_int_bit) };
				double fract_part{ double(pow(2, exponent)) };
				result = result + fract_part;
			}
		}
		if (bits[0] == 0)
			return result;
		return -result;
	}
}
void Binary::to_binary(double x) {
	int exponent{ last_int_bit };
	int range_bound{ int(pow(2, exponent)) };
	if (x > range_bound || x < -range_bound) {
		cout << "\ncan't convert double to Binary; doesn't fit in the range\n";
		return;
	}

	if (bits.size() != 0) {
		for (int i = 0; i < bits.size(); ++i)
			bits[i] = 0;
	}

	if (x > 0)
		bits[0] = 0;
	else if (x < 0)
		bits[0] = 1;

	// integer part conversion
	int int_part{ int(floor(abs(x))) };

	bits[last_int_bit] = (int_part % 2);
	int quotient{ int(floor(int_part / 2)) };

	int i = last_int_bit - 1;
	while (quotient != 0) {
		bits[i] = (quotient % 2);
		quotient = floor(quotient / 2);
		--i;
	}

	// fractional part conversion
	double fract_part{ abs(x) - int_part };
	if (fract_part != 0) {
		double product = fract_part * 2;
		for (int i = last_int_bit + 1; i < bits.size(); ++i) {
			bits[i] = floor(product);
			fract_part = product - floor(product);
			product = fract_part * 2;
		}
	}
}