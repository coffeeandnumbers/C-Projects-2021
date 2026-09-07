#include <iostream>
#include <vector>
#include <deque>
#include <exception>
#include <iterator>
#include <cassert>

template<typename T>
class Deque;

template<typename T>
class WideArray;

template<typename T>
class Reverse {
private:
    int array_num = 0;
    int width_index = 0;
    const std::vector<WideArray<T>*>* parent = nullptr;
    T* ptr = nullptr;

    void correct_indexes();

public:
    Reverse(int array_, int index_, const std::vector<WideArray<T>*>* parent_);

    T* pointer() const;

    T& deref() const;

    void increment();

    void decrement();

    void plus(int x);

    int place() const;
};

template<typename T>
Reverse<T>::Reverse(int array_, int index_, const std::vector<WideArray<T>*>* parent_) :
        array_num(array_), width_index(index_), parent(parent_) {
    if (static_cast <size_t> (array_num) < 0)
        ptr = nullptr;
    else
        ptr = (*parent)[array_num]->pointer(width_index);
}

template<typename T>
void Reverse<T>::correct_indexes() {
    if (width_index < 0) {
        int width = static_cast<int> (Deque<T>::width);
        int change = width_index / width;
        --change;
        width_index -= change * width;
        array_num += change;
    }
    if (width_index >= Deque<T>::width) {
        int width = static_cast<int> (Deque<T>::width);
        int change = width_index / width;
        width_index -= change * width;
        array_num += change;
    }

    ptr = (*parent)[array_num]->pointer(width_index);
}

template<typename T>
T* Reverse<T>::pointer() const {
    return ptr;
}

template<typename T>
T& Reverse<T>::deref() const {
    return *ptr;
}

template<typename T>
void Reverse<T>::increment() {
    ++width_index;
    correct_indexes();
}

template<typename T>
void Reverse<T>::decrement() {
    --width_index;
    correct_indexes();
}

template<typename T>
void Reverse<T>::plus(int x) {
    width_index += x;
    correct_indexes();
}

template<typename T>
int Reverse<T>::place() const {
    return (array_num * Deque<T>::width + width_index);
}

template<typename T>
class DequeReverse {
private:
    Reverse<T> pos;
public:
    DequeReverse(Reverse<T> pos);

    DequeReverse() = default;

    DequeReverse(const DequeReverse&) = default;

    DequeReverse& operator=(const DequeReverse&) = default;

    ~DequeReverse() = default;


    T& operator*();

    const T& operator*() const;

    T* operator->();

    const T* operator->() const;

    DequeReverse<T>& operator++();

    DequeReverse<T> operator++(int);

    DequeReverse<T>& operator--();

    DequeReverse<T> operator--(int);

    T& operator[](int x);

    const T& operator[](int x) const;

    DequeReverse<T>& operator+=(int x);

    DequeReverse<T>& operator-=(int x);

    int place() const;
};

template<typename T>
DequeReverse<T>::DequeReverse(Reverse<T> pos) : pos(pos) {}

template<typename T>
int DequeReverse<T>::place() const {
    return pos.place();
}

template<typename T>
T& DequeReverse<T>::operator*() {
    return pos.deref();
}

template<typename T>
const T& DequeReverse<T>::operator*() const {
    return pos.deref();
}

template<typename T>
DequeReverse<T>& DequeReverse<T>::operator++() {
    pos.increment();
    return *this;
}

template<typename T>
DequeReverse<T> DequeReverse<T>::operator++(int) {
    auto old = *this;
    ++(*this);
    return old;
}

template<typename T>
T* DequeReverse<T>::operator->() {
    return pos.pointer();
}

template<typename T>
const T* DequeReverse<T>::operator->() const {
    return pos.pointer();
}

template<typename T>
DequeReverse<T>& DequeReverse<T>::operator--() {
    pos.decrement();
    return *this;
}

