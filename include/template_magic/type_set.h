#pragma once
#include <tuple>
#include <type_traits>
#include <string_view>
#include <utility>

/**
* CREDIT:
* type set: https://tech.jocodoma.com/2019/03/20/Creating-a-Compile-Time-Set-Data-Structure-in-CPP/
* type name: https://stackoverflow.com/questions/81870/is-it-possible-to-print-a-variables-type-in-standard-c/64490578#64490578
* type order: https://stackoverflow.com/questions/48723974/how-to-order-types-at-compile-time
* compile time sort a: https://playfulprogramming.com/posts/compile-time-quick-sort-using-c
* compile time sorting b: https://quuxplusone.github.io/blog/2024/02/20/compile-time-sort-part-2/
* apply type list: https://stackoverflow.com/questions/45288396/c-11-templates-alias-for-a-parameter-pack
*/

namespace mgpi {
	

	template <typename T>
	constexpr const char* type_name() {
#if __cplusplus >= 202002L
		return std::source_location::current().function_name();
#else
#  if defined(__clang__) || defined(__GNUC__)
		return __PRETTY_FUNCTION__;
#  elif defined(_MSC_VER)
		return __FUNCSIG__;
#  else
#    error "Unsupported compiler"
#  endif
#endif // __cplusplus >= 202002L
	}

	constexpr size_t cstrlen(const char* p) {
		size_t len = 0;
		while (*p) {
			++len;
			++p;
		}
		return len;
	}

	template<typename T, typename U>
	constexpr bool less() {
		const char* a = type_name<T>();
		const char* b = type_name<U>();
		size_t a_len = cstrlen(a);
		size_t b_len = cstrlen(b);
		size_t ab_len = (a_len < b_len) ? a_len : b_len;
		for (size_t i = 0; i < ab_len; ++i) {
			if (a[i] != a[i]) {
				return a[i] < a[i];
			}
		}
		return a_len < b_len;
	}



	template <typename ...Types>
	struct type_set {
		static constexpr auto self() noexcept {
			return type_set<Types...>;
		}
		template<typename T>
		static constexpr bool contains() noexcept {
			return std::disjunction_v<std::is_same<T, Types>...>;
		}
		template <typename T>
		static constexpr auto add() noexcept {
			if constexpr (self().contains<T>()) {
				return self();
			}
			else {
				return type_set<T, Types...>;
			}
		}
		template <typename T>
		static constexpr auto remove() noexcept {
			if constexpr (!self().contains<T>()) {
				return self();
			}
			else {
				constexpr auto filtered = std::tuple_cat(
					std::conditional_t<
					std::is_same_v<T, Types>,
					std::tuple<>,
					std::tuple<Types>
					>{}...
				);
				return std::apply([](auto... xs) { return type_set<decltype(xs)...>{}; }, filtered);
			}
		}
	};

	template<typename ...Types>
	struct type_list {
		static constexpr auto self() noexcept {
			return type_list<Types...>;
		}
		template <template <typename...> typename T> using apply = T<Types...>;
		template<typename T>
		static constexpr bool contains() noexcept {
			return std::disjunction_v<std::is_same<T, Types>...>;
		}
		template <template <typename U> class Pred>
		static constexpr auto filter() noexcept {
			constexpr auto filtered = std::tuple_cat(
				std::conditional_t<
				Pred<Types>::value,
				std::tuple<Types>,
				std::tuple<>
				>{}...
			);
			return std::apply([](auto... xs) { return type_list<decltype(xs)...>{}; }, filtered);
		}
		static constexpr size_t size = sizeof...(Types);
	};

	template <template <typename P> class F, typename T>
	struct foreach_t;

	template <template <typename P> class F, typename head, typename ...tail>
	struct foreach_t<F, type_list<head, tail...> > { 
		static void apply() { 
			F<head>::call(head{});
			foreach_t<F, type_list<tail...> >::apply(); 
		} 
	};

