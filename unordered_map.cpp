#include <iostream>
#include <vector>
#include <algorithm>
#include <limits>
#include <unordered_map>
#include <set>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <list>
#include <iterator>
#include <memory>
#include <cmath>

template <size_t ChunkSize>
class FixedAllocator {
private:
    size_t min_capacity_;
    size_t capacity_ = 0;
    std::vector<int8_t*> heads_;
    int8_t* tail_ = nullptr;
    std::shared_ptr<std::allocator<int8_t>> Allocator;
public:
    FixedAllocator(): min_capacity_(ChunkSize * 20), capacity_(min_capacity_){
        heads_.push_back(reinterpret_cast<int8_t*> (operator new (sizeof(int8_t) * ChunkSize * capacity_)));
        tail_ = heads_.back();
        Allocator = std::make_shared<std::allocator<int8_t>>();
    };

    ~FixedAllocator() {
        for (auto it : heads_)
            operator delete(reinterpret_cast<void*> (it));
    }

    FixedAllocator(const FixedAllocator&) = delete;

    int8_t* allocate() {
        if (tail_ == (heads_.back() + capacity_ * ChunkSize)) {
            capacity_ *= 2;
            heads_.push_back(reinterpret_cast<int8_t*> (operator new (sizeof(int8_t) * ChunkSize * capacity_)));
            tail_ = heads_.back();
        }
        auto ans = tail_;
        tail_ += ChunkSize;
        return ans;
    }

    void deallocate() {
    }
};

template <typename T>
class FastAllocator {
private:
    std::shared_ptr<FixedAllocator<sizeof(T)>> fa;
    std::shared_ptr<std::allocator<T>> Allocator;
public:
    using value_type = T;
    template <typename U> constexpr FastAllocator (const FastAllocator <U>&) noexcept {
        Allocator = std::make_shared<std::allocator<T>>();
        fa = std::make_shared<FixedAllocator<sizeof(T)>>();
    };

    FastAllocator() {
        Allocator = std::make_shared<std::allocator<T>>();
        fa = std::make_shared<FixedAllocator<sizeof(T)>>();
    }

    T* allocate(size_t n) {
        if (n == 1) {
            return reinterpret_cast<T*>(fa->allocate());
        } else {
            return Allocator->allocate(n);
        }
    }

    void deallocate(T* p, size_t n) {
        if (n == 1) {
            fa->deallocate();
        } else {
            Allocator->deallocate(p, n);
        }
    }
};

template <class T, class U>
bool operator==(const FastAllocator<T>&, const FastAllocator<U>&) { return true; }
template <class T, class U>
bool operator!=(const FastAllocator<T>&, const FastAllocator<U>&) { return false; }

template<typename T>
struct Node {
    T elem_;
    Node<T>* left_ = nullptr;
    Node<T>* right_ = nullptr;
    Node(const T& elem, Node<T>* left, Node<T>* right): elem_(elem), left_(left), right_(right) {};
    Node(T&& elem, Node<T>* left, Node<T>* right): elem_(std::move(elem)), left_(left), right_(right) {};
    Node(Node<T>* left, Node<T>* right): elem_(T()), left_(left), right_(right) {};
    Node(Node<T>* node): elem_(node->elem_), left_(node->left_), right_(node->right_) {};
};

template<typename T>
class Position {
private:
public:
    Node<T>* ptr_ = nullptr;
    Position(Node<T>* ptr);

    T* pointer() const;
    T& deref() const;
    void increment();
    void decrement();
};

template<typename T>
Position<T>::Position(Node<T>* ptr): ptr_(ptr) {}

template<typename T>
T* Position<T>::pointer() const {return &ptr_->elem_;}

template<typename T>
T& Position<T>::deref() const {return ptr_->elem_;}

template<typename T>
void Position<T>::increment() {ptr_ = ptr_->right_;}

template<typename T>
void Position<T>::decrement() { ptr_ = ptr_->left_;}

template <typename T, typename Allocator = std::allocator<T>>
class List{
public:
    template<bool is_const>
    class ListIterator;
    typename std::allocator_traits<Allocator>::template rebind_alloc<Node<T>> alloc_;
public:
    typedef ListIterator<false> iterator;
    typedef ListIterator<true> const_iterator;
    typedef std::reverse_iterator<ListIterator<false>> reverse_iterator;
    typedef std::reverse_iterator<ListIterator<true>> const_reverse_iterator;
    typedef std::allocator_traits<decltype(alloc_)> AllocTraits;

    List(size_t count, const T& value, const Allocator& allocator = Allocator());
    List(size_t count, const Allocator& allocator = Allocator());
    List(const List&);
    List(List<T, Allocator>&& another);
    List& operator=(List<T, Allocator>&& another);
    List& operator=(const List&);
    bool operator==(const List&);
    bool operator!=(const List&);
    ~List();

