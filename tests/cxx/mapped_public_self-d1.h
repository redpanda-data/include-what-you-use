//===--- mapped_public_self-d1.h - test input file for iwyu ---------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "tests/cxx/mapped_public_self-i1.h"  // IWYU pragma: export

void TakeDefined(Defined*);

/**** IWYU_SUMMARY

(tests/cxx/mapped_public_self-d1.h has correct #includes/fwd-decls)

***** IWYU_SUMMARY */
