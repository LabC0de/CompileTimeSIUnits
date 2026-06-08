#pragma once
#include <string_view>
#include <tuple>
#include <type_traits>
#include <string_view>
#include <utility>
#include <cstdint>
#include "template_magic/type_set.h"
#include "template_magic/ct_str_ops.h"

/**
* CREDIT:
* https://playfulprogramming.com/posts/compile-time-quick-sort-using-c
*/
namespace si {
	using namespace mgpi;
	template <typename U, typename V>
	struct cmpgt {
		static constexpr bool value = U::unit_tag > V::unit_tag;
	};

	template <typename U, typename V>
	struct cmple {
		static constexpr bool value = U::unit_tag <= V::unit_tag;
	};

	template <typename Scalar, int64_t _exponent>
	struct pow {
	private:
		static constexpr std::string_view caret = "^";
		static constexpr std::string_view strexp = to_string<_exponent>;

		static constexpr int64_t sqr(int64_t a) noexcept {
			return a * a;
		}
		static constexpr int64_t abs(int64_t a) noexcept {
			return a < 0 ? -a : a;
		}
		static constexpr int64_t sgn(int64_t a) noexcept {
			return a < 0 ? -1 : 1;
		}
		static constexpr int64_t _pow(int64_t x, int64_t n) noexcept {
			return n == 0 ? 1 : sqr(_pow(x, n / 2)) * (n % 2 == 0 ? 1 : x);
		}
	public:
		template<typename T>
		static constexpr bool contains() noexcept {
			if constexpr ((_exponent < 0 && T::exponent < 0 && T::exponent >= _exponent) || (_exponent > 0 && T::exponent > 0 && T::exponent <= _exponent)) {
				return std::is_same<typename T::sitype, typename Scalar::sitype>::value;
			}
			return false;
		}

		typedef typename Scalar::sitype sitype;
		typedef typename std::conditional_t<_exponent == -1, Scalar, pow<Scalar, -_exponent>> inverse;
		static constexpr int64_t unit_tag = sgn(_exponent) * _pow(Scalar::unit_tag, abs(_exponent));
		static constexpr int64_t exponent = _exponent;
		static constexpr std::string_view unit = str_join_v<Scalar::unit, caret, strexp>;
		static constexpr std::string_view unit_symbol = str_join_v<Scalar::unit_symbol, caret, strexp>;
		static constexpr std::string_view symbol = str_join_v<Scalar::symbol, caret, strexp>;
		static constexpr std::string_view dimension = str_join_v<Scalar::dimension, caret, strexp>;
	};

	template<typename Data>
	struct shorten;

	template<typename Head, typename ...Tail>
	struct shorten <type_list<Head, Tail...>> {
		template<typename X, typename Y>
		static constexpr int64_t sum() noexcept {
			return (std::is_same_v<X::sitype, Y::sitype> *Y::exponent);
		}
		static constexpr int64_t extract() noexcept {
			return (sum<Head, Tail>() + ...) + Head::exponent;
		}
		static constexpr auto filter() noexcept {
			constexpr auto filtered = std::tuple_cat(
				std::conditional_t<
				!std::is_same_v<Head::sitype, Tail::sitype>,
				std::tuple<Tail>,
				std::tuple<>
				>{}...
			);
			return std::apply([](auto... xs) { return type_list<decltype(xs)...>{}; }, filtered);
		}
		static constexpr int64_t restmp = extract();
		typedef typename Head::sitype rtype;
		typedef typename std::conditional_t<restmp == 0, type_list<>, std::conditional_t<restmp == 1, type_list<Head>, type_list<pow<rtype, restmp>>>> intermediate;
		typedef typename concat<intermediate, typename shorten<decltype(filter())>::result>::type result;

	};

	template<typename Head>
	struct shorten<type_list<Head>> {
		static constexpr int64_t restmp = Head::exponent;
		typedef typename Head::sitype rtype;
		typedef typename std::conditional_t<restmp == 0, type_list<>, std::conditional_t<restmp == 1, type_list<Head>, type_list<pow<rtype, restmp>>>> result;
	};

	template<>
	struct shorten<type_list<>> {
		typedef type_list<> result;
	};

