// This file is distributed under the MIT license.
// See the LICENSE file for details.


#pragma once


#include <iterator>
#include <type_traits>
#include <utility>


namespace support
{


    //----------------------------------------------------------------------------------------------
    // <type_traits>
    //


    template<class T, class U = void>
    using EnableIf = typename std::enable_if<T::value, U>::type;


    template<class T, class U = void>
    using DisableIf = typename std::enable_if<!T::value, U>::type;


    template<class T>
    using Decay = typename std::decay<T>::type;


    template<class T>
    using RemoveReference = typename std::remove_reference<T>::type;


    template<class T>
    using RemoveCV = typename std::remove_cv<T>::type;


    namespace details
    {

        template<class T>
        struct RemoveCVRec {
            using type = RemoveCV<T>;
        };

        template<template<class ...> class T, class ...A>
        struct RemoveCVRec<T<A...>> {
            using type = T<typename RemoveCVRec<A>::type...>;
        };

    } // namespace details

    template<class T>
    using RemoveCVRec = typename details::RemoveCVRec<T>::type;


    template<class T, class U>
    using IsSame = typename std::is_same<T, U>::type;


    template<class From, class To>
    using IsConvertible = typename std::is_convertible<From, To>::type;


    //----------------------------------------------------------------------------------------------
    // <functional>
    //


    struct GetFirst
    {
        template<class T>
        auto operator ()(T&& t) const -> decltype(( std::forward<T>(t).first )) {
            return std::forward<T>(t).first;
        }
    };


    struct GetSecond
    {
        template<class T>
        auto operator ()(T&& t) const -> decltype(( std::forward<T>(t).second )) {
            return std::forward<T>(t).second;
        }
    };


    //----------------------------------------------------------------------------------------------
    // <iterator>
    //


    template<class Function>
    class FunctionOutputIterator
        : public std::iterator<std::output_iterator_tag, void, void, void, void>
    {
        Function Fn;

    public:
        explicit FunctionOutputIterator(Function&& F)
            : Fn(std::move(F))
        {
        }

        template<class T, class = DisableIf< IsSame<Decay<T>, FunctionOutputIterator> >>
        FunctionOutputIterator& operator =(T&& t)
        {
            Fn(std::forward<T>(t));
            return *this;
        }

        FunctionOutputIterator& operator *() {
            return *this;
        }

        FunctionOutputIterator& operator ++() {
            return *this;
        }

        FunctionOutputIterator& operator ++(int) {
            return *this;
        }
    };


    template<class Function>
    inline auto makeFunctionOutputIterator(Function&& F) -> FunctionOutputIterator<Decay<Function>> {
        return FunctionOutputIterator<Decay<Function>>(std::forward<Function>(F));
    }


    template<class Iterator, class Function>
    class MappedIterator
    {
        Iterator It;
        Function Fn;

    public:
        using iterator_category     = std::forward_iterator_tag;
        using value_type            = decltype(( std::declval<Function>()(*std::declval<Iterator>()) ));
        using reference             = void;
        using pointer               = void;
        using difference_type       = ptrdiff_t;

        explicit MappedIterator(Iterator I, Function&& F)
            : It(I)
            , Fn(std::move(F))
        {
        }

        auto operator *() -> value_type {
            return Fn(*It);
        }

        MappedIterator& operator ++() {
            ++It; return *this;
        }

        MappedIterator operator ++(int) {
            auto t = *this; ++It; return t;
        }

        friend bool operator ==(MappedIterator LHS, MappedIterator RHS) {
            return LHS.It == RHS.It;
        }

        friend bool operator !=(MappedIterator LHS, MappedIterator RHS) {
            return LHS.It != RHS.It;
        }
    };


    template<class Iterator, class Function>
    inline auto mapIterator(Iterator I, Function&& F) -> MappedIterator<Iterator, Decay<Function>>
    {
        return MappedIterator<Iterator, Decay<Function>>(I, std::forward<Function>(F));
    }


    template<class Iterator>
    inline auto mapFirstIterator(Iterator I) -> decltype( mapIterator(I, GetFirst()) )
    {
        return mapIterator(I, GetFirst());
    }


    template<class Iterator>
    inline auto mapSecondIterator(Iterator I) -> decltype( mapIterator(I, GetSecond()) )
    {
        return mapIterator(I, GetSecond());
    }


    //----------------------------------------------------------------------------------------------
    // <utility>
    //


    namespace details {
    namespace adl {

        using std::begin;
        using std::end;

        template<class T>
        auto adl_begin(T&& t) -> decltype(( begin(std::forward<T>(t)) ))
        {
            return begin(std::forward<T>(t));
        }

        template<class T>
        auto adl_end(T&& t) -> decltype(( end(std::forward<T>(t)) ))
        {
            return end(std::forward<T>(t));
        }

    }}

    using details::adl::adl_begin;
    using details::adl::adl_end;


} // namespace support