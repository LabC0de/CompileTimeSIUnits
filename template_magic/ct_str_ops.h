#pragma once
#include <cstdint>
#include <string_view>
#include <array>

/**
* CREDIT:
* https://stackoverflow.com/questions/6713420/c-convert-integer-to-string-at-compile-time
*/

namespace mgpi {
	template<std::intmax_t N, std::intmax_t base = 10>
	struct to_string_t {
		constexpr static unsigned buflen() noexcept {
			unsigned int len = N > 0 ? 1 : 2;
			for (auto n = N; n; len++) { n /= base; }
			return len;
		}
		char buffer[buflen()] = {};
		constexpr to_string_t() noexcept {
			auto ptr = buffer + buflen();
			*--ptr = '\0';
			if (N != 0) {
				for (auto n = N; n; n /= base) {
					*--ptr = "0123456789abcdef"[(N < 0 ? -1 : 1) * (n % base)];
				}
				if (N < 0) {
					*--ptr = '-';
				}
			}
			else {
				buffer[0] = '0';
			}
		}
		constexpr operator const char* () const { return buffer; }
		constexpr operator std::string_view() const { return std::string_view(buffer); }
	};

	template<std::intmax_t N>
	constexpr to_string_t<N> to_string;

	template <std::string_view const&... Strs>
	struct str_join {
		static constexpr size_t bufflen() {
			return (Strs.size() + ... + 0);
		}
		char buffer[bufflen() + 1] = {};
		constexpr str_join() noexcept {
			size_t i = 0;
			for (auto s : {Strs...}) {
				for (auto c : s) { buffer[i++] = c; }
			}
			buffer[i] = '\0';
		}
		constexpr operator const char* () const { return buffer; }
		constexpr operator std::string_view() const { return std::string_view(buffer); }
	};

	template <std::string_view const&... Strs>
	constexpr str_join<Strs...> str_join_v;
}