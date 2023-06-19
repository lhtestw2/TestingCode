/*
 * Test about factory mode.
 */
#include <iostream>
#include <algorithm>
#include <string>
#include <vector>

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


int getEditDistance(std::string first, std::string second) {
    int m = first.length();
    int n = second.length();
    std::vector<std::vector<size_t>> record(m + 1, std::vector<size_t>(n + 1, 0));
    for (int i = 0; i <= m; ++i) {
        record[i][0] = i;
    }
    for (int j = 0; j <= n; ++j) {
        record[0][j] = j;
    }
    for (int i = 1; i <= m; ++i) {
        for (int j = 1; j <= n; ++j) {
            size_t weight = first[i - 1] == second[j - 1] ? 0 : 1;
            record[i][j] = std::min(std::min(record[i - 1][j] + 1, record[i][j - 1] + 1), 
            record[i - 1][j - 1] + weight);
        }
    }
    return record[m][n];
}

double StringSimilarity(std::string first, std::string second) {
    double max_len = std::max(first.length(), second.length());
    if (max_len > 0) {
        return (max_len - getEditDistance(first, second)) / max_len;
    }
    return 1;
}


int main()
{
    // ATest atest;
    // std::cout << atest.a << ' ' << atest.b << std::endl;
    // std::cout << 2 << std::endl;
    // BTest btest(1.2);
    // std::cout << btest.a << ' ' << btest.b << ' ' << btest.c << std::endl;
    std::cout << StringSimilarity("test", "log") << std::endl;
    return 0;
}