    iterator create_iterator(Node<T>* ptr);
    const_iterator create_iterator(Node<T>* ptr) const;
    template<class... Args>
    Node<T>* construct(Args&&... args);
    void destroy(Node<T>* ptr);
    template<bool is_const>
    void insert(const ListIterator<is_const>& it, Node<T>* ptr);
    iterator begin();
    iterator end();
    const_iterator begin() const;
    const_iterator end() const;
    const_iterator cbegin() const;
    const_iterator cend() const;
    reverse_iterator rbegin();
    reverse_iterator rend();
    const_reverse_iterator rbegin() const;
    const_reverse_iterator rend() const;
    const_reverse_iterator crbegin() const;
    const_reverse_iterator crend() const;

    size_t size() const {return size_;};
    Allocator get_allocator() {return alloc_;};
    explicit List(const Allocator& alloc = Allocator());
    void push_back(const T&);
    void push_front(const T&);
    void pop_back();
    void pop_front();
    template<bool is_const>
    void insert(const ListIterator<is_const>&, const T&);
    template<bool is_const>
    void insert(const ListIterator<is_const>&, T&&);
    template<bool is_const>
    void erase(const ListIterator<is_const>&);
private:
    Node<T>* begin_ = nullptr;
    Node<T>* end_ = nullptr;
    size_t size_;
};

template<typename T, typename Allocator>
template<bool is_const>
class List<T, Allocator>::ListIterator {
private:
    friend class List<T, Allocator>;
public:
    Position<T> pos;
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = T;
    using difference_type = std::ptrdiff_t;
    using pointer = std::conditional_t<is_const, const T*, T*>;
    using reference = std::conditional_t<is_const, const T&, T&>;
    ListIterator(Position<T> pos);

    ListIterator() = default;
    ListIterator(const ListIterator&) = default;
    ListIterator& operator=(const ListIterator&) = default;
    ~ListIterator() = default;

    reference operator*();
    pointer operator->();
    std::conditional_t<is_const, const Node<T>&, Node<T>&> get_reference();
    std::conditional_t<is_const, const Node<T>*, Node<T>*> get_pointer();

    ListIterator& operator++();
    ListIterator operator++(int);

    ListIterator& operator--();
    ListIterator operator--(int);

    bool operator==(const ListIterator&);
    bool operator!=(const ListIterator&);

    explicit operator ListIterator<true>() const {
        return ListIterator<true> (pos);
    }
};

template<typename T, typename Allocator>
template<bool is_const>
List<T, Allocator>::ListIterator<is_const>::ListIterator(Position<T> pos) : pos(pos) {}

template <typename T, typename Allocator>
template <bool is_const>
std::conditional_t<is_const, const T&, T&>
List<T, Allocator>::ListIterator<is_const>::operator*() { return pos.deref(); }

template <typename T, typename Allocator>
template <bool is_const>
std::conditional_t<is_const, const T*, T*>
List<T, Allocator>::ListIterator<is_const>::operator->() { return pos.pointer(); }

template <typename T, typename Allocator>
template <bool is_const>
std::conditional_t<is_const, const Node<T>&, Node<T>&>
List<T, Allocator>::ListIterator<is_const>::get_reference() {
    return *pos.ptr_;
}

template <typename T, typename Allocator>
template <bool is_const>
std::conditional_t<is_const, const Node<T>*, Node<T>*>
List<T, Allocator>::ListIterator<is_const>::get_pointer() {
    return pos.ptr_;
}

template<typename T, typename Allocator>
template<bool is_const>
typename List<T, Allocator>::template ListIterator<is_const>& List<T, Allocator>::ListIterator<is_const>::operator++() {
    pos.increment();
    return *this;
}

template<typename T, typename Allocator>
template<bool is_const>
typename List<T, Allocator>::template ListIterator<is_const> List<T, Allocator>::ListIterator<is_const>::operator++(int) {
    auto old = *this;
    ++(*this);
    return old;
}

template<typename T, typename Allocator>
template<bool is_const>
typename List<T, Allocator>::template ListIterator<is_const>& List<T, Allocator>::ListIterator<is_const>::operator--() {
    pos.decrement();
    return *this;
}

template<typename T, typename Allocator>
template<bool is_const>
typename List<T, Allocator>::template ListIterator<is_const> List<T, Allocator>::ListIterator<is_const>::operator--(int) {
    auto old = *this;
    --(*this);
    return old;
}

template<typename T, typename Allocator>
template<bool is_const>
bool List<T, Allocator>::ListIterator<is_const>::operator==(const ListIterator& right) {

    return (this->pos.pointer()) == (right.pos.pointer());
}

template<typename T, typename Allocator>
template<bool is_const>
bool List<T, Allocator>::ListIterator<is_const>::operator!=(const ListIterator& right) {
    return !(*this == right);
}

template <typename T, typename Allocator>
typename List<T, Allocator>::iterator List<T, Allocator>::begin() {
    return iterator(Position<T>(begin_->right_));
}

template <typename T, typename Allocator>
typename List<T, Allocator>::iterator List<T, Allocator>::end() {
    return iterator(Position<T>(end_));
}

template <typename T, typename Allocator>
typename List<T, Allocator>::const_iterator List<T, Allocator>::begin() const {
    return const_iterator(Position<T>(begin_->right_));
}

