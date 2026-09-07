#include <iostream>
#include <vector>
#include <algorithm>
#include <limits>
#include <set>
#include <list>
#include "tuple"
#include <type_traits>

template<size_t Size, typename Item>
class Leaf {
public:
    Item value;
    Leaf(): value() {}
    template<typename T>
    Leaf(T&& val): value(std::forward<T>(val)){}
    Leaf(Leaf&&) = default;
    Leaf(const Leaf&) = default;
    template<typename T>
    Item& operator=(T&& val) {std::forward<T>(val);return value;}
    Leaf& operator=(Leaf&&) = default;
    Leaf& operator=(const Leaf&) = default;
};

template<size_t Size, typename... Args>
class TupleType;

template<size_t Size>
class TupleType<Size>{
public:
    static const int size = Size;
    TupleType<Size>() = default;
    TupleType<Size>(const TupleType<Size>& another) = default;
    TupleType<Size>& operator=(const TupleType<Size>& another) = default;
    TupleType<Size>(TupleType<Size>&& another) = default;
    TupleType<Size>& operator=(TupleType<Size>&& another) = default;
    void swap(TupleType<Size>&) {}
    bool operator==(const TupleType<Size>&) {return true;}
    bool operator!=(const TupleType<Size>&) {return false;}
    bool operator<(const TupleType<Size>&) {return false;}
    bool operator>(const TupleType<Size>&) {return false;}
    bool operator<=(const TupleType<Size>&) {return true;}
    bool operator>=(const TupleType<Size>&) {return true;}
};

template<size_t Size, typename Head, typename... Args>
class TupleType<Size, Head, Args...>: public Leaf<Size, Head>, public TupleType<Size + 1, Args...> {
public:
    template<size_t, typename ...>
    friend class TupleType;
    TupleType<Size, Head, Args...>(): Leaf<Size, Head>(), TupleType<Size + 1, Args...>() {}

    template<typename H, typename... Others>
    explicit TupleType<Size, Head, Args...>(H&& head, Others&&... args):
            Leaf<Size, Head>(std::forward<H>(head)), TupleType<Size + 1, Args...> (std::forward<Others>(args)...) {}

    TupleType<Size, Head, Args...>(const TupleType<Size, Head, Args...>& another) = default;

    TupleType<Size, Head, Args...>(TupleType<Size, Head, Args...>&& another) = default;

    template<size_t S, typename H, typename... A>
    TupleType<Size, Head, Args...>& operator=(const TupleType<S, H, A...>& another) {
        Leaf<Size, Head>::operator=(static_cast<const Leaf<S, H>&>(another).value);
        TupleType<Size + 1, Args...>::operator=(static_cast<const TupleType<S + 1, A...>&>(another));
        return *this;
    }
    TupleType<Size, Head, Args...>& operator=(const TupleType<Size, Head, Args...>& another) = default;

    template<size_t S, typename H, typename... A>
    TupleType<Size, Head, Args...>& operator=(TupleType<S, H, A...>&& another){
        Leaf<Size, Head>::operator=(static_cast<TupleType<S, H>&&>(another).value);
        TupleType<Size + 1, Args...>::operator=(static_cast<TupleType<S + 1, A...>&&>(another));
        return *this;
    }
    TupleType<Size, Head, Args...>& operator=(TupleType<Size, Head, Args...>&& another) = default;

    void swap(TupleType<Size, Head, Args...>& another) {
        std::swap(Leaf<Size, Head>::value, another.Leaf<Size, Head>::value);
        TupleType<Size + 1, Args...>::swap(another);
    }
    template<size_t S, typename H, typename... A>
    bool operator==(const TupleType<S, H, A...>& another) {
        return Leaf<Size, Head>::value == another.Leaf<S, H>::value &&
               TupleType<Size + 1, Args...>::operator==(static_cast<const TupleType<S + 1, A...>&>(another));}
    template<size_t S, typename H, typename... A>
    bool operator!=(const TupleType<S, H, A...>& another) {return !(*this == another);}
    template<size_t S, typename H, typename... A>
    bool operator<(const TupleType<S, H, A...>& another) {
        if (Leaf<Size, Head>::value < another.Leaf<S, H>::value)
            return true;
        if (Leaf<Size, Head>::value > another.Leaf<S, H>::value)
            return false;
        return TupleType<Size + 1, Args...>::operator<(static_cast<const TupleType<S + 1, A...>&>(another));}
    template<size_t S, typename H, typename... A>
    bool operator>(const TupleType<S, H, A...>& another) {return !(*this < another || *this == another);}
    template<size_t S, typename H, typename... A>
    bool operator<=(const TupleType<S, H, A...>& another) {return !(*this > another);}
    template<size_t S, typename H, typename... A>
    bool operator>=(const TupleType<S, H, A...>& another) {return (*this > another || *this == another);}
    ~TupleType() = default;
};

template<size_t Size, typename Head, typename... Args>
Head& get(TupleType<Size, Head, Args...>& tuple) {
    return tuple.Leaf<Size, Head>::value;
}
template<typename Head, size_t Size, typename... Args>
Head& get(TupleType<Size, Head, Args...>& tuple) {
    return tuple.Leaf<Size, Head>::value;
}
template<size_t Size, typename Head, typename... Args>
const Head& get(const TupleType<Size, Head, Args...>& tuple) {
    return tuple.Leaf<Size, Head>::value;
}
template<typename Head, size_t Size, typename... Args>
const Head& get(const TupleType<Size, Head, Args...>& tuple) {
    return tuple.Leaf<Size, Head>::value;
}
template<size_t Size, typename Head, typename... Args,
        typename = std::enable_if<true>>
