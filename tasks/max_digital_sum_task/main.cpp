#include <iostream>
#include <vector>
#include <algorithm>

// Function to calculate a^b and return the sum of its digits
int getDigitalSum(int a, int b) {
    std::vector<int> digits;
    digits.push_back(1); // Start with 1

    // Multiply the array by 'a', 'b' times
    for (int i = 0; i < b; ++i) {
        int carry = 0;
        for (size_t j = 0; j < digits.size(); ++j) {
            int product = digits[j] * a + carry;
            digits[j] = product % 10; // Keep the last digit
            carry = product / 10;     // Carry over the rest
        }

        // If there's any carry left over, add it as new digits
        while (carry > 0) {
            digits.push_back(carry % 10);
            carry /= 10;
        }
    }

    // Sum the resulting digits
    int sum = 0;
    for (int digit : digits) {
        sum += digit;
    }

    return sum;
}

int main() {
    int maxDigitalSum = 0;

    // Brute force all combinations where a, b < 100
    // Optimization: We can start higher (e.g., 90) since lower numbers won't yield the max sum.
    for (int a = 1; a < 100; ++a) {
        for (int b = 1; b < 100; ++b) {
            int currentSum = getDigitalSum(a, b);
            if (currentSum > maxDigitalSum) {
                maxDigitalSum = currentSum;
            }
        }
    }

    std::cout << "The maximum digital sum is: " << maxDigitalSum << std::endl;

    return 0;
}
