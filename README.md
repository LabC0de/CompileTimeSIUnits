# Compile Time SI Units



This header only library adds a SI unit template with automatic generation of derived units. It is based on sorted [^3][^5][^6][^7] compile time type sets [^1] and string concatination [^2] to make the units printable [^4] and avoid errors like `m * s` and `s * m` yielding different units. The expressions for derived units shorten themselves automatically. The oldest compatible C++ standard is C++17. 

> [!CAUTION]
> This is a hobby project and not extensively tested.

## Usage

You only need to include the Header `#include <si.h>` the namespace `literals` provides literals for the 22 derived SI units with special symbols as well as the 7 base units. The main template `unit<typename value_t, typename dimension_t>` takes a numeric `value_t` as first parameter. The `value_t` has to support the same Operators as the `unit` class. The unit class provides the following Operators and functions:

|Operator|Description|
|--------|-----------|
|`unit(const value_t&)`/`unit(value_t&&)`| Constructor |
|`unit(const unit&)`/`unit(unit&&)`| Constructor. `dimension_t` of the other unit has to be the same. |
|`explicit operator value_t() const`| Cast `unit` to `value_t`. Returns a copy of the contained value. |
|`const value_t& get_value() const`| Same as cast but returns a const reference. |
|`+()`|  |
|`-()`|  |
|`+(const unit&)`| Parameter has to have the same `dimension_t` as the object itself. |
|`-(const unit&)`| Parameter has to have the same `dimension_t` as the object itself. |
|`*(const unit&)`| Parameter can be any instance of the `unit` template. Return type is new instanciaion of unit template. |
|`/(const unit&)`| Parameter can be any instance of the `unit` template. Return type is new instanciaion of unit template. |
|`+=(const unit&)`| Parameter has to have the same `dimension_t` as the object itself. |
|`-=(const unit&)`| Parameter has to have the same `dimension_t` as the object itself. |
|`*=(const value_t&)`| Parameter has to be dimensionless. |
|`/=(const value_t&)`| Parameter has to be dimensionless. |
|`*(const value_t&)`| Parameter has to be dimensionless. |
|`/(const value_t&)`| Parameter has to be dimensionless. |

### Basic usage

```
using namespace si::literals;

length a = 2.0_m;
area b = a * a;
auto c = b * a; // auto => volume

```

[^1]: https://tech.jocodoma.com/2019/03/20/Creating-a-Compile-Time-Set-Data-Structure-in-CPP/
[^2]: https://stackoverflow.com/questions/6713420/c-convert-integer-to-string-at-compile-time
[^3]: https://playfulprogramming.com/posts/compile-time-quick-sort-using-c
[^4]: https://stackoverflow.com/questions/81870/is-it-possible-to-print-a-variables-type-in-standard-c/64490578#64490578
[^5]: https://quuxplusone.github.io/blog/2024/02/20/compile-time-sort-part-2/
[^6]: https://stackoverflow.com/questions/48723974/how-to-order-types-at-compile-time
[^7]: https://stackoverflow.com/questions/45288396/c-11-templates-alias-for-a-parameter-pack

