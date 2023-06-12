/*
 * Test about factory mode.
 */
#include <iostream>

class ATest {
public:
    float a;
    int b;
    ATest() : a(1.0), b(2) {}
};

class MoreTest {
    int a;
    double b;
    MoreTest() : a(1), b(2.2) {}
    void print() { std::cout << "this is a modify print" << std::endl; }
    void print2() {}
    void print3() {}
    void print1() {}
    void add_func() {}
    void add_func1() { std::cout << "This is a test about snap." << std::endl; }
};


int main()
{
    ATest atest;
    std::cout << atest.a << ' ' << atest.b << std::endl;
    return 0;
}