Head&& get(TupleType<Size, Head, Args...>&& tuple) {
    return std::move(tuple.Leaf<Size, Head>::value);
}
template<typename Head, size_t Size, typename... Args>
Head&& get(TupleType<Size, Head, Args...>&& tuple) {
    return std::move(tuple.Leaf<Size, Head>::value);
}
template<size_t Size, typename Head, typename... Args>
const Head&& get(const TupleType<Size, Head, Args...>&& tuple) {
    return std::move(tuple.Leaf<Size, Head>::value);
}

template<size_t... Args>
struct CustomSequence;

template<size_t Head, size_t... Args>
struct CustomSequence<Head, Args...> {static void print() {std::cout << Head;CustomSequence<Args...>::print();}};

template<>
struct CustomSequence<> {static void print() {}};

template<size_t... Args>
CustomSequence<Args...> functionCat(CustomSequence<Args...>);

template<size_t... Args, size_t... Kwargs>
CustomSequence<Args..., Kwargs...> functionCat(CustomSequence<Args...>, CustomSequence<Kwargs...>);

template<typename...>
struct SequenceCat;

template<typename Sequence>
struct SequenceCat<Sequence>{
    using value = decltype(functionCat(Sequence()));
};

template<typename First, typename Second>
struct SequenceCat<First, Second>{
    using value = decltype(functionCat(First(), Second()));
};

template<typename First, typename Second, typename... Args>
struct SequenceCat<First, Second, Args...> {
    using value = decltype(functionCat(First(), typename SequenceCat<Second, Args...>::value()));
};

template<size_t Value, size_t Size>
struct RepeatString{
    using value = typename SequenceCat<CustomSequence<Value>,
            typename RepeatString<Value, Size - 1>::value>::value;
};

template<size_t Value>
struct RepeatString<Value, 1> {
    using value = typename SequenceCat<CustomSequence<Value>>::value;
};

template<size_t...>
struct MakeRepeat;

template<size_t Index>
struct MakeRepeat<Index> {
    using value = CustomSequence<>;
};

template<size_t Index, size_t Length, size_t... Args>
struct MakeRepeat<Index, Length, Args...> {
    using value = typename SequenceCat<typename RepeatString<Index, Length>::value,
            typename MakeRepeat<Index + 1, Args...>::value>::value;
};

template<size_t...>
struct MakeIncreasing;

template<size_t Index>
struct MakeIncreasing<Index> {
    using value = CustomSequence<>;
};

template<size_t Index, size_t Length, size_t... Args>
struct MakeIncreasing<Index, Length, Args...> {
    using value = typename SequenceCat<CustomSequence<Index>,
            typename MakeIncreasing<Index + 1, Length - 1, Args...>::value>::value;
};

template<size_t Index, size_t... Args>
struct MakeIncreasing<Index, 1, Args...> {
    using value = typename SequenceCat<CustomSequence<Index>,
            typename MakeIncreasing<0, Args...>::value>::value;
};

template<size_t... Args>
using MakeIncreasing_T = typename MakeIncreasing<0, Args...>::value;

template<typename... Args>
using Tuple = TupleType<0, Args...>;

template<typename... Args>
Tuple<typename std::decay<Args>::type...> makeTuple(Args&&... args) {
return Tuple<typename std::decay<Args>::type...>(std::forward<Args>(args)...);
}

template<typename...>
struct Type;

template<>
struct Type<> {
    using value = Tuple<>;
};

template<typename... First>
struct Type<Tuple<First...>> {
    using value = Tuple<First...>;
};

template<typename... First, typename... Second, typename... Args>
struct Type<Tuple<First...>, Tuple<Second...>, Args...> {
    using value = typename Type<Tuple<First..., Second...>, Args...>::value;
};

template<typename... Args>
using Type_T = typename Type<Args...>::value;

template<typename Ans, typename Index, typename... Args>
struct HelpCat;

template<typename Ans, size_t... Index, typename First, typename Second, typename... Args>
struct HelpCat<Ans, CustomSequence<Index...>, First, Second, Args...> {
    template<typename... Pack>
    static Ans ans(First&& first, Second&& second, Args&&... args, Pack&&... pack) {
        return HelpCat<Ans, MakeIncreasing_T<std::remove_reference_t<Second>::size>, Second, Args...>::
        ans(std::forward<Second>(second), std::forward<Args>(args)...,
            std::forward<Pack>(pack)..., get<Index>(std::forward<First>(first))...);
    }
};

template<typename Ans, size_t... Index, typename First>
struct HelpCat<Ans, CustomSequence<Index...>, First> {
    template<typename... Pack>
    static Ans ans(First&& first, Pack&&... pack) {
        return Ans(std::forward<Pack>(pack)..., get<Index>(std::forward<First>(first))...);
    }
};

template<typename First, typename... Args, typename Ans =
Type_T<std::remove_const_t<std::remove_reference_t<First>>,
        std::remove_const_t<std::remove_reference_t<Args>>...>,
        typename Index = MakeIncreasing_T<std::remove_reference_t<First>::size>>
Ans tupleCat(First&& first, Args&&... args) {
    return HelpCat<Ans, Index, First, Args...>::ans(std::forward<First>(first), std::forward<Args>(args)...);
}
