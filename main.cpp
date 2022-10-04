#include <iostream>
#include <set>
#include <list>
#include "MathSet.h"
#include <type_traits>
#include <forward_list>
using namespace std;

template <typename T>
void display_con(const T & con) {
    for (const auto & val : con) {
        cout << val << ' ';
    }
    cout << endl;
}
void test_MathSetHelper_true() {
    int nums1[] = {9,7,8,1,2,3};
    int nums2[] = {4,5,1,2,};

    //构造函数
    MathSetHelper<int,vector<int>, true> s;
    MathSetHelper<int,vector<int>, true> s1 (begin(nums1), end(nums1));
    MathSetHelper<int,vector<int>, true> s2 (begin(nums2), end(nums2));

    auto s3(s2);

    display_con(s1);
    display_con(s2);
    display_con(s3);

    auto s4 = s1.oneon(s2);
    cout << "union: " << endl;
    display_con(s4);

    auto s5 = s1.intersection(s2);
    cout << "intersection: " << endl;
    display_con(s5);

    //insert
    cout << "test insert: " << endl;
    s.insert(1);
    s.insert(5);
    s.insert(2);
    display_con(s);

    //test size
    cout << "test size: " << endl;
    cout << s.size() << endl;
}

void test_MathSet() {
    int nums1[] = {9,7,8,1,2,3};
    int nums2[] = {4,5,1,2,};
    MathSet<int> s;
    MathSet<int> s1(begin(nums1), end(nums1));
    MathSet<int> s2(begin(nums2), end(nums2));

    display_con(s1);
    display_con(s2);

    cout << "union: " << endl;
    auto s3 = s1.oneon(s2);
    display_con(s3);
    cout << "intersection: " << endl;
    auto s4 = s1.intersection(s2);
    display_con(s4);
    cout << "difference: " << endl;
    auto s5 = s1.difference(s2);
    display_con(s5);

    cout << "test insert: " << endl;
    s.insert(10);
    s.insert(20);
    s.insert(50);
    display_con(s);
}
void test_MathSet1() {
    int nums1[] = {9,7,8,1,2,3};
    int nums2[] = {4,5,1,2,};
    MathSet<int,set<int>> s;
    MathSet<int,set<int>> s1(begin(nums1), end(nums1));
    MathSet<int,set<int>> s2(begin(nums2), end(nums2));

    display_con(s1);
    display_con(s2);

    cout << "union: " << endl;
    auto s3 = s1.oneon(s2);
    display_con(s3);
    cout << "intersection: " << endl;
    auto s4 = s1.intersection(s2);
    display_con(s4);
    cout << "difference: " << endl;
    auto s5 = s1.difference(s2);
    display_con(s5);

    cout << "test insert: " << endl;
    s.insert(10);
    s.insert(20);
    s.insert(50);
    display_con(s);
}

void test_MathSet2() {
    MathSet<int> s1;
    MathSet<int> s2;
    cout << "test ==: " << endl;
    cout << (s1 == s2) << endl;
    s1.insert(10);
    cout << (s1 == s2) << endl;

    cout << "test clear: " << endl;
    s1.clear();

    cout << s1 << endl;

    cout << "now s1 is empty :" << endl;
    cout << s1.is_empty() << endl;

}

void test_MathSet3() {
    MathSet<int> s1 = MathSet<int>::from_initializer_list({1,2,3,4,5,6});
    cout << s1 << endl;
    s1.erase(3);
    cout << "erase 3: " << endl;
    cout << s1 << endl;
    cout << "erase begin(): " << endl;
    s1.erase(s1.begin());
    cout << s1 << endl;
}

void test_MathSet4() {
    auto s1 = MathSet<int>::from_initializer_list({7,1,5,2,4});
    auto s2 = MathSet<int>::from_initializer_list({2,3,4});
    auto _s2 = MathSet<int>::from_initializer_list({7,4,1});

    cout << "test contain: " << endl;
    cout << s1.contain(s2) << endl;
    cout << s1.contain(_s2) << endl;
    auto s3 = MathSet<int>::from_initializer_list({1,2,3,4,5});
    auto s4 = MathSet<int>::from_initializer_list({1,3,5});
    cout << s3.contain_with_order(s4);
}

void test_MathSet5() {
    MathSet<int> s1;
//    MathSet s2 (s1);
}

void test_has_reserve() {
    cout.setf(ios_base::boolalpha);
    cout << "forward_list not have push back: " << endl;
    cout << has_reserve<forward_list<int>>::value << endl;
    cout << "vector have push back: " << endl;
    cout << has_reserve<vector<int>>::value << endl;
    cout << "list not have push back: " << endl;
    cout << has_reserve<list<int>>::value << endl;
}

int main() {
    cout << "--------------------------------------" << endl;
    cout << "test MathSetHelper_true: " << endl;
    test_MathSetHelper_true();
    cout << "--------------------------------------" << endl;
    test_has_reserve();
    cout << "--------------------------------------" << endl;
    cout << "test MathSet: " << endl;
    test_MathSet();
    cout << "--------------------------------------" << endl;
    cout << "test MathSet1: " << endl;
    test_MathSet1();
    cout << "--------------------------------------" << endl;
    cout << "test MathSet2: " << endl;
    test_MathSet2();
    cout << "--------------------------------------" << endl;
    cout << "test MathSet3: " << endl;
    test_MathSet3();
    cout << "--------------------------------------" << endl;
    cout << "test MathSet4: " << endl;
    test_MathSet4();

    return 0;
}
