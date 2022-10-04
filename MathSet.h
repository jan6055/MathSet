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
protected:
    Con m_container;
public:
    using self = MathSetHelper<T,Con, false>;
    MathSetHelper() = default;
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
    Con & get_container() {
        return m_container;
    }
    /// 并集，交集，差集运算\n
    /// 对于能预先分配空间的容器，预先分配空间以提高性能\n
    /// XXX(并交差)，调用XXX_impl使用标签分派\n
    /// true_type的版本接受可以预先分配空间的容器\n
    /// false_type的版本接受不可预先分配空间的容器\n
    /// 此操作发成在编译时期，无额外的运行时开销\n
private:
    Con & oneon_impl(const self & rhs, Con & ret, std::true_type) {
        ret.reserve(size()+rhs.size());
        return oneon_impl(rhs,ret, std::false_type());
    }

    Con & oneon_impl(const self & rhs, Con & ret, std::false_type) {
        std::insert_iterator<Con> it(ret,ret.end());
        std::set_union(begin(),end(),rhs.begin(),rhs.end(),it);
        return ret;
    }
public:
    /**
     * 对外提供的接口，来匹配调用oneon_impl
     * @param rhs
     * @return
     */
    virtual Con oneon(self & rhs) {
        Con ret;
        oneon_impl(rhs,ret,typename has_reserve<Con>::type());
        return ret;
    }


public:
    virtual Con intersection(self & rhs) {
        Con ret;
        intersection_impl(rhs,ret,typename has_reserve<Con>::type());
        return ret;
    }
private:
    Con & intersection_impl(const self & rhs,Con & ret, std::true_type) {
        ret.reserve(size()+rhs.size());
        return intersection_impl(rhs,ret,std::false_type());
    }
    Con & intersection_impl(const self & rhs, Con & ret,std::false_type) {
        std::insert_iterator<Con> it(ret,ret.end());
        std::set_intersection(begin(),end(),rhs.begin(),rhs.end(),it);
        return ret;
    }
private:
    Con & difference_impl(const self & rhs, Con & ret, std::false_type) {
        std::insert_iterator<Con> it(ret,ret.end());
        std::set_difference(begin(),end(),rhs.begin(),rhs.end(),it);
        return ret;
    }
    Con & difference_impl(self & rhs, Con & ret, std::true_type) {
        ret.reserve(size()+rhs.size());
        return difference_impl(rhs,ret,std::false_type());
    }
public:
    virtual Con difference(self & rhs) {
        Con ret;
        difference_impl(rhs,ret,typename has_reserve<Con>::type());
        return ret;
    }
public:
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
    Con oneon(super & rhs) override {
        std::sort(super::begin(),super::end());
        std::sort(rhs.begin(), rhs.end());
        return super::oneon(rhs);
    }

    Con intersection(super & rhs) override {
        std::sort(super::begin(),super::end());
        std::sort(rhs.begin(),rhs.end());
        return super::intersection(rhs);
    }

    Con difference(super & rhs) override {
        std::sort(super::begin(),super::end());
        std::sort(rhs.begin(),rhs.end());
        return super::difference(rhs);
    }
};

using std::make_unique;
template <typename T, typename Con = std::vector<T>>
class MathSet {
private:
    using base_type = MathSetHelper<T,Con, false>;
    std::unique_ptr<base_type> m_data;
    using object_type = MathSetHelper<T,Con,has_sort<Con>::value>;
public:
    using iterator = typename base_type::iterator;
    using const_iterator = typename base_type::const_iterator;
    MathSet(): m_data(make_unique<object_type>()) { }
    template<typename InputIter>
    MathSet(InputIter first, InputIter last): m_data(make_unique<object_type>(first,last)) { }

    static MathSet from_initializer_list(const std::initializer_list<T> & ls) {
        return MathSet(ls.begin(), ls.end());
    }
    iterator begin() {
        return m_data->begin();
    }
    iterator end() {
        return m_data->end();
    }
    const_iterator begin() const {
        return m_data->begin();
    }
    const_iterator end() const {
        return m_data->end();
    }

    auto size() const {
        return m_data->size();
    }

    MathSet oneon(MathSet & rhs) {
        //Con
        auto result =  m_data->oneon(*rhs.m_data);
        return MathSet(result.begin(),result.end());
    }

    MathSet intersection(MathSet & rhs) {
        auto result = m_data->intersection(*rhs.m_data);
        return MathSet(result.begin(),result.end());
    }

    MathSet difference(MathSet & rhs) {
        auto result = m_data->difference(*rhs.m_data);
        return MathSet(result.begin(),result.end());
    }

    void insert(const T & val) {
        m_data->insert(val);
    }

    bool operator==(const MathSet & rhs) const {
        if (size() != rhs.size()) {
            return false;
        }
        auto it = begin();
        for (const auto & val : rhs) {
            if(*it != val) {
                return false;
            }
        }
        return true;
    }
    void clear() {
        m_data->get_container().clear();
    }

    bool is_empty() const {
        return size() == 0;
    }

    /**
     * 移除迭代器指向的元素
     * @param pos
     * @return
     */
    iterator erase(const_iterator pos) {
        return m_data->get_container().erase(pos);
    }
    /**
     * 移除值维val的元素，如果没有此元素，返回end()
     * @param val
     * @return
     */
    iterator erase(const T & val) {
        auto it = std::find(begin(),end(),val);
        if (it != end()) {
            return erase(it);
        }
        return end();
    }

    /**
     * 判断是否包含另一个集合\n
     * 此操作不会对任何一个集合排序, 时间复杂度维O(N*M)
     * @param rhs
     * @return 如果包含返回true, 否则返回false
     */
    bool contain(const MathSet & rhs) {
        if(size() < rhs.size()) {
            return false;
        }
        for(const auto & val : rhs) {
            auto it = std::find(begin(),end(),val);
            if(it == end()) {
                return false;
            }
        }
        return true;
    }
    bool contain_with_order(const MathSet & rhs) {
        if(size() < rhs.size()) {
            return false;
        }
        auto first1 = begin();
        auto first2 = rhs.begin();
        while (first2 != rhs.end()) {
            if(*first1 == *first2) {
                ++first1;
                ++first2;
            } else if(*first1 > *first2){
                return false;
            } else {
                ++first1;
            }
        }
        return true;
    }
};

template<typename T, typename Con>
std::ostream & operator<<(std::ostream & os, const MathSet<T,Con> & mathSet) {
    if(mathSet.is_empty()) {
        os << "[]";
        return os;
    }
    os << "[";
    auto it = mathSet.begin();
    os << *it;
    ++it;
    for(; it != mathSet.end(); ++it) {
        os << ", " << *it;
    }
    os << "]";
    return os;
}
#endif //MATH_SET_MATHSET_H
