#pragma once

#include "type_traits/base.hpp"
#include "type_traits/is_type.hpp"
#include "type_traits/is_operation.hpp"
#include "type_traits/relations.hpp"
#include "type_traits/decay.hpp"
#include "type_traits/reference_wrapper.hpp"

#include "utility/declval.hpp"
#include "utility/typecast.hpp"

namespace yilib::__internal {
    // 20.14.4.1.1
    template<class MemberPtr, class Base, class T1, class ...Args> 
    requires is_member_function_pointer<MemberPtr Base::*>::value && is_base_of<Base, typename decay<T1>::type>::value
    constexpr auto __INVOKE(int*, MemberPtr Base::* fn, T1&& t1, Args&& ...args)
        noexcept(noexcept((forward<T1>(t1).*fn)(forward<Args>(args)...))) -> decltype((forward<T1>(t1).*fn)(forward<Args>(args)...)) {
            return (forward<T1>(t1).*fn)(forward<Args>(args)...);
    }

    // 20.14.4.1.2
    template<class MemberPtr, class Base, class T1, class ...Args> 
    requires is_member_function_pointer<MemberPtr Base::*>::value && __is_reference_wrapper<typename decay<T1>::type>::value
    constexpr auto __INVOKE(int*, MemberPtr Base::* fn, T1&& t1, Args&& ...args)
        noexcept(noexcept((t1.get().*fn)(forward<Args>(args)...))) -> decltype((t1.get().*fn)(forward<Args>(args)...)) {
            return (t1.get().*fn)(forward<Args>(args)...);
    }

    // 20.14.4.1.3
    template<class MemberPtr, class Base, class T1, class ...Args> 
    requires is_member_function_pointer<MemberPtr Base::*>::value
    constexpr auto __INVOKE(void*, MemberPtr Base::* fn, T1&& t1, Args&& ...args)
        noexcept(noexcept(((*forward<T1>(t1)).*fn)(forward<Args>(args)...))) -> decltype(((*forward<T1>(t1)).*fn)(forward<Args>(args)...)) {
            return ((*forward<T1>(t1)).*fn)(forward<Args>(args)...);
    }

    template<class MemberPtr, class Base, class T1>
    requires is_member_object_pointer<MemberPtr Base::*>::value && is_base_of<Base, typename decay<T1>::type>::value
    constexpr auto __INVOKE(int*, MemberPtr Base::* fn, T1&& t1)
        noexcept(noexcept(forward<T1>(t1).*fn)) -> decltype(forward<T1>(t1).*fn) {
            return forward<T1>(t1).*fn;
    }

    template<class MemberPtr, class Base, class T1>
    requires is_member_object_pointer<MemberPtr Base::*>::value && __is_reference_wrapper<typename decay<T1>::type>::value
    constexpr auto __INVOKE(int*, MemberPtr Base::* fn, T1&& t1)
        noexcept(noexcept(t1.get().*fn)) -> decltype(t1.get().*fn) {
            return t1.get().*fn;
    }

    template<class MemberPtr, class Base, class T1>
    requires is_member_object_pointer<MemberPtr Base::*>::value
    constexpr auto __INVOKE(void*, MemberPtr Base::* fn, T1&& t1)
        noexcept(noexcept((*forward<T1>(t1)).*fn)) -> decltype((*forward<T1>(t1)).*fn) {
            return (*forward<T1>(t1)).*fn;
    }

    template<class Fn, class ...Args>
    constexpr auto __INVOKE(void*, Fn&& fn, Args&& ...args) 
        noexcept(noexcept((forward<Fn>(fn))(forward<Args>(args)...))) -> decltype((forward<Fn>(fn))(forward<Args>(args)...)) {
        return (forward<Fn>(fn))(forward<Args>(args)...);
    }

    template<class Void, class ...Args> requires is_void<Void>::value
    struct __is_invocable : false_type {};
    template<class ...Args> struct __is_invocable<void_t<decltype(__INVOKE(static_cast<int*>(nullptr), declval<Args>()...))>, Args...> : true_type {};

    template<class Fn, class ...Args> struct is_invocable : __is_invocable<void, Fn, Args...> {};

    template<class R, class ...Args>
    struct __is_invocable_r {
        using Return = decltype(__INVOKE(static_cast<int*>(nullptr), declval<Args>()...));
        using type = typename conditional<
            is_void<R>::value,
            true_type,
            is_convertible<Return, R>
        >::type;
    };

    template<class R, class Fn, class ...Args> struct is_invocable_r : __is_invocable_r<R, Fn, Args...>::type {};

    template<class Fn, class ...Args>
    constexpr auto __INVOKE_NOTHROW(int*, Fn&& fn, Args&& ...args) noexcept -> decltype(auto) {
        return __INVOKE(static_cast<int*>(nullptr), forward<Fn>(fn), forward<Args>(args)...);
    }

    template<class Void, class ...Args> requires is_void<Void>::value
    struct __is_nothrow_invocable : false_type {};
    template<class ...Args> struct __is_nothrow_invocable<void_t<decltype(__INVOKE_NOTHROW(static_cast<int*>(nullptr), declval<Args>()...))>, Args...> : true_type {};

    template<class Fn, class ...Args> struct is_nothrow_invocable : __is_nothrow_invocable<void, Fn, Args...> {};

    template<class R, class ...Args>
    struct __is_nothrow_invocable_r {
        using Return = decltype(__INVOKE_NOTHROW(static_cast<int*>(nullptr), declval<Args>()...));
        using type = typename conditional<
            is_void<R>::value,
            true_type,
            is_convertible<Return, R>
        >::type;
    };

    template<class R, class Fn, class ...Args> struct is_nothrow_invocable_r : __is_nothrow_invocable_r<R, Fn, Args...>::type {};

    template<class Void, class ...Args> struct __invoke_result {};
    template<class ...Args> struct __invoke_result<void_t<decltype(__INVOKE(static_cast<int*>(nullptr), declval<Args>()...))>, Args...> {
        using type = decltype(__INVOKE(static_cast<int*>(nullptr), declval<Args>()...));
    };

    template<class Fn, class ...Args> struct invoke_result : __invoke_result<void, Fn, Args...> {};
}