template<typename T>
DequeReverse<T> DequeReverse<T>::operator--(int) {
    auto old = *this;
    --(*this);
    return old;
}

template<typename T>
T& DequeReverse<T>::operator[](int x) {
    auto tmp = *this;
    tmp += x;
    return *tmp;
}

template<typename T>
const T& DequeReverse<T>::operator[](int x) const {
    auto tmp = *this;
    tmp += x;
    return *tmp;
}

template<typename T>
DequeReverse<T>& DequeReverse<T>::operator+=(int x) {
    pos.plus(x);
    return *this;
}

template<typename T>
DequeReverse<T>& DequeReverse<T>::operator-=(int x) { return *this += -x; }

template<typename T>
bool operator==(const DequeReverse<T>& left, const DequeReverse<T>& right) {
    return left.place() == right.place();
}

template<typename T>
bool operator!=(const DequeReverse<T>& left, const DequeReverse<T>& right) {
    return !(left == right);
}

template<typename T>
bool operator<(const DequeReverse<T>& left, const DequeReverse<T>& right) {
    return left.place() < right.place();
}

template<typename T>
bool operator>(const DequeReverse<T>& left, const DequeReverse<T>& right) {
    return right < left;
}

template<typename T>
bool operator<=(const DequeReverse<T>& left, const DequeReverse<T>& right) {
    return !(right > left);
}

template<typename T>
bool operator>=(const DequeReverse<T>& left, const DequeReverse<T>& right) {
    return !(left < right);
}

template<typename T>
DequeReverse<T> operator+(DequeReverse<T> iter, int x) {
    iter += x;
    return iter;
}

template<typename T>
DequeReverse<T> operator-(DequeReverse<T> iter, int x) {
    iter -= x;
    return iter;
}

template<typename T>
DequeReverse<T> operator+(int x, DequeReverse<T> iter) {
    return iter + x;
}

template<typename T>
int operator-(const DequeReverse<T>& left, const DequeReverse<T>& right) {
    return left.place() - right.place();
}


template<typename T>
class Position {
private:
    int array_num = 0;
    int width_index = 0;
    const std::vector<WideArray<T>*>* parent = nullptr;
    T* ptr = nullptr;

    void correct_indexes();

public:
    Position(int array_, int index_, const std::vector<WideArray<T>*>* parent_);

    T* pointer() const;
    T& deref() const;
    void increment();
    void decrement();
    void plus(int x);
    int place() const;
};

template<typename T>
Position<T>::Position(int array_, int index_, const std::vector<WideArray<T>*>* parent_) :
        array_num(array_), width_index(index_), parent(parent_) {
    if (static_cast <size_t> (array_num) == parent_->size())
        ptr = nullptr;
    else
        ptr = (*parent)[array_num]->pointer(width_index);
}

template<typename T>
void Position<T>::correct_indexes() {
    if (width_index < 0) {
        int width = static_cast<int> (Deque<T>::width);
        int change = width_index / width;
        --change;
        width_index -= change * width;
        array_num += change;
    }
    if (width_index >= Deque<T>::width) {
        int width = static_cast<int> (Deque<T>::width);
        int change = width_index / width;
        width_index -= change * width;
        array_num += change;
    }

    ptr = (*parent)[array_num]->pointer(width_index);
}

template<typename T>
T* Position<T>::pointer() const {
    return ptr;
}

template<typename T>
T& Position<T>::deref() const {
    return *ptr;
}

template<typename T>
void Position<T>::increment() {
    ++width_index;
    correct_indexes();
}

template<typename T>
void Position<T>::decrement() {
    --width_index;
    correct_indexes();
}

template<typename T>
void Position<T>::plus(int x) {
    width_index += x;
    correct_indexes();
}

template<typename T>
int Position<T>::place() const {
    return (array_num * Deque<T>::width + width_index);
}

template<typename T>
class DequeIterator {
private:
    Position<T> pos;
public:
    DequeIterator(Position<T> pos);