	template<typename ...Scalars>
	struct unit_identifiers {
		static constexpr std::string_view unit = "";
		static constexpr std::string_view unit_symbol = str_join_v<Scalars::unit_symbol...>;
		static constexpr std::string_view symbol = str_join_v<Scalars::symbol...>;
		static constexpr std::string_view dimension = str_join_v<Scalars::dimension...>;
	};

	template <typename ...Scalars>
	struct product {
	private:
		static constexpr std::string_view space = " ";
		typedef typename ordered<cmpgt, typename shorten<type_list<Scalars...>>::result>::type scalar_list;
		template<typename T>
		struct _scontains {
			static constexpr bool value = std::disjunction_v<Scalars::template contains<T>()...>;
		};

		template<typename Head, typename ...Tail>
		struct _scontains<product<Head, Tail...>> {
			static constexpr bool value = _scontains<Head>::value && _scontains<product<Tail...>::result>::value;
		};
	public:
		typedef typename std::conditional_t<scalar_list::size == 1,
			typename first<scalar_list>::type,
			typename scalar_list::template apply<product>> result;
		typedef typename product<typename Scalars::inverse...>::result inverse;

		static constexpr int64_t mul() {
			return (Scalars::unit_tag * ...);
		}
		//static constexpr result self() noexcept { return {}; }
		template<typename T>
		static constexpr bool contains() noexcept {
			return _scontains<T>::value;
		}

		static constexpr int64_t unit_tag = mul();
		static constexpr int64_t exponent = 1;
		//static constexpr std::string_view unit = "";
		//static constexpr std::string_view unit_symbol = str_join_v<Scalars::unit_symbol...>;
		//static constexpr std::string_view symbol = str_join_v<Scalars::symbol...>;
		//static constexpr std::string_view dimension = str_join_v<Scalars::dimension...>;

		static constexpr std::string_view unit = unit_identifiers<Scalars...>::unit;
		static constexpr std::string_view unit_symbol = unit_identifiers<Scalars...>::unit_symbol;
		static constexpr std::string_view symbol = unit_identifiers<Scalars...>::symbol;
		static constexpr std::string_view dimension = unit_identifiers<Scalars...>::dimension;
	};

	struct time {
		typedef time sitype;
		typedef pow<sitype, -1> inverse;
		static constexpr int64_t unit_tag = 2;
		static constexpr int64_t exponent = 1;
		static constexpr std::string_view unit = "second";
		static constexpr std::string_view unit_symbol = "s";
		static constexpr std::string_view symbol = "t";
		static constexpr std::string_view dimension = "T";

		template<typename T>
		static constexpr bool contains() noexcept {
			return std::is_same<T, sitype>::value;
		}
	};

	struct length {
		typedef length sitype;
		typedef pow<sitype, -1> inverse;
		static constexpr int64_t unit_tag = 3;
		static constexpr int64_t exponent = 1;
		static constexpr std::string_view unit = "meter";
		static constexpr std::string_view unit_symbol = "m";
		static constexpr std::string_view symbol = "l";
		static constexpr std::string_view dimension = "L";

		template<typename T>
		static constexpr bool contains() noexcept {
			return std::is_same<T, sitype>::value;
		}
	};

	struct mass {
		typedef mass sitype;
		typedef pow<sitype, -1> inverse;
		static constexpr int64_t unit_tag = 5;
		static constexpr int64_t exponent = 1;
		static constexpr std::string_view unit = "kilogramm";
		static constexpr std::string_view unit_symbol = "kg";
		static constexpr std::string_view symbol = "m";
		static constexpr std::string_view dimension = "M";

		template<typename T>
		static constexpr bool contains() noexcept {
			return std::is_same<T, sitype>::value;
		}
	};

	struct current {
		typedef current sitype;
		typedef pow<sitype, -1> inverse;
		static constexpr int64_t unit_tag = 7;
		static constexpr int64_t exponent = 1;
		static constexpr std::string_view unit = "ampere";
		static constexpr std::string_view unit_symbol = "A";
		static constexpr std::string_view symbol = "I";
		static constexpr std::string_view dimension = "I";

		template<typename T>
		static constexpr bool contains() noexcept {
			return std::is_same<T, sitype>::value;
		}
	};

