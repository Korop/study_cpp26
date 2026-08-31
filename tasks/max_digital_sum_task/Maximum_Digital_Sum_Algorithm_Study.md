# Algorithm Study: Maximum Digital Sum (Project Euler #56)

## Problem Statement

**Task:** Considering natural numbers of the form, $a^b$, where $a, b < 100$, what is the maximum digital sum?

### The Core Challenge: Integer Overflow
The main hurdle in this problem is integer overflow. Standard primitive data types in C++ and C# (such as `long` or `unsigned long long`) peak at 64 bits, which can only store up to approximately 19 digits. 
However, numbers like $99^{99}$ are nearly 200 digits long. 

To solve this, we must use **Arbitrary-Precision Arithmetic** (Big Integer math).

---

## Part 1: High-Level Language Solutions (C#)

In modern environments like .NET, handling arbitrary-precision arithmetic is a solved problem thanks to the built-in `BigInteger` struct. 

### C# Brute Force Solution
The C# solution is straightforward: generate every combination, calculate the power, convert to a string to iterate over digits, and track the maximum sum.

```csharp
using System;
using System.Numerics;

public class MaximumDigitalSum
{
    public static void Main()
    {
        int maxDigitalSum = 0;

        for (int a = 1; a < 100; a++)
        {
            for (int b = 1; b < 100; b++)
            {
                BigInteger power = BigInteger.Pow(a, b);
                int currentSum = GetDigitSum(power);
                
                if (currentSum > maxDigitalSum)
                {
                    maxDigitalSum = currentSum;
                }
            }
        }
        Console.WriteLine($"The maximum digital sum is: {maxDigitalSum}");
    }

    private static int GetDigitSum(BigInteger number)
    {
        int sum = 0;
        string digits = number.ToString();
        foreach (char digit in digits)
        {
            sum += digit - '0';
        }
        return sum;
    }
}
```

---

## Part 2: Custom Big Integer Multiplication (C++)

C++ does not have a standard BigInteger library. To solve this, we must represent giant numbers manually using arrays or vectors.

### Theory: The "Little-Endian" Array Trick
We treat an array as a sequence of single-digit boxes. To make multiplication easier, we store the number **backwards** so that the smallest place value is at index 0.

Example: Storing the number **1,234**
| Index 0 | Index 1 | Index 2 | Index 3 |
| :---: | :---: | :---: | :---: |
| **4** | **3** | **2** | **1** |
| ($10^0$) | ($10^1$) | ($10^2$) | ($10^3$) |

**Why store it backwards?**
When you multiply numbers by hand, the number grows to the left (units → tens → hundreds). If index 0 represents the units place, new digits created by carryovers are simply appended to higher array indices. This prevents the need to shift all existing array elements to the right whenever the number's length increases.

### Theory: How the Multiplication Algorithm Works

This algorithm mimics elementary school paper-and-pencil multiplication: **multiply digit-by-digit, write down the unit digit, and carry over the rest.**

#### Step-by-Step Example: Multiply 123 by 5

**Initial Array:** `digits = [3, 2, 1]`, `digitCount = 3`, `a = 5`

Cycle through array indices:

1.  **Index 0** (Value 3):
    *   $\text{Product} = (3 \times 5) + \text{carry}(0) = 15$
    *   $\text{New digit at Index 0} = 15 \pmod{10} = \mathbf{5}$
    *   $\text{New carry} = \lfloor 15 / 10 \rfloor = \mathbf{1}$

2.  **Index 1** (Value 2):
    *   $\text{Product} = (2 \times 5) + \text{carry}(1) = 11$
    *   $\text{New digit at Index 1} = 11 \pmod{10} = \mathbf{1}$
    *   $\text{New carry} = \lfloor 11 / 10 \rfloor = \mathbf{1}$

3.  **Index 2** (Value 1):
    *   $\text{Product} = (1 \times 5) + \text{carry}(1) = 6$
    *   $\text{New digit at Index 2} = 6 \pmod{10} = \mathbf{6}$
    *   $\text{New carry} = \lfloor 6 / 10 \rfloor = \mathbf{0}$

**Resulting Array:** `[5, 1, 6]` $\implies$ Reading backwards, the result is **615**.

### C++ Variant 1: Pure Brute Force with `std::vector`

```cpp
#include <iostream>
#include <vector>

int getDigitalSum(int a, int b) {
    std::vector<int> digits = {1}; // Start with 1

    // Multiply by 'a', 'b' times
    for (int i = 0; i < b; ++i) {
        int carry = 0;
        for (size_t j = 0; j < digits.size(); ++j) {
            int product = digits[j] * a + carry;
            digits[j] = product % 10;  // Store the units digit
            carry = product / 10;      // Carry over the rest
        }
        while (carry > 0) {
            digits.push_back(carry % 10);
            carry /= 10;
        }
    }

    int sum = 0;
    for (int digit : digits) {
        sum += digit;
    }
    return sum;
}

int main() {
    int maxDigitalSum = 0;
    for (int a = 1; a < 100; ++a) {
        for (int b = 1; b < 100; ++b) {
            int currentSum = getDigitalSum(a, b);
            if (currentSum > maxDigitalSum) maxDigitalSum = currentSum;
        }
    }
    std::cout << "Max Digital Sum: " << maxDigitalSum << std::endl;
    return 0;
}
```

