//
// Created by wangq on 2022/10/2.
//

#ifndef MATH_SET_MATHSET_H
#define MATH_SET_MATHSET_H
#include <vector>
#include <algorithm>
#include <type_traits>
#include <iterator>
#include <memory>
using std::declval;
using std::vector;
using std::void_t;

/**
 * 推导T容器是否能应用与std::sort && 此容器提供的迭代器是否是随机迭代器
 * 主模板为无的情况
 * @tparam T
 */
template <typename T, typename = void>
struct has_sort:std::false_type {};

/**
 * 偏特化为有的情况
 * @tparam T
 */
template <typename T>
struct has_sort<T, void_t<decltype(std::sort(declval<T>().begin(), declval<T>().end()))
        ,std::enable_if_t<std::is_same_v<typename T::iterator::iterator_category,std::random_access_iterator_tag>>>>: std::true_type {};

/**
 * 推导T容器是否有reserve成员函数，用于容器预先分配空间
 * 主模板为无的情况
 * @tparam T
 */
template<typename T, typename = void>
struct has_reserve : std::false_type {};

template<typename T>
struct has_reserve<T, void_t<decltype(declval<T>().reserve(0))>> : std::true_type {};

/**
 * 无法应用sort的容器，但是确保其插入是有序的
 * @tparam T
 * @tparam Con
 * @tparam p
 */
template<typename T, typename Con , bool p = false>
class MathSetHelper {
private:

protected:
    Con m_container;
public:
    using self = MathSetHelper<T,Con, false>;
    MathSetHelper() = default;
    explicit MathSetHelper(size_t cap): m_container(cap) { }

    template<typename InputIter>
    MathSetHelper(InputIter first, InputIter last): m_container(first,last) { }
    using iterator = typename Con::iterator;
    using const_iterator = typename Con::const_iterator;
    virtual ~MathSetHelper() = default;
    auto size() const {
        return m_container.size();
    }
    void insert(const T & val) {
        m_container.insert(end(),val);
    }
    iterator begin() {
        return m_container.begin();
    }
    iterator end() {
        return m_container.end();
    }
    const_iterator begin() const {
        return m_container.begin();
    }
    const_iterator end() const {
        return m_container.end();
    }

    /// 并集，交集，差集运算\n
    /// 对于能预先分配空间的容器，预先分配空间以提高性能\n
    /// XXX(并交差)，调用XXX_impl使用标签分派\n
    /// true_type的版本接受可以预先分配空间的容器\n
    /// false_type的版本接受不可预先分配空间的容器\n
    /// 此操作发成在编译时期，无额外的运行时开销\n
private:
    self oneon_impl(self & rhs, self & ret, std::true_type) {
        ret.m_container.reserve(size()+rhs.size());
        return oneon_impl(rhs,ret, std::false_type());
    }

    self oneon_impl(self & rhs, self & ret, std::false_type) {
        std::back_insert_iterator<Con> it(ret.m_container);
        std::set_union(begin(),end(),rhs.begin(),rhs.end(),it);
        return ret;
    }
public:
    /**
     * 对外提供的接口，来匹配调用oneon_impl
     * @param rhs
     * @return
     */
    virtual self oneon(self & rhs) {
        self ret;
        return oneon_impl(rhs,ret,typename has_reserve<Con>::type());
    }


public:
    virtual self intersection(self & rhs) {
        self ret;
        return intersection_impl(rhs,ret,typename has_reserve<Con>::type());
    }
private:
    self intersection_impl(self & rhs,self & ret, std::true_type) {
        ret.m_container.reserve(size()+rhs.size());
        return intersection_impl(rhs,ret,std::false_type());
    }
    self intersection_impl(self & rhs, self & ret,std::false_type) {
        std::back_insert_iterator<Con> it(ret.m_container);
        std::set_intersection(begin(),end(),rhs.begin(),rhs.end(),it);
        return ret;
    }
private:
    self difference_impl(self & rhs, self & ret, std::true_type) {
        std::back_insert_iterator<Con> it(ret.m_container);
        std::set_difference(begin(),end(),rhs.begin(),rhs.end(),it);
        return ret;
    }
    self difference_impl(self & rhs, self & ret, std::false_type) {
        ret.m_container.reserve(size()+rhs.size());
        return difference_impl(rhs,ret,std::true_type());
    }
public:
    virtual self difference(self & rhs) {
        self ret;
        return difference_impl(rhs,ret,has_reserve<Con>::type);
    }
};

/**
 * 插入是无序的，但是其可以应用于sort函数
 * public 继承MatchSetHelper<_,_,false>
 * @tparam T
 * @tparam Con
 */
template<typename T, typename Con>
class MathSetHelper<T,Con, true> : public MathSetHelper<T,Con, false>{
private:

public:
    using super = typename MathSetHelper<T,Con, false>::self;

    MathSetHelper() = default;
    template<typename InputIter>
    MathSetHelper(InputIter first, InputIter last): super(first,last) { }
    ~MathSetHelper() override = default;

    using typename super::self;
    super oneon(super & rhs) override {
        std::sort(super::begin(),super::end());
        std::sort(rhs.begin(), rhs.end());
        return super::oneon(rhs);
    }

    super intersection(super & rhs) override {
        std::sort(super::begin(),super::end());
        std::sort(rhs.begin(),rhs.end());
        return super::intersection(rhs);
    }

    super difference(super & rhs) override {
        std::sort(super::begin(),super::end());
        std::sort(rhs.begin(),rhs.end());
        return super::difference(rhs);
    }
};

#endif //MATH_SET_MATHSET_H