	struct temperature {
		typedef temperature sitype;
		typedef pow<sitype, -1> inverse;
		static constexpr int64_t unit_tag = 11;
		static constexpr int64_t exponent = 1;
		static constexpr std::string_view unit = "kelvin";
		static constexpr std::string_view unit_symbol = "K";
		static constexpr std::string_view symbol = "T";
		static constexpr std::string_view dimension = "theta";

		template<typename T>
		static constexpr bool contains() noexcept {
			return std::is_same<T, sitype>::value;
		}
	};

	struct substance {
		typedef substance sitype;
		typedef pow<sitype, -1> inverse;
		static constexpr int64_t unit_tag = 13;
		static constexpr int64_t exponent = 1;
		static constexpr std::string_view unit = "moles";
		static constexpr std::string_view unit_symbol = "mol";
		static constexpr std::string_view symbol = "n";
		static constexpr std::string_view dimension = "N";

		template<typename T>
		static constexpr bool contains() noexcept {
			return std::is_same<T, sitype>::value;
		}
	};

	struct luminosity {
		typedef luminosity sitype;
		typedef pow<sitype, -1> inverse;
		static constexpr int64_t unit_tag = 17;
		static constexpr int64_t exponent = 1;
		static constexpr std::string_view unit = "candela";
		static constexpr std::string_view unit_symbol = "cd";
		static constexpr std::string_view symbol = "Iv";
		static constexpr std::string_view dimension = "J";

		template<typename T>
		static constexpr bool contains() noexcept {
			return std::is_same<T, sitype>::value;
		}
	};

	

	template<>
	struct pow<length, 2> {
		typedef typename length sitype;
		typedef pow<length, -2> inverse;
		static constexpr int64_t unit_tag = 9;
		static constexpr int64_t exponent = 2;
		static constexpr std::string_view unit = "square meter";
		static constexpr std::string_view unit_symbol = "m^2";
		static constexpr std::string_view symbol = "A";
		static constexpr std::string_view dimension = "L^2";

		template<typename T>
		static constexpr bool contains() noexcept {
			if constexpr ((exponent < 0 && T::exponent < 0 && T::exponent >= exponent) || (exponent > 0 && T::exponent > 0 && T::exponent <= exponent)) {
				return std::is_same<typename T::sitype, typename length::sitype>::value;
			}
			return false;
		}
	};

	template<>
	struct pow<length, 3> {
		typedef typename length sitype;
		typedef pow<length, -3> inverse;
		static constexpr int64_t unit_tag = 27;
		static constexpr int64_t exponent = 3;
		static constexpr std::string_view unit = "cubic meter";
		static constexpr std::string_view unit_symbol = "m^3";
		static constexpr std::string_view symbol = "V";
		static constexpr std::string_view dimension = "L^3";

		template<typename T>
		static constexpr bool contains() noexcept {
			if constexpr ((exponent < 0 && T::exponent < 0 && T::exponent >= exponent) || (exponent > 0 && T::exponent > 0 && T::exponent <= exponent)) {
				return std::is_same<typename T::sitype, typename length::sitype>::value;
			}
			return false;
		}
	};

	template<>
	struct pow<time, -1> {
		typedef typename time sitype;
		typedef typename time inverse;
		static constexpr int64_t unit_tag = -2;
		static constexpr int64_t exponent = -1;
		static constexpr std::string_view unit = "hertz";
		static constexpr std::string_view unit_symbol = "Hz";
		static constexpr std::string_view symbol = "f";
		static constexpr std::string_view dimension = "T^-1";

		template<typename T>
		static constexpr bool contains() noexcept {
			if constexpr ((exponent < 0 && T::exponent < 0 && T::exponent >= exponent) || (exponent > 0 && T::exponent > 0 && T::exponent <= exponent)) {
				return std::is_same<typename T::sitype, typename time::sitype>::value;
			}
			return false;
		}
	};


	using area = pow<length, 2>;
	using volume = pow<length, 3>;
	using frequency = pow<time, -1>;