template <typename T, typename Allocator>
typename List<T, Allocator>::const_iterator List<T, Allocator>::end() const {
    return const_iterator(Position<T>(end_));
}

template <typename T, typename Allocator>
typename List<T, Allocator>::const_iterator List<T, Allocator>::cbegin() const {
    return const_iterator(Position<T>(begin_->right_));
}

template <typename T, typename Allocator>
typename List<T, Allocator>::const_iterator List<T, Allocator>::cend() const {
    return const_iterator(Position<T>(end_));
}

template <typename T, typename Allocator>
typename List<T, Allocator>::reverse_iterator List<T, Allocator>::rbegin() {
    return std::make_reverse_iterator(end());
}

template <typename T, typename Allocator>
typename List<T, Allocator>::reverse_iterator List<T, Allocator>::rend() {
    return std::make_reverse_iterator(begin());
}

template <typename T, typename Allocator>
typename List<T, Allocator>::const_reverse_iterator List<T, Allocator>::rbegin() const {
    return std::make_reverse_iterator(cend());
}

template <typename T, typename Allocator>
typename List<T, Allocator>::const_reverse_iterator List<T, Allocator>::rend() const {
    return std::make_reverse_iterator(cbegin());
}

template <typename T, typename Allocator>
typename List<T, Allocator>::const_reverse_iterator List<T, Allocator>::crbegin() const {
    return std::make_reverse_iterator(cend());
}

template <typename T, typename Allocator>
typename List<T, Allocator>::const_reverse_iterator List<T, Allocator>::crend() const {
    return std::make_reverse_iterator(cbegin());
}

template <typename T, typename Allocator>
typename List<T, Allocator>::iterator List<T, Allocator>::create_iterator(Node<T>* ptr) {
    if (ptr != nullptr)
        return (++ListIterator<false>(Position<T>(ptr)));
    return begin();
}

template <typename T, typename Allocator>
typename List<T, Allocator>::const_iterator List<T, Allocator>::create_iterator(Node<T>* ptr) const {
    if (ptr != nullptr)
        return (++ListIterator<true>(Position<T>(ptr)));
    return cbegin();
}

template <typename T, typename Allocator>
template<class... Args>
Node<T>* List<T, Allocator>::construct(Args&&... args) {
    auto ptr = AllocTraits::allocate(alloc_, 1);
    Allocator alloc;
    typedef std::allocator_traits<decltype(alloc)> AT;
    auto now = AT::allocate(alloc, 1);
    AT::construct(alloc, now, std::forward<Args>(args)...);
    AllocTraits::construct(alloc_, ptr, std::move(*now), nullptr, nullptr);
    AT::destroy(alloc, now);
    AT::deallocate(alloc, now, 1);
    return ptr;
}

template <typename T, typename Allocator>
void List<T, Allocator>::destroy(Node<T>* ptr) {
    AllocTraits::destroy(alloc_, ptr);
    AllocTraits::deallocate(alloc_, ptr, 1);
}

template <typename T, typename Allocator>
template<bool is_const>
void List<T, Allocator>::insert(const ListIterator<is_const>& it, Node<T>* ptr) {
    Position<T> pos = (it.pos);
    ptr->left_ = pos.ptr_->left_;
    ptr->right_ = pos.ptr_;
    pos.ptr_->left_->right_ = ptr;
    pos.ptr_->left_ = ptr;
    ++size_;
}

template <typename T, typename Allocator>
List<T, Allocator>::List(size_t count, const T& value, const Allocator& allocator):
        alloc_(allocator), size_(count) {
    begin_ = AllocTraits::allocate(alloc_, 1);
    end_ = AllocTraits::allocate(alloc_, 1);
    auto current = begin_;
    for (size_t i = 0; i < count; ++i){
        current->right_ = AllocTraits::allocate(alloc_, 1);
        AllocTraits::construct(alloc_, current->right_, value, current, nullptr);
        current = current->right_;
    }
    current->right_ = end_;
    end_->left_ = current;
}

template <typename T, typename Allocator>
List<T, Allocator>::List(size_t count, const Allocator& allocator):
        alloc_(allocator), size_(count) {
    begin_ = AllocTraits::allocate(alloc_, 1);
    end_ = AllocTraits::allocate(alloc_, 1);
    auto current = begin_;
    for (size_t i = 0; i < count; ++i){
        current->right_ = AllocTraits::allocate(alloc_, 1);
        AllocTraits::construct(alloc_, current->right_, current, nullptr);
        current = current->right_;
    }
    current->right_ = end_;
    end_->left_ = current;
}

template <typename T, typename Allocator>
List<T, Allocator>::List(const List& list): size_(list.size_) {
    alloc_ = std::allocator_traits<Allocator>::select_on_container_copy_construction(list.alloc_);
    begin_ = AllocTraits::allocate(alloc_, 1);
    end_ = AllocTraits::allocate(alloc_, 1);
    auto current = begin_;
    for (auto it = list.begin(); it != list.end(); ++it) {
        current->right_ = AllocTraits::allocate(alloc_, 1);
        AllocTraits::construct(alloc_, current->right_, *it, current, nullptr);
        current = current->right_;
    }
    current->right_ = end_;
    end_->left_ = current;
}

