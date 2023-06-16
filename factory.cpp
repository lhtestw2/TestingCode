/*
 * Test about factory mode.
 */
#include <iostream>

#define TEST 1

class ATest {
public:
    float a;
    int b;
    ATest() : a(1.0), b(2) {}
};

class BTest : virtual public ATest {
public:
    float c;
    BTest(float c) : ATest(), c(c) {}
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
    std::cout << 2 << std::endl;
    BTest btest(1.2);
    std::cout << btest.a << ' ' << btest.b << ' ' << btest.c << std::endl;
    return 0;
}