	template<>
	struct unit_identifiers <mass, length, pow<time, -2>> {
		static constexpr std::string_view unit = "newton";
		static constexpr std::string_view unit_symbol = "N";
		static constexpr std::string_view symbol = "F";
		static constexpr std::string_view dimension = "MLT^-2";
	};

	template<>
	struct unit_identifiers<mass, pow<length, -1>, pow<time, -2>> {
		static constexpr std::string_view unit = "pascal";
		static constexpr std::string_view unit_symbol = "Pa";
		static constexpr std::string_view symbol = "p";
		static constexpr std::string_view dimension = "ML^-1T^-2";
	};

	template<>
	struct unit_identifiers<pow<length, 2>, mass, pow<time, -2>> {
		static constexpr std::string_view unit = "joule";
		static constexpr std::string_view unit_symbol = "J";
		static constexpr std::string_view symbol = "E";
		static constexpr std::string_view dimension = "ML^2T^-2";
	};

	template<>
	struct unit_identifiers<pow<length, 2>, mass, pow<time, -3>> {
		static constexpr std::string_view unit = "watt";
		static constexpr std::string_view unit_symbol = "W";
		static constexpr std::string_view symbol = "P";
		static constexpr std::string_view dimension = "ML^2T^-3";
	};

	template<>
	struct unit_identifiers<current, time> {
		static constexpr std::string_view unit = "coloumb";
		static constexpr std::string_view unit_symbol = "C";
		static constexpr std::string_view symbol = "Q";
		static constexpr std::string_view dimension = "IT";
	};

	template<>
	struct unit_identifiers<area, mass, pow<current, -1>, pow<time, -3>> {
		static constexpr std::string_view unit = "volt";
		static constexpr std::string_view unit_symbol = "V";
		static constexpr std::string_view symbol = "U";
		static constexpr std::string_view dimension = "ML^2T^-3I^-1";
	};

	template<>
	struct unit_identifiers<pow<current, 2>, pow<time, 4>, pow<mass, -1>, pow<length, -2>> {
		static constexpr std::string_view unit = "farad";
		static constexpr std::string_view unit_symbol = "F";
		static constexpr std::string_view symbol = "C";
		static constexpr std::string_view dimension = "M^-1L^-2T^4I^2";
	};

	template<>
	struct unit_identifiers<area, mass, pow<time, -3>, pow<current, -2>> {
		static constexpr std::string_view unit = "ohm";
		static constexpr std::string_view unit_symbol = "omega";
		static constexpr std::string_view symbol = "R";
		static constexpr std::string_view dimension = "ML^2T^-3I^-2";
	};

	template<>
	struct unit_identifiers<pow<current, 2>, pow<time, 3>, mass::inverse, area::inverse> {
		static constexpr std::string_view unit = "siemens";
		static constexpr std::string_view unit_symbol = "S";
		static constexpr std::string_view symbol = "G";
		static constexpr std::string_view dimension = "M^-1L^-2T^-3I^2";
	};

	template<>
	struct unit_identifiers<area, mass, pow<time, -2>, pow<current, -1>> {
		static constexpr std::string_view unit = "weber";
		static constexpr std::string_view unit_symbol = "Wb";
		static constexpr std::string_view symbol = "phi";
		static constexpr std::string_view dimension = "ML^2T^-2I^-1";
	};

	template<>
	struct unit_identifiers<mass, pow<time, -2>, pow<current, -1>> {
		static constexpr std::string_view unit = "tesla";
		static constexpr std::string_view unit_symbol = "T";
		static constexpr std::string_view symbol = "B";
		static constexpr std::string_view dimension = "MT^-2I^-1";
	};

	template<>
	struct unit_identifiers<area, mass, pow<time, -2>, pow<current, -2>> {
		static constexpr std::string_view unit = "henry";
		static constexpr std::string_view unit_symbol = "H";
		static constexpr std::string_view symbol = "L";
		static constexpr std::string_view dimension = "ML^2T^-2I^-2";
	};

	template<>
	struct unit_identifiers<luminosity, pow<length, -2>> {
		static constexpr std::string_view unit = "lux";
		static constexpr std::string_view unit_symbol = "lx";
		static constexpr std::string_view symbol = "Ev";
		static constexpr std::string_view dimension = "JL^-2";
	};