    DequeIterator() = default;
    DequeIterator(const DequeIterator&) = default;
    DequeIterator& operator=(const DequeIterator&) = default;
    ~DequeIterator() = default;

    T& operator*();

    const T& operator*() const;

    DequeIterator<T>& operator++();
    DequeIterator<T> operator++(int);

    T* operator->();

    const T* operator->() const;

    DequeIterator<T>& operator--();

    DequeIterator<T> operator--(int);

    T& operator[](int x);

    const T& operator[](int x) const;

    DequeIterator<T>& operator+=(int x);

    DequeIterator<T>& operator-=(int x);

    int place() const;
};

template<typename T>
DequeIterator<T>::DequeIterator(Position<T> pos) : pos(pos) {}

template<typename T>
T& DequeIterator<T>::operator*() { return pos.deref(); }

template<typename T>
const T& DequeIterator<T>::operator*() const { return pos.deref(); }

template<typename T>
DequeIterator<T>& DequeIterator<T>::operator++() {
    pos.increment();
    return *this;
}

template<typename T>
DequeIterator<T> DequeIterator<T>::operator++(int) {
    auto old = *this;
    ++(*this);
    return old;
}

template<typename T>
T* DequeIterator<T>::operator->() { return pos.pointer(); };

template<typename T>
const T* DequeIterator<T>::operator->() const { return pos.pointer(); };

template<typename T>
DequeIterator<T>& DequeIterator<T>::operator--() {
    pos.decrement();
    return *this;
}

template<typename T>
DequeIterator<T> DequeIterator<T>::operator--(int) {
    auto old = *this;
    --(*this);
    return old;
}

template<typename T>
T& DequeIterator<T>::operator[](int x) {
    auto tmp = *this;
    tmp += x;
    return *tmp;
}

template<typename T>
const T& DequeIterator<T>::operator[](int x) const {
    auto tmp = *this;
    tmp += x;
    return *tmp;
}

template<typename T>
DequeIterator<T>& DequeIterator<T>::operator+=(int x) {
    pos.plus(x);
    return *this;
}

template<typename T>
DequeIterator<T>& DequeIterator<T>::operator-=(int x) { return *this += -x; }

template<typename T>
int DequeIterator<T>::place() const { return pos.place(); };

template<typename T>
bool operator==(const DequeIterator<T>& left, const DequeIterator<T>& right) {
    int first = left.place();
    int second = right.place();
    return first == second;
}

template<typename T>
bool operator!=(const DequeIterator<T>& left, const DequeIterator<T>& right) { return !(left == right); }

template<typename T>
bool operator<(const DequeIterator<T>& left, const DequeIterator<T>& right) {
    return left.place() < right.place();
}

template<typename T>
bool operator>(const DequeIterator<T>& left, const DequeIterator<T>& right) { return right < left; }

template<typename T>
bool operator<=(const DequeIterator<T>& left, const DequeIterator<T>& right) { return !(left > right); }

template<typename T>
bool operator>=(const DequeIterator<T>& left, const DequeIterator<T>& right) { return !(left < right); }

template<typename T>
DequeIterator<T> operator+(DequeIterator<T> iter, int x) {
    iter += x;
    return iter;
}

template<typename T>
DequeIterator<T> operator+(int x, DequeIterator<T> iter) { return iter + x; }

template<typename T>
DequeIterator<T> operator-(DequeIterator<T> iter, int x) {
    iter -= x;
    return iter;
}

template<typename T>
int operator-(const DequeIterator<T>& left, const DequeIterator<T>& right) {
    return left.place() - right.place();
}

template<typename T>
class WideArray {
private:
    T* array = nullptr;
    size_t begin = 0;
    size_t end = 0;
public:
    WideArray() noexcept;

    WideArray(bool) noexcept;

    WideArray(const WideArray<T>& another);

    T& operator[](size_t x);

