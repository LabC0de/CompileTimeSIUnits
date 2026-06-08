# Compile Time SI Units



This header only library adds a SI unit template with automatic generation of derived units. It is based on sorted [^3][^5] compile time type sets [^1] and string concatination [^2] to make the units printable and avoid errors like `m \\\* s` and `s \\\* m` yielding different units. The oldest compatible C++ standard is C++17.

[^1]: https://tech.jocodoma.com/2019/03/20/Creating-a-Compile-Time-Set-Data-Structure-in-CPP/
[^2]: https://stackoverflow.com/questions/6713420/c-convert-integer-to-string-at-compile-time
[^3]: https://playfulprogramming.com/posts/compile-time-quick-sort-using-c
[^4]: https://stackoverflow.com/questions/81870/is-it-possible-to-print-a-variables-type-in-standard-c/64490578#64490578
[^5]: https://quuxplusone.github.io/blog/2024/02/20/compile-time-sort-part-2/
[^6]: https://stackoverflow.com/questions/48723974/how-to-order-types-at-compile-time
[^7]: https://stackoverflow.com/questions/45288396/c-11-templates-alias-for-a-parameter-pack

