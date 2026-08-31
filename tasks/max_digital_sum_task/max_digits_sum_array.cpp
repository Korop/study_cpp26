#include <iostream>

// Function using a raw C-style array instead of std::vector
int getDigitalSum(int a, int b) {
    int digits[200] = {0}; // Fixed-size array initialized to zero
    digits[0] = 1;         // Start with the value 1 (10^0)
    int digitCount = 1;    // Tracks the number of active digits in the array

    // Multiply 'a', 'b' times
    for (int i = 0; i < b; ++i) {
        int carry = 0;

        // Multiply existing digits
        for (int j = 0; j < digitCount; ++j) {
            int product = digits[j] * a + carry;
            digits[j] = product % 10; // Keep the units digit
            carry = product / 10;     // Carry over the tens digit
        }

        // Append remaining carry as new digits
        while (carry > 0) {
            digits[digitCount] = carry % 10;
            carry /= 10;
            digitCount++; // Number expands into higher array indices
        }
    }

    // Calculate sum of digits
    int sum = 0;
    for (int i = 0; i < digitCount; ++i) {
        sum += digits[i];
    }

    return sum;
}

int main() {
    int a = 99, b = 95;
    std::cout << "Digital sum of " << a << "^" << b << " is: " << getDigitalSum(a, b) << std::endl;
    return 0;
}
