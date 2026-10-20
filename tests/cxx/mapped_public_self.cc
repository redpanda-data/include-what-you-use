//===--- mapped_public_self.cc - test input file for iwyu -----------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

// IWYU_ARGS: -Xiwyu --check_also=tests/cxx/mapped_public_self-d1.h -I .

// Tests a forward-declare use of a definition inside the public header that
// the definition maps to. The public header provides the definition by
// exporting the private header that holds it, so it needs neither a forward
// declaration nor an include of itself. IWYU used to look the definition up
// only by its public header, which is the file of the use and so never among
// its own includes, and suggested 'struct Defined;'.

#include "tests/cxx/mapped_public_self-d1.h"

void Use(Defined* d) {
  TakeDefined(d);
}

/**** IWYU_SUMMARY

(tests/cxx/mapped_public_self.cc has correct #includes/fwd-decls)

***** IWYU_SUMMARY */
