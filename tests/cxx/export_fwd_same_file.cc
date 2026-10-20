//===--- export_fwd_same_file.cc - test input file for iwyu ---------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

// IWYU_ARGS: -Xiwyu --check_also=tests/cxx/export_fwd_same_file-d1.h -I .

// Tests that a header which exports a forward declaration and also uses it
// is not told to include itself. Rule A7 points a forward-declare use at an
// exported redeclaration so the exporting file stays included, but when that
// redeclaration is in the file of the use, no include is needed.

#include "tests/cxx/export_fwd_same_file-d1.h"

// IWYU: Exported needs a declaration, but does not provide or directly #include one.
void Use(Exported* e) {
  TakeExported(e);
}

/**** IWYU_SUMMARY

(tests/cxx/export_fwd_same_file.cc has correct #includes/fwd-decls)

***** IWYU_SUMMARY */