template <typename T, typename Allocator>
List<T, Allocator>& List<T, Allocator>::operator=(const List& list) {
    if (*this == list)
        return *this;
    auto current = begin_->right_;
    AllocTraits::deallocate(alloc_, begin_, 1);
    while (current != end_) {
        current = current->right_;
        AllocTraits::destroy(alloc_, current->left_);
        AllocTraits::deallocate(alloc_, current->left_, 1);
    }
    AllocTraits::deallocate(alloc_, end_, 1);
    size_ = list.size_;
    if (AllocTraits ::propagate_on_container_copy_assignment::value && alloc_ != list.alloc_)
        alloc_ = list.alloc_;
    begin_ = AllocTraits::allocate(alloc_, 1);
    end_ = AllocTraits::allocate(alloc_, 1);
    current = begin_;
    for (auto it = list.begin(); it != list.end(); ++it) {
        current->right_ = AllocTraits::allocate(alloc_, 1);
        AllocTraits::construct(alloc_, current->right_, *it, current, nullptr);
        current = current->right_;
    }
    current->right_ = end_;
    end_->left_ = current;
    return *this;
}

template <typename T, typename Allocator>
List<T, Allocator>::List(List<T, Allocator>&& another) {
    size_ = another.size_;
    another.size_ = 0;
    begin_ = another.begin_;
    end_ = another.end_;
    another.begin_ = std::allocator_traits<decltype(another.alloc_)>::allocate(alloc_, 1);
    another.end_ = std::allocator_traits<decltype(another.alloc_)>::allocate(alloc_, 1);
    another.begin_->right_ = another.end_;
    another.end_->left_ = another.begin_;
    if (std::allocator_traits<Allocator>::propagate_on_container_copy_assignment::value) {
        alloc_ = another.alloc_;
    }
}

template <typename T, typename Allocator>
List<T, Allocator>& List<T, Allocator>::operator=(List<T, Allocator>&& another) {
    if (this == &another)
        return *this;
    size_ = another.size_;
    another.size_ = 0;
    begin_ = another.begin_;
    end_ = another.end_;
    another.begin_ = std::allocator_traits<decltype(another.alloc_)>::allocate(another.alloc_, 1);
    another.end_ = std::allocator_traits<decltype(another.alloc_)>::allocate(another.alloc_, 1);
    another.begin_->right_ = another.end_;
    another.end_->left_ = another.begin_;
    if (std::allocator_traits<Allocator>::propagate_on_container_copy_assignment::value) {
        alloc_ = another.alloc_;
    }
    return *this;
}

template <typename T, typename Allocator>
List<T, Allocator>::List(const Allocator& alloc): alloc_(alloc), size_(0) {
    begin_ = AllocTraits::allocate(alloc_, 1);
    end_ = AllocTraits::allocate(alloc_, 1);
    begin_->right_ = end_;
    end_->left_ = begin_;
};

template <typename T, typename Allocator>
bool List<T, Allocator>::operator==(const List<T, Allocator>& list) {
    return (begin_ == list.begin_) && (end_ == list.end_);
};

template <typename T, typename Allocator>
bool List<T, Allocator>::operator!=(const List<T, Allocator>& list) {
    return !(*this == list);
};

template <typename T, typename Allocator>
void List<T, Allocator>::push_back(const T& value) {
    auto ptr = AllocTraits::allocate(alloc_, 1);
    AllocTraits::construct(alloc_, ptr, value, end_->left_, end_);
    end_->left_->right_ = ptr;
    end_->left_ = ptr;
    ++size_;
};

template <typename T, typename Allocator>
void List<T, Allocator>::push_front(const T& value) {
    auto ptr = AllocTraits::allocate(alloc_, 1);
    AllocTraits::construct(alloc_, ptr, value, begin_, begin_->right_);
    begin_->right_->left_ = ptr;
    begin_->right_ = ptr;
    ++size_;
};

template <typename T, typename Allocator>
void List<T, Allocator>::pop_back() {
    auto current = end_->left_;
    end_->left_ = end_->left_->left_;
    end_->left_->right_ = end_;
    AllocTraits::destroy(alloc_, current);
    AllocTraits::deallocate(alloc_, current, 1);
    --size_;
};

template <typename T, typename Allocator>
void List<T, Allocator>::pop_front() {
    auto current = begin_->right_;
    begin_->right_ = begin_->right_->right_;
    begin_->right_->left_ = begin_;
    AllocTraits::destroy(alloc_, current);
    AllocTraits::deallocate(alloc_, current, 1);
    --size_;
}

template <typename T, typename Allocator>
template<bool is_const>
void List<T, Allocator>::insert(const ListIterator<is_const>& it, const T& value) {
    Position<T> pos = (it.pos);
    auto ptr = AllocTraits::allocate(alloc_, 1);
    AllocTraits::construct(alloc_, ptr, value, pos.ptr_->left_, pos.ptr_);
    pos.ptr_->left_->right_ = ptr;
    pos.ptr_->left_ = ptr;
    ++size_;
}

