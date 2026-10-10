# Fork notes

This is a thin fork of
[include-what-you-use](https://github.com/include-what-you-use/include-what-you-use)
that carries a small set of patches on top of an upstream release branch. The
patches are required for IWYU to run effectively on the Redpanda codebase
(seastar, coroutine-heavy C++23, Boost.Test, and a Bazel build with ~300 header
search paths). Without them some files take more than 10 minutes, or never
finish.

The patches are kept as discrete commits so they can be rebased onto new
upstream branches and dropped once equivalent fixes land upstream.

## Base

Upstream `clang_23` (IWYU 0.27, for Clang 23).

## Patches

- **Avoid repeated template specialization traversal** (plus its test):
  cherry-picked from upstream PR
  [#2129](https://github.com/include-what-you-use/include-what-you-use/pull/2129),
  which fixes the exponential traversal of nested alias templates reported in
  [#2131](https://github.com/include-what-you-use/include-what-you-use/issues/2131).
  On `group_mirroring_task.cc`: more than 600s before, 25s after.

- **Cache quoted-include lookups in FileTransitivelyIncludes**: on every symbol
  use, IWYU converted every file in the translation unit to its quoted include
  name, which costs a `stat()` and a linear scan of all header search paths
  each time. Boost.Test assertion macros generate many symbol uses, so those
  files were hit hardest. This patch memoizes the conversion and indexes files
  by quoted name. A synthetic test with 400 `BOOST_REQUIRE_EQUAL` calls goes
  from 191.7s to 3.7s (`clang -fsyntax-only` takes 1.6s). Not yet filed upstream.

- **Report args held by specializations fully used in templates** (plus its
  test): inside a template instantiation, a cast to `Box<T>*` or a member
  access through one needs `Box<T>` complete, and so `T` when `Box<T>` holds
  it by value. IWYU did not report `T`, so it suggested forward-declaring a
  type the code needs complete, which fails to compile. Seastar's
  `lw_shared_ptr<T>` hits this on every access. Fixes upstream
  [#2136](https://github.com/include-what-you-use/include-what-you-use/issues/2136).

- **Map only macro-dependent declarations to the includer that defines
  the macro** (plus its test): a header that defines a macro before
  including another, such as `#define LZ4F_STATIC_LINKING_ONLY` before
  `<lz4frame.h>`, became the only allowed provider of everything in the
  included header, so IWYU removed direct includes of `<lz4frame.h>` from
  files that use its ordinary API. Now only the declarations in lines that
  depend on the macro map to the includer. A header whose existing
  declarations change with the macro, like `<picojson.h>` with
  `PICOJSON_USE_INT64`, stays mapped as a whole. Also fixes upstream
  [#1370](https://github.com/include-what-you-use/include-what-you-use/issues/1370).
  Not yet filed upstream.

- **Link the TargetParser component directly**: build fix so IWYU links
  against an LLVM built with `BUILD_SHARED_LIBS=ON`, where
  `getDefaultTargetTriple()` no longer arrives transitively.

Except for the #2136 fix, which adds the includes it requires, output matched
stock IWYU on every file compared.
