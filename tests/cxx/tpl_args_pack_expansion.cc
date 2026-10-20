//===--- tpl_args_pack_expansion.cc - test input file for iwyu ------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

// Tests template specializations whose written arguments include a pack
// expansion. 'Pair<Ts...>' writes one argument, but the expansion can stand
// for any number of them, so it doesn't mean that the remaining parameters
// take their default arguments. IWYU used to look for a redeclaration giving
// a default to the second parameter of Pair and abort when it found none.

template <typename A, typename B>
struct Pair {};

template <typename A, typename B, typename C = int>
struct Triple {};

template <typename T>
struct Trait;

template <typename... Ts>
struct Trait<Pair<Ts...>> {};

template <typename... Ts>
void TakePair(Pair<Ts...>);

template <typename... Ts>
using PairAlias = Pair<Ts...>;

template <typename... Ts>
struct Holder {
  Triple<Ts...>* p;
};

Holder<char, long> two_args;
Holder<char, long, short> three_args;

/**** IWYU_SUMMARY

(tests/cxx/tpl_args_pack_expansion.cc has correct #includes/fwd-decls)

***** IWYU_SUMMARY */
