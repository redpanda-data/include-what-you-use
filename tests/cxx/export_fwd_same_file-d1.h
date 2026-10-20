//===--- export_fwd_same_file-d1.h - test input file for iwyu -------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

// IWYU pragma: begin_exports
class Exported;
// IWYU pragma: end_exports

void TakeExported(Exported*);

/**** IWYU_SUMMARY

(tests/cxx/export_fwd_same_file-d1.h has correct #includes/fwd-decls)

***** IWYU_SUMMARY */