template <typename T, typename Allocator>
template<bool is_const>
void List<T, Allocator>::insert(const ListIterator<is_const>& it, T&& value) {
    Position<T> pos = (it.pos);
    auto ptr = AllocTraits::allocate(alloc_, 1);
    AllocTraits::construct(alloc_, ptr, std::move(value), pos.ptr_->left_, pos.ptr_);
    pos.ptr_->left_->right_ = ptr;
    pos.ptr_->left_ = ptr;
    ++size_;
}

template <typename T, typename Allocator>
template<bool is_const>
void List<T, Allocator>::erase(const ListIterator<is_const>& it) {
    Position<T> pos = (it.pos);
    pos.ptr_->right_->left_ = pos.ptr_->left_;
    pos.ptr_->left_->right_ = pos.ptr_->right_;
    AllocTraits::destroy(alloc_, pos.ptr_);
    AllocTraits::deallocate(alloc_, pos.ptr_, 1);
    --size_;
}

template <typename T, typename Allocator>
List<T, Allocator>::~List() {
    if (begin_ == nullptr)
        return;
    auto current = begin_->right_;
    AllocTraits::deallocate(alloc_, begin_, 1);
    while (current != end_) {
        current = current->right_;
        AllocTraits::destroy(alloc_, current->left_);
        AllocTraits::deallocate(alloc_, current->left_, 1);
    }
    AllocTraits::deallocate(alloc_, end_, 1);
}


template <typename T>
struct UnorderedMapNode{
    UnorderedMapNode(const T& node, size_t cache = 0): node_(node), cache_(cache) {}
    UnorderedMapNode(T&& node, size_t cache = 0): node_(
            std::move(const_cast<typename std::remove_const<decltype(node.first)>::type&>
                      (node.first)), std::move(node.second)), cache_(cache) {}
    template<typename U, typename W>
    UnorderedMapNode(U&& first, W&& second, size_t cache): node_(std::make_pair(first, second)), cache_(cache) {}
    template<class... Args>
    UnorderedMapNode(Args&&... args): node_(std::forward<Args>(args)...) {}
    UnorderedMapNode(const UnorderedMapNode<T>& another) = default;
    UnorderedMapNode(UnorderedMapNode<T>&& another):
            node_(std::move(const_cast<typename std::remove_const<decltype(another.node_.first)>::type&>
                            (another.node_.first)), std::move(another.node_.second)) {
        cache_ = another.cache_;
    }
    T node_;
    size_t cache_;
};

template <typename Key,
        typename Value,
        typename Hash = std::hash<Key>,
        typename Equal = std::equal_to<Key>,
        typename Alloc = std::allocator<std::pair<const Key,Value>>>
class UnorderedMap {
private:
    template<bool is_const>
    class UnorderedMapIterator;
public:
    typedef std::pair<const Key, Value> NodeType;
    typedef UnorderedMapIterator<false> iterator;
    typedef UnorderedMapIterator<true> const_iterator;

    UnorderedMap();
    UnorderedMap(const UnorderedMap<Key, Value, Hash, Equal, Alloc>&);
    UnorderedMap& operator=(const UnorderedMap<Key, Value, Hash, Equal, Alloc>&);
    UnorderedMap(UnorderedMap<Key, Value, Hash, Equal, Alloc>&& another);
    UnorderedMap& operator=(UnorderedMap<Key, Value, Hash, Equal, Alloc>&& another);
    ~UnorderedMap() = default;

    Value& operator[](const Key&);
    Value& at(const Key&);
    const Value& at(const Key&) const;

    iterator find(const Key&);
    const_iterator find(const Key&) const;

    std::pair<iterator, bool> insert(const NodeType&);
    std::pair<iterator, bool> insert(NodeType&&);

    template<class... Args>
    std::pair<iterator, bool> emplace(Args&&... args);

    iterator erase(const_iterator pos);
    iterator erase(iterator pos);
    iterator erase(iterator first, iterator last);

    void resize();
    void resize(size_t);
    size_t size() const { return list_.size();}
    size_t cache(const Key& key) const { return hash_(key) % body_.size();};

    template<class InputIterator>
    void insert(InputIterator first, InputIterator last);

    iterator begin();
    iterator end();
    const_iterator begin() const;
    const_iterator end() const;
    const_iterator cbegin() const;
    const_iterator cend() const;

    float max_load_factor() const { return max_load_factor_; }
    void reserve(size_t count) {if (count > body_.size()) resize(std::ceil(count / max_load_factor()));}
    float load_factor() const { return static_cast<float>(list_.size()) / static_cast<float>(body_.size());}

private:
    void add_body(Node<UnorderedMapNode<NodeType>>*) noexcept;
    std::vector<Node<UnorderedMapNode<NodeType>>*> body_;
    List<UnorderedMapNode<NodeType>, Alloc> list_;
    Hash hash_;
    Equal equal_;
    const size_t min_capcity_ = 100;
    const float max_load_factor_ = 0.95;
};

