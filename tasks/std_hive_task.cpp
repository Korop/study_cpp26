#include <iostream>
#include <memory>

struct B;

struct A
{
    std::shared_ptr<B> b;

    ~A()
    {
        std::cout << "~A() called\n";
    }
};

struct B
{
    std::shared_ptr<A> a;

    ~B()
    {
        std::cout << "~B() called\n";
    }
};

int main()
{
    std::cout << "Start RAII\n";

    {
        auto ptrA = std::make_shared<A>();
        auto ptrB = std::make_shared<B>();

        std::cout << "\nAfter creation:\n";
        std::cout << "a use_count = " << ptrA.use_count() << '\n';
        std::cout << "b use_count = " << ptrB.use_count() << '\n';

        // A owns B
        ptrA->b = ptrB;
        

        // B owns A
        ptrB->a = ptrA;
        


        std::cout << "\nAfter creating cycle:\n";
        std::cout << "a use_count = " << ptrA.use_count() << '\n';
        std::cout << "b use_count = " << ptrB.use_count() << '\n';

    } // local shared_ptr a and b are destroyed here

    std::cout << "\nScope finished\n";
    std::cout << "Stop RAII\n";
}