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
    void print() {}
    void print2() {}
    void print1() {}
};


int main()
{
    ATest atest;
    std::cout << atest.a << ' ' << atest.b << std::endl;
    return 0;
}