template <typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
template<bool is_const>
class UnorderedMap<Key, Value, Hash, Equal, Alloc>::UnorderedMapIterator {
private:
    typename List<UnorderedMapNode<UnorderedMap<Key, Value, Hash, Equal, Alloc>::NodeType>,
            Alloc>::template ListIterator<is_const> iterator_;
    friend class UnorderedMap<Key, Value, Hash, Equal, Alloc>;
    size_t& get_hash() { return iterator_.pos.ptr_->elem_.cache_; };
public:
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = UnorderedMap<Key, Value, Hash, Equal, Alloc>::NodeType;
    using difference_type = std::ptrdiff_t;

    using pointer = std::conditional_t<is_const,
            const UnorderedMap<Key, Value, Hash, Equal, Alloc>::NodeType*,
            UnorderedMap<Key, Value, Hash, Equal, Alloc>::NodeType*>;

    using reference = std::conditional_t<is_const,
            const UnorderedMap<Key, Value, Hash, Equal, Alloc>::NodeType&,
            UnorderedMap<Key, Value, Hash, Equal, Alloc>::NodeType&>;

    UnorderedMapIterator(typename List<UnorderedMapNode<UnorderedMap<Key, Value, Hash, Equal, Alloc>::NodeType>, Alloc>::
    template ListIterator<is_const> pos);

    UnorderedMapIterator() = default;
    UnorderedMapIterator(const UnorderedMapIterator&) = default;
    UnorderedMapIterator& operator=(const UnorderedMapIterator&) = default;
    ~UnorderedMapIterator() = default;

    reference operator*();
    pointer operator->();

    UnorderedMapIterator& operator++();
    UnorderedMapIterator operator++(int);


    bool operator==(const UnorderedMapIterator&);
    bool operator!=(const UnorderedMapIterator&);

    explicit operator UnorderedMapIterator<true>() const {
        return UnorderedMapIterator<true> (iterator_);
    }
};


template <typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
template<bool is_const>
UnorderedMap<Key, Value, Hash, Equal, Alloc>::UnorderedMapIterator<is_const>::UnorderedMapIterator
        (typename List<UnorderedMapNode<UnorderedMap<Key, Value, Hash, Equal, Alloc>::NodeType>, Alloc>::
        template ListIterator<is_const> pos) : iterator_(pos) {}

template <typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
template <bool is_const>
std::conditional_t<is_const, const typename UnorderedMap<Key, Value, Hash, Equal, Alloc>::NodeType&,
        typename UnorderedMap<Key, Value, Hash, Equal, Alloc>::NodeType&>
UnorderedMap<Key, Value, Hash, Equal, Alloc>::UnorderedMapIterator<is_const>::operator*() {
    return iterator_->node_;
}

template <typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
template <bool is_const>
std::conditional_t<is_const, const typename UnorderedMap<Key, Value, Hash, Equal, Alloc>::NodeType*,
        typename UnorderedMap<Key, Value, Hash, Equal, Alloc>::NodeType*>
UnorderedMap<Key, Value, Hash, Equal, Alloc>::UnorderedMapIterator<is_const>::operator->() {
    return &(iterator_->node_);
}

template <typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
template<bool is_const>
typename UnorderedMap<Key, Value, Hash, Equal, Alloc>::template UnorderedMapIterator<is_const>&
UnorderedMap<Key, Value, Hash, Equal, Alloc>::UnorderedMapIterator<is_const>::operator++() {
    ++iterator_;
    return *this;
}

template <typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
template<bool is_const>
typename UnorderedMap<Key, Value, Hash, Equal, Alloc>::template UnorderedMapIterator<is_const>
UnorderedMap<Key, Value, Hash, Equal, Alloc>::UnorderedMapIterator<is_const>::operator++(int) {
    return UnorderedMapIterator<is_const>(iterator_++);
}

template <typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
template<bool is_const>
bool UnorderedMap<Key, Value, Hash, Equal, Alloc>::UnorderedMapIterator<is_const>::
operator==(const UnorderedMap<Key, Value, Hash, Equal, Alloc>::UnorderedMapIterator<is_const>& right) {
    return iterator_ == right.iterator_;
}

template <typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
template<bool is_const>
bool UnorderedMap<Key, Value, Hash, Equal, Alloc>::UnorderedMapIterator<is_const>::
operator!=(const UnorderedMap<Key, Value, Hash, Equal, Alloc>::UnorderedMapIterator<is_const>& right) {
    return !(*this == right);
}

template <typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
typename UnorderedMap<Key, Value, Hash, Equal, Alloc>::iterator UnorderedMap<Key, Value, Hash, Equal, Alloc>::begin() {
    return iterator(list_.begin());
}

template <typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
typename UnorderedMap<Key, Value, Hash, Equal, Alloc>::iterator UnorderedMap<Key, Value, Hash, Equal, Alloc>::end() {
    return iterator(list_.end());
}

