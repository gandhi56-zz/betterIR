; ModuleID = '/home/anshil/workspace/betterIR/tests/LivenessAnalysis/input/singleblk0.ll'
source_filename = "singleblk1.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

define dso_local i32 @main() {
entry:
  %0 = alloca i32, align 4
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  store i32 0, i32* %0, align 4
  store i32 0, i32* %1, align 4
  %5 = load i32, i32* %1, align 4
  %6 = add nsw i32 %5, 1
  store i32 %6, i32* %1, align 4
  store i32 %5, i32* %2, align 4
  %7 = load i32, i32* %1, align 4
  %8 = load i32, i32* %2, align 4
  %9 = icmp slt i32 %7, %8
  br i1 %9, label %a, label %b

a:                                                ; preds = %entry
  store i32 4, i32* %3, align 4
  store i32 3, i32* %4, align 4
  br label %c

b:                                                ; preds = %entry
  store i32 3, i32* %3, align 4
  store i32 4, i32* %4, align 4
  br label %c

c:                                                ; preds = %b, %a
  %10 = load i32, i32* %3, align 4
  ret i32 %10
}

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"Ubuntu clang version 10.0.1-++20200708122807+ef32c611aa2-1~exp1~20200707223407.61 "}