    const T& operator[](size_t x) const;

    T& at(size_t x);

    const T& at(size_t x) const;

    T* pointer(size_t x);

    const T* pointer(size_t x) const;

    void push_back(const T& elem);

    void push_front(const T& elem);

    void push_back();

    void push_front();

    void pop_back();

    void pop_front();

    bool isEmpty() const noexcept;

    ~WideArray();
};

template<typename T>
WideArray<T>::WideArray() noexcept {
    array = reinterpret_cast<T*>(new int8_t[Deque<T>::width * sizeof(T)]);
}

template<typename T>
WideArray<T>::WideArray(bool) noexcept {
    array = reinterpret_cast<T*>(new int8_t[Deque<T>::width * sizeof(T)]);
    begin = (end = Deque<T>::width);
}

template<typename T>
WideArray<T>::WideArray(const WideArray<T>& another): begin(another.begin), end(another.end) {
    array = reinterpret_cast<T*>(new int8_t[Deque<T>::width * sizeof(T)]);
    try {
        std::uninitialized_copy(another.array, another.array + Deque<T>::width, array);
    }
    catch (...) {
        delete[] reinterpret_cast<int8_t*>(array);
        throw;
    }
}

template<typename T>
T& WideArray<T>::operator[](size_t x) {
    return *(array + x);
}

template<typename T>
const T& WideArray<T>::operator[](size_t x) const {
    return *(array + x);
}

template<typename T>
T* WideArray<T>::pointer(size_t x) {
    return (array + x);
}

template<typename T>
const T* WideArray<T>::pointer(size_t x) const {
    return (array + x);
}

template<typename T>
const T& WideArray<T>::at(size_t x) const {
    if ((x < begin) || (x >= end))
        throw std::out_of_range("bad index");
    else
        return *(array + x);
}

template<typename T>
T& WideArray<T>::at(size_t x) {
    if ((x < begin) || (x >= end))
        throw std::out_of_range("bad index");
    else
        return *(array + x);
}

template<typename T>
void WideArray<T>::push_back(const T& elem) {
    try {
        new(array + end) T(elem);
    }
    catch (...) {
        throw;
    }
    ++end;
}

template<typename T>
void WideArray<T>::push_front(const T& elem) {
    --begin;
    try {
        new(array + begin) T(elem);
    }
    catch (...) {
        ++begin;
        throw;
    }
}

template<typename T>
void WideArray<T>::push_back() {
    try {
        new(array + end) T();
    }
    catch (...) {
        throw;
    }
    ++end;
}

template<typename T>
void WideArray<T>::push_front() {
    --begin;
    try {
        new(array + begin) T();
    }
    catch (...) {
        ++begin;
        throw;
    }
}

template<typename T>
void WideArray<T>::pop_back() {
    --end;
    (array + end)->~T();
}

template<typename T>
void WideArray<T>::pop_front() {
    (array + begin)->~T();
    ++begin;
}

template<typename T>
bool WideArray<T>::isEmpty() const noexcept {
    return (begin == end);
}

template<typename T>
WideArray<T>::~WideArray() {
    delete[] reinterpret_cast<int8_t*>(array);
    array = nullptr;
}


template<typename T>
class Deque {
public:
    static const int width = 100;
private:
    static const size_t min_mid = 2;
    size_t middle;
    std::pair<int, int> head;
    std::pair<int, int> tail;
    std::vector<WideArray<T>*> body;

    void resize();

public:
    typedef DequeIterator<T> iterator;
    typedef const DequeIterator<T> const_iterator;
    typedef DequeReverse<T> reversed_iterator;
    typedef const DequeReverse<T> const_reversed_iterator;

    Deque();

    Deque(size_t length);

    Deque(const Deque<T>& another);

    Deque(size_t length, const T& def);

    void operator=(const Deque<T>& deq);

    ~Deque();