template <typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
typename UnorderedMap<Key, Value, Hash, Equal, Alloc>::const_iterator UnorderedMap<Key, Value, Hash, Equal, Alloc>::begin() const {
    return const_iterator(list_.cbegin());
}

template <typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
typename UnorderedMap<Key, Value, Hash, Equal, Alloc>::const_iterator UnorderedMap<Key, Value, Hash, Equal, Alloc>::end() const {
    return const_iterator(list_.cend());
}

template <typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
typename UnorderedMap<Key, Value, Hash, Equal, Alloc>::const_iterator UnorderedMap<Key, Value, Hash, Equal, Alloc>::cbegin() const {
    return const_iterator(list_.cbegin());
}

template <typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
typename UnorderedMap<Key, Value, Hash, Equal, Alloc>::const_iterator UnorderedMap<Key, Value, Hash, Equal, Alloc>::cend() const {
    return const_iterator(list_.cend());
}

template <typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
UnorderedMap<Key, Value, Hash, Equal, Alloc>::UnorderedMap() {
    body_.resize(min_capcity_);
}

template <typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
UnorderedMap<Key, Value, Hash, Equal, Alloc>::UnorderedMap(const UnorderedMap<Key, Value, Hash, Equal, Alloc>& another) {
    body_.resize(another.body_.size());
    try { list_ = another.list_;}
    catch (...) { throw;}
    for (auto it = list_.begin(); it != list_.end(); ++it)
        add_body(it.get_pointer());
}

template <typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
UnorderedMap<Key, Value, Hash, Equal, Alloc>& UnorderedMap<Key, Value, Hash, Equal, Alloc>::
operator=(UnorderedMap<Key, Value, Hash, Equal, Alloc>&& another) {
    list_ = std::move(another.list_);
    body_ = std::move(another.body_);
    return *this;
}

template <typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
UnorderedMap<Key, Value, Hash, Equal, Alloc>::UnorderedMap(UnorderedMap<Key, Value, Hash, Equal, Alloc>&& another):
        body_(std::move(another.body_)), list_(std::move(another.list_)) {}


template <typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
UnorderedMap<Key, Value, Hash, Equal, Alloc>&
UnorderedMap<Key, Value, Hash, Equal, Alloc>::operator=(const UnorderedMap<Key, Value, Hash, Equal, Alloc>& another) {
    List<UnorderedMapNode<NodeType>, Alloc> list;
    try { list = another.list_;}
    catch (...) { throw;}
    list_.~List<UnorderedMapNode<NodeType>, Alloc>();
    list_ = std::move(list);
    body_.clear();
    body_.resize(another.body_.size());
    for (auto it = list_.begin(); it != list_.end(); ++it)
        add_body(it.get_pointer());
}

template <typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
Value& UnorderedMap<Key, Value, Hash, Equal, Alloc>::operator[](const Key& key) {
    auto iter = find(key);
    if (iter == end()) {
        iter = list_.create_iterator(body_[cache(key)]);
        list_.insert(iter.iterator_, UnorderedMapNode<NodeType>(key, Value(), cache(key)));
        --iter.iterator_;
        add_body(iter.iterator_.get_pointer());
        resize();
        return find(key)->second;
    }
    return iter->second;
}

template <typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
Value& UnorderedMap<Key, Value, Hash, Equal, Alloc>::at(const Key& key) {
    auto iter = find(key);
    if (iter == end())
        throw std::out_of_range("bad index");
    return iter->second;
}

template <typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
const Value& UnorderedMap<Key, Value, Hash, Equal, Alloc>::at(const Key& key) const {
    auto iter = find(key);
    if (iter == end())
        throw std::out_of_range("bad index");
    return iter->second;
}

template <typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
std::pair<typename UnorderedMap<Key, Value, Hash, Equal, Alloc>::iterator, bool>
UnorderedMap<Key, Value, Hash, Equal, Alloc>::insert(const NodeType& node) {
    auto iter = find(node.first);
    if (iter != end())
        return {iter, false};
    iter = list_.create_iterator(body_[cache(node.first)]);
    list_.insert(iter.iterator_, UnorderedMapNode<NodeType>(node, cache(node.first)));
    --iter.iterator_;
    add_body(iter.iterator_.get_pointer());
    resize();
    return {find(node.first), true};
}

template <typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
std::pair<typename UnorderedMap<Key, Value, Hash, Equal, Alloc>::iterator, bool>
UnorderedMap<Key, Value, Hash, Equal, Alloc>::insert(NodeType&& node) {
    auto iter = find(node.first);
    if (iter != end())
        return {iter, false};
    iter = list_.create_iterator(body_[cache(node.first)]);
    list_.insert(iter.iterator_, std::move(UnorderedMapNode<NodeType>(std::move(node), cache(node.first))));
    --iter.iterator_;
    add_body(iter.iterator_.get_pointer());
    resize();
    return {find(node.first), true};
}

template <typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
template<class InputIterator>
void UnorderedMap<Key, Value, Hash, Equal, Alloc>::insert(InputIterator first, InputIterator last) {
    for (; first != last; ++first)
        insert(*first);
}