	template<>
	struct unit_identifiers<area, pow<time, -2>> {
		static constexpr std::string_view unit = "gray";
		static constexpr std::string_view unit_symbol = "Gy";
		static constexpr std::string_view symbol = "D";
		static constexpr std::string_view dimension = "L^2T^-2";
	};

	template<>
	struct unit_identifiers<substance, frequency> {
		static constexpr std::string_view unit = "katal";
		static constexpr std::string_view unit_symbol = "kat";
		static constexpr std::string_view symbol = "z";
		static constexpr std::string_view dimension = "NT^-1";
	};



	using pressure = product <mass, pow<length, -1>, pow<time, -2>>;
	using force = product <mass, length, pow<time, -2>>;
	using energy = product<pow<length, 2>, mass, pow<time, -2>>;
	using power = product<pow<length, 2>, mass, pow<time, -3>>;
	using charge = product<current, time>;
	using potential = product<area, mass, pow<current, -1>, pow<time, -3>>;
	using capacity = product<pow<current, 2>, pow<time, 4>, pow<mass, -1>, pow<length, -2>>;
	using resistance = product<area, mass, pow<time, -3>, pow<current, -2>>;
	using conductivity = product<pow<current, 2>, pow<time, 3>, mass::inverse, area::inverse>;
	using magnetic_flux = product<area, mass, pow<time, -2>, pow<current, -1>>;
	using magnetic_flux_density = product<mass, pow<time, -2>, pow<current, -1>>;
	using inductivity = product<area, mass, pow<time, -2>, pow<current, -2>>;
	using activity = product<substance, frequency>;
	using energy_dose = product<area, pow<time, -2>>;
	using illuminance = product<luminosity, pow<length, -2>>;

	template <typename A, typename B>
	struct multiply {
		typename typedef product<A, B>::result result;
	};

	template <typename ...A, typename B>
	struct multiply<product<A...>, B> {
		typename typedef product<A..., B>::result result;
	};

	template <typename A, typename ...B>
	struct multiply<A, product<B...>> {
		typename typedef product<A, B...>::result result;
	};

	template <typename ...A, typename ...B>
	struct multiply<product<A...>, product<B...>> {
		typename typedef product<A..., B...>::result result;
	};

	template <typename A, typename B>
	struct divide {
		typename typedef multiply<A, typename B::inverse>::result result;
	};

	template <typename ...A, typename B>
	struct divide<product<A...>, B> {
		typename typedef multiply<product<A...>, typename B::inverse>::result result;
	};

	template <typename A, typename ...B>
	struct divide<A, product<B...>> {
		typename typedef multiply<A, typename product<B...>::inverse>::result result;
	};

	template <typename ...A, typename ...B>
	struct divide<product<A...>, product<B...>> {
		typename typedef multiply<product<A...>, typename product<B...>::inverse>::result result;
	};

	template<typename value_t, typename dimension_t>
	class unit {
		value_t data;
	public:
		static constexpr std::string_view unit_name = dimension_t::unit;
		static constexpr std::string_view unit_symbol = dimension_t::unit_symbol;
		static constexpr std::string_view symbol = dimension_t::symbol;
		static constexpr std::string_view dimension = dimension_t::dimension;

		unit<value_t, dimension_t>(const unit<value_t, dimension_t>& o) : data(o.data) {};
		unit<value_t, dimension_t>(value_t v) : data(v) {};
		inline explicit operator value_t() const { return data; }
		unit<value_t, dimension_t>& operator=(value_t v) {
			data = v;
			return *this;
		};
		unit<value_t, dimension_t>& operator=(const unit<value_t, dimension_t>& o) {
			data = o.data;
			return *this;
		};
		unit<value_t, dimension_t> operator+() const { return unit<value_t, dimension_t>(+data); }
		unit<value_t, dimension_t> operator-() const { return unit<value_t, dimension_t>(-data); }
		unit<value_t, dimension_t> operator+(const unit<value_t, dimension_t>& o) const { return unit<value_t, dimension_t>(data + o.data); }
		unit<value_t, dimension_t> operator-(const unit<value_t, dimension_t>& o) const { return unit<value_t, dimension_t>(data - o.data); }