    T& operator[](size_t index) {
        int now = head.second + index;
        return (*body[head.first + now / width])[now % width];
    };

    const T& operator[](size_t index) const {
        int now = head.second + index;
        return (*body[head.first + now / width])[now % width];
    };

    T& at(size_t index) {
        if ((index < 0) || (index >= size()))
            throw std::out_of_range("bad index");
        else
            return (*this)[index];
    };

    const T& at(size_t index) const {
        if ((index < 0) || (index >= size()))
            throw std::out_of_range("bad index");
        else
            return (*this)[index];
    }

    size_t size() const noexcept { return (tail.first - head.first) * width + (tail.second - head.second);
    };


    iterator begin() {
        Position<T> pos(head.first, head.second, &body);
        iterator iter(pos);
        return iter;
    };

    iterator end() {
        Position<T> pos(tail.first, tail.second, &body);
        iterator iter(pos);
        return iter;
    };

    const_iterator begin() const {
        Position<T> pos(head.first, head.second, &body);
        const_iterator iter(pos);
        return iter;
    };

    const_iterator end() const {
        Position<T> pos(tail.first, tail.second, &body);
        const_iterator iter(pos);
        return iter;
    };

    const_iterator cbegin() const {
        Position<T> pos(head.first, head.second, &body);
        const_iterator iter(pos);
        return iter;
    };

    const_iterator cend() const {
        Position<T> pos(tail.first, tail.second, &body);
        const_iterator iter(pos);
        return iter;
    };

    reversed_iterator rbegin() {
        int second = head.second;
        int first = head.first;
        --second;
        if (second < 0)
            second += width, --first;
        Reverse<T> pos(first, second, &body);
        reversed_iterator iter(pos);
        return iter;
    };

    reversed_iterator rend() {
        int second = tail.second;
        int first = tail.first;
        --second;
        if (second < 0)
            second += width, --first;
        Reverse<T> pos(first, second, &body);
        reversed_iterator iter(pos);
        return iter;
    };

    reversed_iterator rbegin() const {
        int second = head.second;
        int first = head.first;
        --second;
        if (second < 0)
            second += width, --first;
        Reverse<T> pos(first, second, &body);
        const_reversed_iterator iter(pos);
        return iter;
    };

    const_reversed_iterator rend() const {
        int second = tail.second;
        int first = tail.first;
        --second;
        if (second < 0)
            second += width, --first;
        Reverse<T> pos(first, second, &body);
        const_reversed_iterator iter(pos);
        return iter;
    };

    const_reversed_iterator crbegin() const {
        int second = head.second;
        int first = head.first;
        --second;
        if (second < 0)
            second += width, --first;
        Reverse<T> pos(first, second, &body);
        const_reversed_iterator iter(pos);
        return iter;
    };

    const_reversed_iterator crend() const {
        int second = tail.second;
        int first = tail.first;
        --second;
        if (second < 0)
            second += width, --first;
        Reverse<T> pos(first, second, &body);
        const_reversed_iterator iter(pos);
        return iter;
    };


    bool is_empty() const noexcept { return head == tail; };

    void push_back(const T& elem);

    void pop_back();

    void push_front(const T& elem);

    void pop_front();