template <typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
typename UnorderedMap<Key, Value, Hash, Equal, Alloc>::iterator
UnorderedMap<Key, Value, Hash, Equal, Alloc>::erase(const_iterator iter) {
    if (iter == end())
        return iter;
    if (iter.iterator_.pos.ptr_->right_ != end().iterator_.pos.ptr_
        && body_[iter.iterator_.pos.ptr_->right.cache_] == iter.iterator_.pos.ptr_) {
        body_[iter.iterator_.pos.ptr_->right.cache_] = iter.iterator_.pos.ptr_->left;
    }
    if (body_[iter.iterator_.pos.ptr_.cache_] == iter.iterator_.pos.ptr_->right)
        body_[iter.iterator_.pos.ptr_->cache_] = nullptr;
    list_.erase(iter);
    return iter;
}

template <typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
typename UnorderedMap<Key, Value, Hash, Equal, Alloc>::iterator
UnorderedMap<Key, Value, Hash, Equal, Alloc>::erase(iterator iter) {
    if (iter.iterator_.pos.ptr_->right_ != end().iterator_.pos.ptr_
        && body_[iter.iterator_.pos.ptr_->right_->elem_.cache_] == iter.iterator_.pos.ptr_) {
        body_[iter.iterator_.pos.ptr_->right_->elem_.cache_] = iter.iterator_.pos.ptr_->left_;
    }
    if (body_[iter.iterator_.pos.ptr_->elem_.cache_] == iter.iterator_.pos.ptr_->right_)
        body_[iter.iterator_.pos.ptr_->elem_.cache_] = nullptr;
    list_.erase(iter.iterator_);
    return iter;
}

template <typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
typename UnorderedMap<Key, Value, Hash, Equal, Alloc>::iterator
UnorderedMap<Key, Value, Hash, Equal, Alloc>::erase(iterator first, iterator last) {
    for (; first != last;)
        erase(first++);
    return last;
}

template <typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
void UnorderedMap<Key, Value, Hash, Equal, Alloc>::add_body(Node<UnorderedMapNode<NodeType>>* ptr) noexcept {
    if (body_[ptr->elem_.cache_] == nullptr) {
        body_[ptr->elem_.cache_] = (--list_.begin()).get_pointer();
        if (list_.size() > 1)
            body_[(++(list_.begin()))->cache_] = ptr;
    }
}

template <typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
typename UnorderedMap<Key, Value, Hash, Equal, Alloc>::iterator
UnorderedMap<Key, Value, Hash, Equal, Alloc>::find(const Key& key) {
    size_t cache = this->cache(key);
    auto ptr = body_[cache];
    auto iter = iterator(list_.create_iterator(ptr));
    if (iter != begin() && iter != end())
        ptr->elem_.cache_ %= body_.size();
    for (; iter != iterator(list_.end()) && iter.get_hash() == cache; ++iter) {
        if (equal_(iter->first, key))
            return iter;
    }
    return end();
}

template <typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
typename UnorderedMap<Key, Value, Hash, Equal, Alloc>::const_iterator
UnorderedMap<Key, Value, Hash, Equal, Alloc>::find(const Key& key) const {
    size_t cache = this->cache(key);
    auto ptr = body_[cache];
    auto iter = const_iterator(list_.create_iterator(ptr));
    for (; iter != iterator(list_.end()) && iter.get_hash() == cache; ++iter) {
        if (equal_(iter->first, key))
            return iter;
    }
    return cend();
}

template <typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
void UnorderedMap<Key, Value, Hash, Equal, Alloc>::resize() {
    if (static_cast<float>(list_.size())/static_cast<float>(body_.size()) > max_load_factor_)
        resize(body_.size() * 2);
}

template <typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
void UnorderedMap<Key, Value, Hash, Equal, Alloc>::resize(size_t size) {
    for (iterator it = begin(); it != end(); ++it) {
        it.get_hash() = hash_(it->first) % size;
    }
    body_.clear();
    body_.resize(size);
    for (iterator it = begin(); it != end(); ++it) {
        add_body(it.iterator_.get_pointer());
    }
    resize();
}

template <typename Key, typename Value, typename Hash, typename Equal, typename Alloc>
template<class... Args>
std::pair<typename UnorderedMap<Key, Value, Hash, Equal, Alloc>::iterator, bool>
UnorderedMap<Key, Value, Hash, Equal, Alloc>::emplace(Args&&... args) {
    Node<UnorderedMapNode<NodeType>>* ptr = list_.construct(std::forward<Args>(args)...);
    auto iter = find(ptr->elem_.node_.first);
    if (iter != end()) {
        list_.destroy(ptr);
        return {iter, false};
    }
    ptr->elem_.cache_ = cache(ptr->elem_.node_.first);
    list_.insert(list_.create_iterator(body_[ptr->elem_.cache_]), ptr);
    add_body(ptr);
    iter = iterator(list_.create_iterator(ptr));
    resize();
    return {iter, true};
}
