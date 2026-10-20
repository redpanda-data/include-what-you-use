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
  Upstream PR
  [#2139](https://github.com/include-what-you-use/include-what-you-use/pull/2139).

- **Treat any enclosing destructor as a method body** (plus its test): a
  member's destructor instantiated from the enclosing class's implicit
  destructor reported uses of its template argument outside a method body,
  so a nested class used that way (for example `absl::FixedArray<Nested>` or
  `std::variant<Nested>`) got the invalid suggestion `struct Outer::Nested;`.
  Fixes the reduced case on upstream
  [#1707](https://github.com/include-what-you-use/include-what-you-use/issues/1707).
  Upstream PR
  [#2142](https://github.com/include-what-you-use/include-what-you-use/pull/2142).

- **Report the object type of an unresolved member call** (plus its test): a
  call to a member template with dependent template arguments in a template,
  such as `cache.local().with_node_client<proto_t>(...)` or
  `vlog(kwire.debug, ...)`, did not count as a full use of the object's type,
  so IWYU replaced the object's header with a forward declaration that does
  not compile. Upstream issue
  [#2146](https://github.com/include-what-you-use/include-what-you-use/issues/2146),
  PR [#2147](https://github.com/include-what-you-use/include-what-you-use/pull/2147).

- **Don't point a forward-declare use at an export in its own file** (plus
  its test): a header that exports a forward declaration and then uses it,
  like a `fwd.h` with `begin_exports`, was told to include itself.

- **Find a definition in the public header the use is in** (plus its test):
  a public header that provides a definition by exporting its private
  header, like `base/outcome.h` for its outcome headers, looked the
  definition up only by its own name, so it was told to forward-declare the
  type, or to include itself.

- **Don't count a pack expansion as one template argument** (plus its
  test): a specialization written with a pack expansion, like
  `absl::flat_hash_map<Args...>` in a partial specialization, was taken to
  use the default arguments of every parameter after the first, and IWYU
  aborted with "There should be a redecl specifying the default arg" when a
  parameter had none. Upstream issue
  [#2148](https://github.com/include-what-you-use/include-what-you-use/issues/2148),
  PR [#2149](https://github.com/include-what-you-use/include-what-you-use/pull/2149).

Except for the #2136 fix, which adds the includes it requires, output matched
stock IWYU on every file compared.