    void insert(iterator iter, const T& elem) {
        int pos = iter.place();
        int start_first = pos / width;
        int start_second = pos - start_first * width;
        int end_first = tail.first;
        int end_second = tail.second;

        std::vector<WideArray<T>*> newarr;
        newarr.resize(end_first - start_first + 1);
        for (size_t i = 0; i < static_cast<size_t> (newarr.size()); ++i)
            newarr[i] = new WideArray<T>;
        auto it = (iter - start_second);
        try {
            for (int i = 0; i < end_first - start_first; ++i) {
                for (int j = 0; j < width; ++j) {
                    if (i == 0 && j == start_second) {
                        newarr[i]->push_back(elem);
                        continue;
                    }
                    newarr[i]->push_back(*it);
                    ++it;
                }
            }
            for (int j = 0; j <= end_second; ++j) {
                if (end_first == start_first && j == start_second) {
                    newarr[end_first - start_first]->push_back(elem);
                    continue;
                }
                newarr[end_first - start_first]->push_back(*it);
                ++it;
            }
        }
        catch (...) {
            for (size_t j = 0; j < static_cast<size_t> (newarr.size()); ++j) {
                delete newarr[j];
            }
            throw;
        }
        for (int i = start_first; i <= end_first; ++i) {
            delete body[i];
            body[i] = newarr[i - start_first];
        }
        tail.second += 1;
        if (tail.second == width) {
            tail.second -= width;
            tail.first += 1;
        }
        resize();
    };
    void erase(iterator iter) {
        int pos = iter.place();
        int start_first = pos / width;
        int start_second = pos - start_first * width;
        int end_first = tail.first;
        int end_second = tail.second;
        end_second -= 2;
        if (end_second < 0) {
            end_second += width;
            end_first -= 1;
        }
        std::vector<WideArray<T>*> newarr;
        newarr.resize(end_first - start_first + 1);
        if (newarr.size() == 0) {
            delete body[start_first];
            body[start_first] = new WideArray<T>;
            tail.second -= 1;
            if (tail.second < 0) {
                tail.second += width;
                tail.first -= 1;
            }
            return;
        }
        for (size_t i = 0; i < newarr.size(); ++i)
            newarr[i] = new WideArray<T>;
        auto it = (iter - start_second);
        try {
            for (int i = 0; i < end_first - start_first; ++i) {
                for (int j = 0; j < width; ++j) {
                    if (i == 0 && j == start_second) {
                        ++it;
                    }
                    newarr[i]->push_back(*it);
                    ++it;
                }
            }
            for (int j = 0; j <= end_second; ++j) {
                if (end_first == start_first && j == start_second) {
                    ++it;
                }
                newarr[end_first - start_first]->push_back(*it);
                ++it;
            }
        }
        catch (...) {
            for (size_t j = 0; j < newarr.size(); ++j) {
                delete newarr[j];
            }
            throw;
        }
        for (int i = start_first; i <= end_first; ++i) {
            delete body[i];
            body[i] = newarr[i - start_first];
        }
        tail.second -= 1;
        if (tail.second < 0) {
            tail.second += width;
            tail.first -= 1;
        }
    }
};

template<typename T>
Deque<T>::Deque(): middle(min_mid), head({middle, 0}), tail({middle, 0}) {
    body.resize(middle * 2);
    size_t size = middle * 2;
    for (size_t i = 0; i < middle; ++i)
        body[i] = new WideArray<T>(false);
    for (size_t i = middle; i < size; ++i)
        body[i] = new WideArray<T>;
    resize();
}

template<typename T>
Deque<T>::Deque(const Deque<T>& another) : middle(another.middle), head(another.head), tail(another.tail) {
    size_t size = middle * 2;

    body.resize(size);
    size_t i = 0;
    try {
        for (; i < size; ++i)
            body[i] = new WideArray<T>(*another.body[i]);
    }
    catch (...) {
        for (size_t j = 0; j < i; ++j) {
            delete body[j];
        }
        throw;
    }
    resize();
}

template<typename T>
Deque<T>::Deque(size_t length): middle(min_mid) {
    while (middle * width < length)
        middle *= 2;
    head = {middle, 0};
    size_t size = middle * 2;
    tail ={middle + length / width, length % width};
    body.resize(size);
    for (size_t i = 0; i < middle; ++i)
        body[i] = new WideArray<T>(false);
    for (size_t i = middle; i < size; ++i)
        body[i] = new WideArray<T>;
    size_t mid = middle;
    try {
        while (length > width) {
            length -= width;
            for (size_t i = 0; i < width; ++i)
                body[mid]->push_back();
            ++mid;
        }
        for (size_t i = 0; i < length; ++i)
            body[mid]->push_back();
    }
    catch (...) {
        for (size_t j = 0; j < size; ++j) {
            delete body[j];
        }
        throw;
    }
    resize();
}