		template<typename odim_t>
		unit<value_t, typename multiply<dimension_t, odim_t>::result> operator*(const unit<value_t, odim_t>& o) const { 
			return unit<value_t, typename multiply<dimension_t, odim_t>::result>(data * (value_t)o); 
		}
		template<typename odim_t>
		unit<value_t, typename divide<dimension_t, odim_t>::result> operator/(const unit<value_t, odim_t>& o) const { 
			return unit<value_t, typename divide<dimension_t, odim_t>::result>(data / (value_t)o); 
		}
		value_t operator/(const unit<value_t, dimension_t>& o) const { return data / o.data; }

		unit<value_t, dimension_t> operator+=(const unit<value_t, dimension_t>& o) {
			data += o.data;
			return *this;
		}
		unit<value_t, dimension_t> operator-=(const unit<value_t, dimension_t>& o) { 
			data -= o.data;
			return *this;
		}
		unit<value_t, dimension_t> operator*=(const unit<value_t, dimension_t>& o) {
			data *= o.data;
			return *this;
		}
		unit<value_t, dimension_t> operator/=(const unit<value_t, dimension_t>& o) {
			data /= o.data;
			return *this;
		}
		friend std::ostream& operator<< (std::ostream& stream, const unit<value_t, dimension_t>& unt) {
			stream << unt.data << dimension_t::unit_symbol;
			return stream;
		}
	};
	namespace literals {
		unit<long double, length> operator""_m(long double v) {
			return unit<long double, length>(v);
		}
		unit<long double, time> operator""_s(long double v) {
			return unit<long double, time>(v);
		}
		unit<long double, mass> operator""_kg(long double v) {
			return unit<long double, mass>(v);
		}
		unit<long double, current> operator""_A(long double v) {
			return unit<long double, current>(v);
		}
		unit<long double, temperature> operator""_K(long double v) {
			return unit<long double, temperature>(v);
		}
		unit<long double, luminosity> operator""_cd(long double v) {
			return unit<long double, luminosity>(v);
		}
		unit<long double, substance> operator""_mol(long double v) {
			return unit<long double, substance>(v);
		}
		unit<long double, frequency> operator""_hz(long double v) {
			return unit<long double, frequency>(v);
		}
		unit<long double, area> operator""_m2(long double v) {
			return unit<long double, area>(v);
		}
		unit<long double, volume> operator""_m3(long double v) {
			return unit<long double, volume>(v);
		}
		unit<long double, pressure> operator""_pa(long double v) {
			return unit<long double, pressure>(v);
		}
		unit<long double, force> operator""_N(long double v) {
			return unit<long double, force>(v);
		}
		unit<long double, energy> operator""_J(long double v) {
			return unit<long double, energy>(v);
		}
		unit<long double, power> operator""_W(long double v) {
			return unit<long double, power>(v);
		}
		unit<long double, charge> operator""_C(long double v) {
			return unit<long double, charge>(v);
		}
		unit<long double, potential> operator""_V(long double v) {
			return unit<long double, potential>(v);
		}
		unit<long double, capacity> operator""_F(long double v) {
			return unit<long double, capacity>(v);
		}
		unit<long double, resistance> operator""_omega(long double v) {
			return unit<long double, resistance>(v);
		}
		unit<long double, conductivity> operator""_S(long double v) {
			return unit<long double, conductivity>(v);
		}
		unit<long double, magnetic_flux> operator""_Wb(long double v) {
			return unit<long double, magnetic_flux>(v);
		}
		unit<long double, magnetic_flux_density> operator""_T(long double v) {
			return unit<long double, magnetic_flux_density>(v);
		}
		unit<long double, inductivity> operator""_H(long double v) {
			return unit<long double, inductivity>(v);
		}
		unit<long double, activity> operator""_kat(long double v) {
			return unit<long double, activity>(v);
		}
		unit<long double, energy_dose> operator""_Gv(long double v) {
			return unit<long double, energy_dose>(v);
		}
		unit<long double, energy_dose> operator""_Sv(long double v) {
			return unit<long double, energy_dose>(v);
		}
		unit<long double, illuminance> operator""_lx(long double v) {
			return unit<long double, illuminance>(v);
		}
	}
}