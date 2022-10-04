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
 * @tparam T    迭代的元素的类型
 * @tparam Con  底层容器
 * @tparam p    p=false 无法应用与sort的容器，确保其插入是有有序的
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

    //右值版本的底层容器构造
    explicit MathSetHelper(Con && con): m_container(std::move(con)) { }

    //iterator and const_iterator
    using iterator = typename Con::iterator;
    using const_iterator = typename Con::const_iterator;

    virtual ~MathSetHelper() = default;

    /**
     * 返回集合的大小，确保底层容器有size()方法
     * @return
     */
    auto size() const {
        return m_container.size();
    }

    // 确保底层容器有insert方法
    // 为什么不用push_back?
    // STL中的容器不全提供push_back操作，但是全提供insert操作
    // 使用insert而不是push_back可使得泛用性更强
    // 但是有可能会牺牲性能
    void insert(const T & val) {
        m_container.insert(end(),val);
    }

    //确保底层容器有首，尾迭代器
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

    //返回底层集合的引用。
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
 * public 继承MatchSetHelper<T,Con,false>
 * @tparam T    集合中元素类型
 * @tparam Con  底层集合的类型
 * 能够应用于sort的集合
 */
template<typename T, typename Con>
class MathSetHelper<T,Con, true> : public MathSetHelper<T,Con, false>{
private:

public:
    //父类别名
    using super = typename MathSetHelper<T,Con, false>::self;

    MathSetHelper() = default;
    //区间构造
    template<typename InputIter>
    MathSetHelper(InputIter first, InputIter last): super(first,last) { }

    //通过底层集合来构造一个MathSetHelper,使用移动方式
    //此构造函数用于辅助于并，交，差函数
    explicit MathSetHelper(Con && con): super(std::move(con)) { }

    ~MathSetHelper() override = default;


    ///并集，交集，差集
    ///此并集，交集，差集会操作会对原容器进行排序
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

//默认使用std::set作为底层容器
//你可以宏定义__MATSET_DEFAULT_CON_STD_VECTOR来将std::vector作为默认的底层容器
//注意如果你使用了其他的任意集合——请确保他们有相应的接口。
//MathSet的性质是底层集合的性质
//例如vector中元素可以重复，无需插入，那么用vector为基础模板的MathSet的行为表现的和vector一样
#if defined(__MATSET_DEFAULT_CON_STD_VECTOR)
template <typename T, typename Con = std::vector<T>>
#else
template <typename T, typename Con = std::set<T>>
#endif
class MathSet {
private:
    using base_type = MathSetHelper<T,Con, false>;

    //使用unique_ptr指向MathSetHelper,确保方法的一致性。
    std::unique_ptr<base_type> m_data;
    using object_type = MathSetHelper<T,Con,has_sort<Con>::value>;
public:
    using iterator = typename base_type::iterator;
    using const_iterator = typename base_type::const_iterator;

    //构造函数
    MathSet(): m_data(make_unique<object_type>()) { }
    template<typename InputIter>
    MathSet(InputIter first, InputIter last): m_data(make_unique<object_type>(first,last)) { }
    //通过容器来构造MathSet,使用移动操作
    explicit MathSet(Con && con): m_data(std::make_unique<object_type>(std::move(con))) { }

    //此集合不提供initializer构造(这样做或导致重载匹配与预期不符合)
    //但是可以用这个方法来代替
    static MathSet from_initializer_list(const std::initializer_list<T> & ls) {
        return MathSet(ls.begin(), ls.end());
    }

    MathSet(MathSet && rhs) noexcept = default;
    //拷贝构造
    //MathSetHelper支持默认的拷贝构造
    //由于std::unique_ptr不支持拷贝构造
    //所以需要手动实现,使用区间初始化
    MathSet(MathSet & rhs): MathSet(rhs.begin(),rhs.end()) { }

    //赋值运算符，包括左值和右值版本
    //左值版本进行指针的深拷贝
    MathSet & operator=(MathSet && rhs) noexcept = default;

    MathSet & operator=(const MathSet & rhs) {
        m_data.reset(new object_type(*rhs.m_data));
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
        //result: Con
        auto result =  m_data->oneon(*rhs.m_data);
        return MathSet(std::move(result));
    }

    MathSet intersection(MathSet & rhs) {
        auto result = m_data->intersection(*rhs.m_data);
        return MathSet(std::move(result));
    }

    MathSet difference(MathSet & rhs) {
        auto result = m_data->difference(*rhs.m_data);
        return MathSet(std::move(result));
    }

    void insert(const T & val) {
        m_data->insert(val);
    }

    bool operator==(const MathSet & rhs) const {
        //自我赋值的情况交给底层容器
        if (size() != rhs.size()) {
            return false;
        }
        return m_data->get_container() == rhs.m_data->get_container();
    }
    //清空MathSet，确保底层容器有clear方法
    void clear() {
        m_data->get_container().clear();
    }

    //判断容器是否为空
    [[nodiscard]] bool is_empty() const {
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
     * 此操作不会对任何一个集合排序, 时间复杂度O(N*M)
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
    bool contain_with_order(const MathSet &);

};

/**
 * 两个有序集合，判断此集合是否包含另一个集合\n
 * 如果不是有序的，此函数是一个未定义的行为\n
 * 时间复杂度O(N+M)
 * @tparam T
 * @tparam Con
 * @param rhs
 * @return 如果包含，返回true，否则返回false
 */
template<typename T, typename Con>
bool MathSet<T,Con>::contain_with_order(const MathSet & rhs) {
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