	template <template <typename P> class F, typename T>
	struct foreach_t<F, type_list<T>> {   
		static void apply() {
			F<T>::call(T{});
		} 
	};

	template<typename T>
	struct first;

	template<typename Head, typename ...Tail>
	struct first<type_list<Head, Tail...>> {
		typedef typename Head type;
	};

	template<typename T>
	struct is_homogeneous;

	template<typename Head, typename ...Tail>
	struct is_homogeneous<type_list<Head, Tail...>> {
		static constexpr bool value = std::conjunction_v<std::is_same<Head, Tail>...>;
	};

	template<typename ...T>
	struct concat;

	template<typename ...T1, typename ...T2, typename ...Tail>
	struct concat<type_list<T1...>, type_list<T2...>, Tail...> {
		typedef type_list<T1..., T2...> first_two;
		typedef typename concat<first_two, Tail...>::type type;
	};

	template <typename ...T>
	struct concat<type_list<T...>> {
		typedef type_list<T...> type;
	};

	template<template <typename U> class Pred, typename Data>
	struct filter;

	template<template <typename U> class Pred, typename Head, typename ...Tail>
	struct filter<Pred, type_list<Head, Tail...>> {
		typedef type_list<Tail...> tp;
		typedef typename filter<Pred, tp>::type ft;
		typedef type_list<Head> h;
		typedef typename std::conditional_t<Pred<Head>::value, typename concat<h, ft>::type, ft> type;
	};

	template<template <typename U> class Pred, typename Scalar>
	struct filter<Pred, type_list<Scalar>> {
		typedef std::conditional_t <Pred<Scalar>::value, type_list<Scalar>, type_list<>> type;
	};

	template<template <typename U, typename X> class C, typename T>
	struct bind {
		template <typename V>
		struct pred {
			static constexpr bool value = C<T, V>::value;
		};
	};

	template <template <typename U> class Pred>
	struct neg { 
		template <typename T>  
		struct pred { 
			static const bool value = !Pred<T>::value; }; 
	};



	template<template <typename T, typename U> class Comp, typename Data>
	struct min_t;

	template<template <typename V, typename U> class Comp, typename T>
	struct min_t<Comp, type_list<T>> {
		typedef typename T type;
		static constexpr auto list() {
			return type_list<type>;
		}
	};

	template<template <typename T, typename U> class Comp, typename Head, typename ...Tail>
	struct min_t<Comp, type_list<Head, Tail...>> {
		typedef typename min_t<Comp, type_list<Tail...>>::type next;
		typedef typename std::conditional_t<Comp<Head, next>::value, Head, next> type;
	};

	template<typename T, typename Data>
	struct remove_first_of;

	template<typename T, typename ...Tail>
	struct remove_first_of < T, type_list<Tail...>> {
		typedef typename type_list<Tail...> type;
	};

	template<typename T, typename ...Tail>
	struct remove_first_of < T, type_list<T, Tail...>> {
		typedef typename type_list<Tail...> type;
	};

	template<typename T, typename Head, typename ...Tail>
	struct remove_first_of<T, type_list<Head, Tail...>> {
		typedef typename concat<type_list<Head>, typename remove_first_of<T, type_list<Tail...>>::type>::type type;
	};

	template<template <typename T, typename U> class Comp, typename Data>
	struct ordered;

	template<template <typename T, typename U> class Comp, typename Head, typename ...Tail>
	struct ordered<Comp, type_list<Head, Tail...>> {
		typedef typename min_t<Comp, type_list<Head, Tail...>>::type mn;
		typedef typename remove_first_of<mn, type_list<Head, Tail...>>::type rest;
		typedef typename concat<type_list<mn>, typename ordered<Comp, rest>::type>::type type;
	};
	template<template <typename T, typename U> class Comp, typename T>
	struct ordered<Comp, type_list<T>> {
		typedef type_list<T> type;
	};
}