template<typename T>
Deque<T>::Deque(size_t length, const T& def): middle(min_mid) {
    while (middle * width < length)
        middle *= 2;
    head = {middle, 0};
    size_t size = middle * 2;
    tail ={middle + length / width, length % width};
    body.resize(size);
    for (size_t i = 0; i < middle; ++i)
        body[i] = new WideArray<T>(false);
    for (size_t i = middle; i < size; ++i)
        body[i] = new WideArray<T>;
    size_t mid = middle;
    try {
        while (length > width) {
            length -= width;
            for (size_t i = 0; i < width; ++i)
                body[mid]->push_back(def);
            ++mid;
        }
        for (size_t i = 0; i < length; ++i)
            body[mid]->push_back(def);
    }
    catch (...) {
        for (size_t j = 0; j < size; ++j) {
            delete body[j];
        }
        throw;
    }
    resize();
}

template<typename T>
void Deque<T>::operator=(const Deque<T>& another) {
    size_t size = body.size();
    for (size_t i = 0; i < size; ++i) {
        delete body[i];
    }
    middle = another.middle;
    head = another.head;
    tail = another.tail;
    size = middle * 2;
    body.resize(size);
    size_t i = 0;
    try {
        for (; i < size; ++i)
            body[i] = new WideArray<T>(*another.body[i]);
    }
    catch (...) {
        for (size_t j = 0; j < i; ++j) {
            delete body[j];
        }
        throw;
    }
    resize();
}

template<typename T>
void Deque<T>::resize() {
    std::pair<int, int> p_f = {0, 0};
    std::pair<int, int> p_s = {middle * 2, 0};
    if (head == p_f || tail == p_s) {
        std::vector<WideArray<T>*> newbody;
        size_t size = body.size();
        newbody.resize(size * 2);
        size_t i = 0;
        try {
            for (; i < size; ++i) {
                newbody[middle + i] = new WideArray<T>(*body[i]);
            }
        }
        catch (...) {
            for (size_t j = 0; j < i; ++j) {
                delete body[middle + j];
            }
        }
        size *= 2;
        for (size_t k = 0; k < middle; ++k)
            newbody[k] = new WideArray<T>(false);
        for (size_t k = size - middle; k < size; ++k)
            newbody[k] = new WideArray<T>;
        head.first += middle;
        tail.first += middle;
        middle *= 2;
        for (size_t i = 0; i < middle; ++i) {
        }
        body = newbody;
    }
}

template<typename T>
void Deque<T>::push_back(const T& elem) {
    try {
        body[tail.first]->push_back(elem);
    }
    catch (...) {
        throw;
    }
    ++tail.second;
    if (tail.second == width) {
        tail.second = 0;
        ++tail.first;
    }
    resize();
}

template<typename T>
void Deque<T>::push_front(const T& elem) {
    if (head.second == 0) {
        head.second += width;
        --head.first;
    }
    try {
        body[head.first]->push_front(elem);
    }
    catch (...) {
        if (head.second == width) {
            head.second -= width;
            ++head.first;
        }
        throw;
    }
    --head.second;
    resize();
}

template<typename T>
void Deque<T>::pop_back() {
    --tail.second;
    if (tail.second < 0) {
        tail.second += width;
        --tail.first;
    }
    body[tail.first]->pop_back();
}

template<typename T>
void Deque<T>::pop_front() {
    body[head.first]->pop_front();
    ++head.second;
    if (head.second == width) {
        head.second -= width;
        ++head.first;
    }
}

template<typename T>
Deque<T>::~Deque() {
    size_t size = middle * 2;
    for (size_t i = 0; i < size; ++i) {
        delete body[i];
    }
};