---

## Part 3: Mathematical Optimization

Calculating $100 \times 100$ combinations using Big Integer arithmetic works quickly enough, but we can dramatically reduce the computational load using mathematics.

### Theory: Logarithmic Upper Bound
A number with $D$ digits can have a maximum theoretical digit sum of $9 \times D$ (if every digit is a 9). The exact number of digits in $a^b$ is calculated using base-10 logarithms:

$$ \text{Digits} = \lfloor b \cdot \log_{10}(a) \rfloor + 1 $$
$$ \text{Maximum Possible Sum} = 9 \times (\lfloor b \cdot \log_{10}(a) \rfloor + 1) $$

**The Pruning Strategy:**
1. Iterate backwards (99 down to 1). Large bases and exponents yield larger sums immediately.
2. For each $a$ and $b$, calculate the theoretical maximum sum.
3. If this theoretical max cannot beat our current `maxDigitalSum`, it is impossible for $a^b$ (and any smaller exponents) to be the answer. We can break the loop early.
4. **Skip Multiples of 10**: $90^b = 9^b \times 10^b$. The zeroes added by $10^b$ contribute nothing to the digital sum, so we can ignore any base ending in 0.

### C++ Variant 2: Mathematically Optimized (`std::vector`)

```cpp
#include <iostream>
#include <vector>
#include <cmath>

int getDigitalSum(int a, int b) {
    std::vector<int> digits = {1};
    for (int i = 0; i < b; ++i) {
        int carry = 0;
        for (size_t j = 0; j < digits.size(); ++j) {
            int product = digits[j] * a + carry;
            digits[j] = product % 10;
            carry = product / 10;
        }
        while (carry > 0) {
            digits.push_back(carry % 10);
            carry /= 10;
        }
    }
    int sum = 0;
    for (int digit : digits) sum += digit;
    return sum;
}

int main() {
    int maxDigitalSum = 0;

    // 1. Iterate downward
    for (int a = 99; a >= 1; --a) {
        // 2. Skip multiples of 10
        if (a % 10 == 0) continue;

        for (int b = 99; b >= 1; --b) {
            // 3. Mathematical Pruning
            int maxPossibleDigits = std::floor(b * std::log10(a)) + 1;
            int maxPossibleSum = maxPossibleDigits * 9;

            if (maxPossibleSum <= maxDigitalSum) {
                break; // Skip smaller 'b's as they yield even fewer digits
            }

            int currentSum = getDigitalSum(a, b);
            if (currentSum > maxDigitalSum) {
                maxDigitalSum = currentSum;
            }
        }
    }
    std::cout << "Max Digital Sum: " << maxDigitalSum << std::endl;
    return 0;
}
```
*Result: This reduces computations from 9,801 full operations to less than 300!*

---

## Part 4: Memory Optimization (Stack vs Heap)

`std::vector` allocates memory dynamically on the heap. We can make our algorithm noticeably faster by using a static-sized C-style array on the stack.

### Theory: Predicting Maximum Size
To use a fixed-size array, we must know the absolute maximum length our number could reach. 
The largest equation we will process is $99^{99}$.
$$ 99^{99} < 100^{99} = (10^2)^{99} = 10^{198} $$
$10^{198}$ is a 1 with 198 zeros (199 digits total). Therefore, a fixed array size of **200** guarantees we will never encounter an index out-of-bounds error.

### C++ Variant 3: Fixed-Size Array Allocation (Max Performance)

```cpp
#include <iostream>

int getDigitalSum(int a, int b) {
    int digits[200] = {0}; // Fixed-size array initialized to zero
    digits[0] = 1;         // Start with the value 1 (10^0)
    int digitCount = 1;    // Tracks the number of active digits

    for (int i = 0; i < b; ++i) {
        int carry = 0;
        for (int j = 0; j < digitCount; ++j) {
            int product = digits[j] * a + carry;
            digits[j] = product % 10;
            carry = product / 10;
        }
        while (carry > 0) {
            digits[digitCount] = carry % 10;
            carry /= 10;
            digitCount++;
        }
    }

    int sum = 0;
    for (int i = 0; i < digitCount; ++i) {
        sum += digits[i];
    }
    return sum;
}

int main() {
    int maxDigitalSum = 0;
    for (int a = 1; a < 100; ++a) {
        for (int b = 1; b < 100; ++b) {
            int currentSum = getDigitalSum(a, b);
            if (currentSum > maxDigitalSum) {
                maxDigitalSum = currentSum;
            }
        }
    }
    std::cout << "Max Digital Sum: " << maxDigitalSum << std::endl;
    return 0;
}
```

### Trade-Offs Summary
| Metric | `std::vector<int>` | `int arr[200]` |
| :--- | :--- | :--- |
| **Memory Allocation** | Dynamic (Heap) | Static (Stack) |
| **Speed** | Marginally slower | **Faster** |
| **Safety** | Expands automatically | Can overflow if math prediction is wrong |
