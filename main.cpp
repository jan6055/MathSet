#include <iostream>
#include <set>
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
//    MathSet<int> s;
//    MathSet<int> s1(begin(nums1), end(nums1));
//    MathSet<int> s2(begin(nums2), end(nums2));


}
void test_has_reserve() {
    cout.setf(ios_base::boolalpha);
    cout << "forward_list not have push back: " << endl;
    cout << has_reserve<forward_list<int>>::value << endl;
    cout << "vector have push back: " << endl;
    cout << has_reserve<vector<int>>::value << endl;
}
int main() {
    test_MathSetHelper_true();
    test_has_reserve();
    test_MathSet();
    return 0;
}
