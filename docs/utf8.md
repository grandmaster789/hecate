The standard library solution for doing character conversions has been deprecated in C++17 *WITH NO REPLACEMENT*. This means that we either need to roll my own, rely on OS-specific tooling or use a library.

Especially on windows, a lot of OS integration is done with UTF16, so conversions are actually needed, preferably in a to_string/to_wstring style.

It's very doable to roll my own, but it may be worth it to just go with the other two options

(currently we're using OS tooling)

Notable libraries
- https://github.com/unicode-org/icu (official unicode reference)
- https://github.com/simdutf/simdutf
- 