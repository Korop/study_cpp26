# C++26 Interview Study: `decltype((x))` and Value Categories

## 1. Why is `decltype((x))` `int&`?

Given:

```cpp
int x = 10;
```

Why are these different?

```cpp
decltype(x)    // int
decltype((x))  // int&
```

`decltype` has two relevant rules.

### Rule 1: unparenthesized name

For an unparenthesized id-expression or member access, `decltype` returns the declared type of the entity.

```cpp
#include <type_traits>

int x = 10;

static_assert(std::is_same_v<decltype(x), int>);
```

So `decltype(x)` is `int`.

### Rule 2: all other expressions

Otherwise, `decltype(expr)` depends on the expression value category:

```text
lvalue  -> T&
xvalue  -> T&&
prvalue -> T
```

Parenthesizing `x` changes which `decltype` rule applies:

```cpp
static_assert(std::is_same_v<decltype((x)), int&>);
```

`(x)` is still an lvalue expression, so `decltype((x))` is `int&`.

### Important

The parentheses do **not** themselves mean "reference".

They only prevent the special unparenthesized-name rule from applying.

---

## 2. What is a prvalue?

`prvalue` means **pure rvalue**.

A practical mental model:

> A prvalue computes a value rather than identifying an existing object.

Example:

```cpp
int x = 10;

static_assert(std::is_same_v<decltype(x + 1), int>);
```

`x + 1` computes a new integer value, so it is a prvalue.

Compare:

```cpp
#include <type_traits>
#include <utility>

int x = 10;

static_assert(std::is_same_v<decltype((x)), int&>);           // lvalue
static_assert(std::is_same_v<decltype(x + 1), int>);          // prvalue
static_assert(std::is_same_v<decltype(std::move(x)), int&&>); // xvalue
```

---

## 3. lvalue, xvalue, prvalue

### lvalue

An lvalue identifies an existing object.

```cpp
int x = 10;
x = 20;
```

`x` has identity and can be referred to again.

With `decltype`:

```cpp
static_assert(std::is_same_v<decltype((x)), int&>);
```

---

### xvalue

`xvalue` means **expiring value**.

It still identifies an existing object, but the object's resources may be reused.

```cpp
std::string text = "hello";

static_assert(
    std::is_same_v<decltype(std::move(text)), std::string&&>
);
```

`std::move(text)` still refers to the same `text` object.

`std::move` itself does not move anything. It converts the expression into an xvalue.

---

### prvalue

A prvalue computes or creates a value.

```cpp
static_assert(
    std::is_same_v<decltype(std::string{"hello"}), std::string>
);
```

Another example:

```cpp
std::string a = "hello";
std::string b = " world";

static_assert(std::is_same_v<decltype(a + b), std::string>);
```

`a + b` produces a new string value, so it is a prvalue.

---

## 4. `std::string` example

```cpp
#include <string>
#include <type_traits>
#include <utility>

std::string text = "hello";

static_assert(std::is_same_v<decltype((text)), std::string&>);
static_assert(std::is_same_v<decltype(std::move(text)), std::string&&>);
static_assert(std::is_same_v<decltype(std::string{"hello"}), std::string>);
static_assert(std::is_same_v<decltype(text + "!"), std::string>);
```

Interpretation:

```text
(text)              -> lvalue  -> std::string&
std::move(text)     -> xvalue  -> std::string&&
std::string{"hello"}-> prvalue -> std::string
text + "!"          -> prvalue -> std::string
```

---

## 5. What about a string literal?

This is an important interview trap:

```cpp
"text"
```

is **not** a `std::string`.

Its type is:

```text
const char[5]
```

and a string literal is an lvalue.

Therefore:

```cpp
#include <type_traits>

static_assert(
    std::is_same_v<decltype("text"), const char (&)[5]>
);
```

So:

```cpp
decltype("text") // const char (&)[5]
```

because the expression is an lvalue array of type `const char[5]`.

---

## 6. What does the `g` in `glvalue` mean?

`glvalue` means **generalized lvalue**.

A glvalue is an expression that has identity.

Its two categories are:

```text
glvalue
├── lvalue
└── xvalue
```

Both identify an object.

Example:

```cpp
std::string s = "hello";

s;            // lvalue
std::move(s); // xvalue
```

Both expressions refer to the same `s`.

---

## 7. Relationship between glvalue and rvalue

Modern C++ value categories can be visualized as:

```text
                 expression
                /          \
            glvalue        prvalue
           /      \
       lvalue     xvalue
```

Another useful grouping:

```text
glvalue = lvalue + xvalue
rvalue  = xvalue + prvalue
```

So `xvalue` belongs to both:

- it has identity, like an lvalue;
- it is an rvalue, so resources may be moved from it.

---

## 8. Compact mental model

```text
lvalue:
    "Which existing object?"
    decltype(expr) -> T&

xvalue:
    "Which existing object may I reuse/move from?"
    decltype(expr) -> T&&

prvalue:
    "What value?"
    decltype(expr) -> T
```

Example:

```cpp
std::string s = "Ivan";

static_assert(std::is_same_v<decltype((s)), std::string&>);
static_assert(std::is_same_v<decltype(std::move(s)), std::string&&>);
static_assert(std::is_same_v<decltype(std::string{"Ivan"}), std::string>);
```

---

## 9. `decltype` cheat sheet

```cpp
int x = 10;

decltype(x)             // int
decltype((x))           // int&
decltype(x + 1)         // int
decltype(std::move(x))  // int&&
```

The rules:

```text
decltype(name)
    -> declared type
    -> special rule for unparenthesized id-expression/member access

decltype(expression)
    -> lvalue  => T&
    -> xvalue  => T&&
    -> prvalue => T
```

---

## 10. C++17+ nuance

Avoid defining a prvalue simply as "a temporary object".

Since C++17, a prvalue often initializes its destination directly without first materializing a separate temporary object.

A better definition is:

> A prvalue is an expression whose evaluation computes a value or initializes an object, but does not identify an object in the way a glvalue does.

For interview discussion, the practical distinction is usually enough:

```text
glvalue -> identity
prvalue -> value computation
```

---

## Interview answer in ~30 seconds

`decltype` has a special rule for an unparenthesized name. If `int x`, then `decltype(x)` is simply `int`.

But `decltype((x))` does not use that special rule. It uses the general value-category rule. Since `(x)` is an lvalue, the result is `int&`.

For general expressions:

```text
lvalue  -> T&
xvalue  -> T&&
prvalue -> T
```

A prvalue is a pure rvalue that computes a value, such as `x + 1` or `std::string{"hello"}`. An xvalue is an expiring value that still identifies an existing object, such as `std::move(x)`. A glvalue, or generalized lvalue, is any expression with identity and includes both lvalues and xvalues.

---

## Core points to remember

- `decltype(x)` and `decltype((x))` may be different.
- Parentheses do not create a reference; they change which `decltype` rule applies.
- `prvalue` = pure rvalue.
- `xvalue` = expiring value.
- `glvalue` = generalized lvalue.
- `glvalue = lvalue + xvalue`.
- `rvalue = xvalue + prvalue`.
- `std::move` creates an xvalue expression but does not itself move anything.
- A string literal such as `"text"` is an lvalue of type `const char[N]`.
- `decltype("text")` is therefore `const char (&)[5]`.
