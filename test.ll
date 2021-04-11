;#[LLVM_IR]:
; ModuleID = 'top'
source_filename = "top"

@regs_ = global [4 x i32] zeroinitializer
@array_ = global [10 x i32] zeroinitializer
@nTacts = global [32 x i32] zeroinitializer
@IORegister = private unnamed_addr constant [8 x i8] c"RBX: = \00", align 1
@IOformat = private unnamed_addr constant [3 x i8] c"%d\00", align 1
@IORegister.1 = private unnamed_addr constant [8 x i8] c"RAX: = \00", align 1
@IOformat.2 = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1
@BenchmarkResult = private unnamed_addr constant [430 x i8] c"\0A\0A[Result of Benchmark]\0AVer 1.0.0\0ANumber of tacts of each instruction:\0A\09Push - %d\0A\09Push_R - %d\0A\09Pop_R - %d\0A\09Mov - %d\0A\09Mov_R - %d\0A\09Mov_RP - %d\0A\09Call - %d\0A\09Ret - %d\0A\09Exit - %d\0A\09Write - %d\0A\09Read - %d\0A\09Add - %d\0A\09Sub - %d\0A\09IMul - %d\0A\09IDiv - %d\0A\09Add_R - %d\0A\09Sub_R - %d\0A\09IMul_R - %d\0A\09IDiv_R - %d\0A\09Inc - %d\0A\09Dec - %d\0A\09Cmp - %d\0A\09Cmp_R - %d\0A\09Jmp - %d\0A\09JG - %d\0A\09JGE - %d\0A\09JL - %d\0A\09JLE - %d\0A\09JE - %d\0A\09JNE - %d\0A\0ANumber of jumps - %d\0A\0A[End!]\0A\0A\00", align 1

define i32 @main() {
entryBB:
  %0 = load i32, i32* getelementptr inbounds ([32 x i32], [32 x i32]* @nTacts, i32 0, i32 11), align 4
  %1 = add i32 %0, 1
  store i32 %1, i32* getelementptr inbounds ([32 x i32], [32 x i32]* @nTacts, i32 0, i32 11), align 4
  %2 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([8 x i8], [8 x i8]* @IORegister, i32 0, i32 0))
  %3 = call i32 (i8*, ...) @scanf(i8* getelementptr inbounds ([3 x i8], [3 x i8]* @IOformat, i32 0, i32 0), i32* getelementptr inbounds ([4 x i32], [4 x i32]* @regs_, i32 0, i32 1))
  %4 = load i32, i32* getelementptr inbounds ([32 x i32], [32 x i32]* @nTacts, i32 0, i32 3), align 4
  %5 = add i32 %4, 1
  store i32 %5, i32* getelementptr inbounds ([32 x i32], [32 x i32]* @nTacts, i32 0, i32 3), align 4
  %6 = load i32, i32* getelementptr inbounds ([4 x i32], [4 x i32]* @regs_, i32 0, i32 0), align 4
  store i32 1, i32* getelementptr inbounds ([4 x i32], [4 x i32]* @regs_, i32 0, i32 0), align 4
  %7 = load i32, i32* getelementptr inbounds ([32 x i32], [32 x i32]* @nTacts, i32 0, i32 7), align 4
  %8 = add i32 %7, 1
  store i32 %8, i32* getelementptr inbounds ([32 x i32], [32 x i32]* @nTacts, i32 0, i32 7), align 4
  %9 = load i32, i32* getelementptr inbounds ([32 x i32], [32 x i32]* @nTacts, i32 0, i32 31), align 4
  %10 = add i32 %9, 1
  store i32 %10, i32* getelementptr inbounds ([32 x i32], [32 x i32]* @nTacts, i32 0, i32 31), align 4
  call void @Function1()
  %11 = load i32, i32* getelementptr inbounds ([32 x i32], [32 x i32]* @nTacts, i32 0, i32 10), align 4
  %12 = add i32 %11, 1
  store i32 %12, i32* getelementptr inbounds ([32 x i32], [32 x i32]* @nTacts, i32 0, i32 10), align 4
  %13 = load i32, i32* getelementptr inbounds ([4 x i32], [4 x i32]* @regs_, i32 0, i32 0), align 4
  %14 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([8 x i8], [8 x i8]* @IORegister.1, i32 0, i32 0))
  %15 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @IOformat.2, i32 0, i32 0), i32 %13)
  %16 = load i32, i32* getelementptr inbounds ([32 x i32], [32 x i32]* @nTacts, i32 0, i32 9), align 4
  %17 = add i32 %16, 1
  store i32 %17, i32* getelementptr inbounds ([32 x i32], [32 x i32]* @nTacts, i32 0, i32 9), align 4
  %18 = load i32, i32* getelementptr inbounds ([32 x i32], [32 x i32]* @nTacts, i32 0, i32 0), align 4
  %19 = load i32, i32* getelementptr inbounds ([32 x i32], [32 x i32]* @nTacts, i32 0, i32 1), align 4
  %20 = load i32, i32* getelementptr inbounds ([32 x i32], [32 x i32]* @nTacts, i32 0, i32 2), align 4
  %21 = load i32, i32* getelementptr inbounds ([32 x i32], [32 x i32]* @nTacts, i32 0, i32 3), align 4
  %22 = load i32, i32* getelementptr inbounds ([32 x i32], [32 x i32]* @nTacts, i32 0, i32 4), align 4
  %23 = load i32, i32* getelementptr inbounds ([32 x i32], [32 x i32]* @nTacts, i32 0, i32 5), align 4
  %24 = load i32, i32* getelementptr inbounds ([32 x i32], [32 x i32]* @nTacts, i32 0, i32 6), align 4
  %25 = load i32, i32* getelementptr inbounds ([32 x i32], [32 x i32]* @nTacts, i32 0, i32 7), align 4
  %26 = load i32, i32* getelementptr inbounds ([32 x i32], [32 x i32]* @nTacts, i32 0, i32 8), align 4
  %27 = load i32, i32* getelementptr inbounds ([32 x i32], [32 x i32]* @nTacts, i32 0, i32 9), align 4
  %28 = load i32, i32* getelementptr inbounds ([32 x i32], [32 x i32]* @nTacts, i32 0, i32 10), align 4
  %29 = load i32, i32* getelementptr inbounds ([32 x i32], [32 x i32]* @nTacts, i32 0, i32 11), align 4
  %30 = load i32, i32* getelementptr inbounds ([32 x i32], [32 x i32]* @nTacts, i32 0, i32 12), align 4
  %31 = load i32, i32* getelementptr inbounds ([32 x i32], [32 x i32]* @nTacts, i32 0, i32 13), align 4
  %32 = load i32, i32* getelementptr inbounds ([32 x i32], [32 x i32]* @nTacts, i32 0, i32 14), align 4
  %33 = load i32, i32* getelementptr inbounds ([32 x i32], [32 x i32]* @nTacts, i32 0, i32 15), align 4
  %34 = load i32, i32* getelementptr inbounds ([32 x i32], [32 x i32]* @nTacts, i32 0, i32 16), align 4
  %35 = load i32, i32* getelementptr inbounds ([32 x i32], [32 x i32]* @nTacts, i32 0, i32 17), align 4
  %36 = load i32, i32* getelementptr inbounds ([32 x i32], [32 x i32]* @nTacts, i32 0, i32 18), align 4
  %37 = load i32, i32* getelementptr inbounds ([32 x i32], [32 x i32]* @nTacts, i32 0, i32 19), align 4
  %38 = load i32, i32* getelementptr inbounds ([32 x i32], [32 x i32]* @nTacts, i32 0, i32 20), align 4
  %39 = load i32, i32* getelementptr inbounds ([32 x i32], [32 x i32]* @nTacts, i32 0, i32 21), align 4
  %40 = load i32, i32* getelementptr inbounds ([32 x i32], [32 x i32]* @nTacts, i32 0, i32 22), align 4
  %41 = load i32, i32* getelementptr inbounds ([32 x i32], [32 x i32]* @nTacts, i32 0, i32 23), align 4
  %42 = load i32, i32* getelementptr inbounds ([32 x i32], [32 x i32]* @nTacts, i32 0, i32 24), align 4
  %43 = load i32, i32* getelementptr inbounds ([32 x i32], [32 x i32]* @nTacts, i32 0, i32 25), align 4
  %44 = load i32, i32* getelementptr inbounds ([32 x i32], [32 x i32]* @nTacts, i32 0, i32 26), align 4
  %45 = load i32, i32* getelementptr inbounds ([32 x i32], [32 x i32]* @nTacts, i32 0, i32 27), align 4
  %46 = load i32, i32* getelementptr inbounds ([32 x i32], [32 x i32]* @nTacts, i32 0, i32 28), align 4
  %47 = load i32, i32* getelementptr inbounds ([32 x i32], [32 x i32]* @nTacts, i32 0, i32 29), align 4
  %48 = load i32, i32* getelementptr inbounds ([32 x i32], [32 x i32]* @nTacts, i32 0, i32 30), align 4
  %49 = load i32, i32* getelementptr inbounds ([32 x i32], [32 x i32]* @nTacts, i32 0, i32 31), align 4
  %50 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([430 x i8], [430 x i8]* @BenchmarkResult, i32 0, i32 0), i32 %18, i32 %19, i32 %20, i32 %21, i32 %22, i32 %23, i32 %24, i32 %25, i32 %26, i32 %27, i32 %28, i32 %29, i32 %30, i32 %31, i32 %32, i32 %33, i32 %34, i32 %35, i32 %36, i32 %37, i32 %38, i32 %39, i32 %40, i32 %41, i32 %42, i32 %43, i32 %44, i32 %45, i32 %46, i32 %47, i32 %48, i32 %49)
  ret i32 0
}

define void @Function1() {
trueBB1:
  %0 = load i32, i32* getelementptr inbounds ([32 x i32], [32 x i32]* @nTacts, i32 0, i32 22), align 4
  %1 = add i32 %0, 1
  store i32 %1, i32* getelementptr inbounds ([32 x i32], [32 x i32]* @nTacts, i32 0, i32 22), align 4
  %2 = load i32, i32* getelementptr inbounds ([4 x i32], [4 x i32]* @regs_, i32 0, i32 1), align 4
  %resCmp = icmp ne i32 %2, 1
  %3 = load i32, i32* getelementptr inbounds ([32 x i32], [32 x i32]* @nTacts, i32 0, i32 30), align 4
  %4 = add i32 %3, 1
  store i32 %4, i32* getelementptr inbounds ([32 x i32], [32 x i32]* @nTacts, i32 0, i32 30), align 4
  %5 = load i32, i32* getelementptr inbounds ([32 x i32], [32 x i32]* @nTacts, i32 0, i32 31), align 4
  %6 = add i32 %5, 1
  store i32 %6, i32* getelementptr inbounds ([32 x i32], [32 x i32]* @nTacts, i32 0, i32 31), align 4
  br i1 %resCmp, label %trueBB2, label %falseBB2

trueBB2:                                          ; preds = %trueBB1
  %7 = load i32, i32* getelementptr ([32 x i32], [32 x i32]* @nTacts, i64 5, i32 15), align 4
  %8 = add i32 %7, 1
  store i32 %8, i32* getelementptr ([32 x i32], [32 x i32]* @nTacts, i64 5, i32 15), align 4
  %9 = load i32, i32* getelementptr inbounds ([4 x i32], [4 x i32]* @regs_, i32 0, i32 0), align 4
  %10 = load i32, i32* getelementptr inbounds ([4 x i32], [4 x i32]* @regs_, i32 0, i32 1), align 4
  %11 = mul i32 %9, %10
  store i32 %11, i32* getelementptr inbounds ([4 x i32], [4 x i32]* @regs_, i32 0, i32 0), align 4
  %12 = load i32, i32* getelementptr inbounds ([32 x i32], [32 x i32]* @nTacts, i32 0, i32 21), align 4
  %13 = add i32 %12, 1
  store i32 %13, i32* getelementptr inbounds ([32 x i32], [32 x i32]* @nTacts, i32 0, i32 21), align 4
  %14 = load i32, i32* getelementptr inbounds ([4 x i32], [4 x i32]* @regs_, i32 0, i32 1), align 4
  %15 = sub i32 %14, 1
  store i32 %15, i32* getelementptr inbounds ([4 x i32], [4 x i32]* @regs_, i32 0, i32 1), align 4
  %16 = load i32, i32* getelementptr inbounds ([32 x i32], [32 x i32]* @nTacts, i32 0, i32 7), align 4
  %17 = add i32 %16, 1
  store i32 %17, i32* getelementptr inbounds ([32 x i32], [32 x i32]* @nTacts, i32 0, i32 7), align 4
  %18 = load i32, i32* getelementptr inbounds ([32 x i32], [32 x i32]* @nTacts, i32 0, i32 31), align 4
  %19 = add i32 %18, 1
  store i32 %19, i32* getelementptr inbounds ([32 x i32], [32 x i32]* @nTacts, i32 0, i32 31), align 4
  call void @Function1()
  %20 = load i32, i32* getelementptr inbounds ([32 x i32], [32 x i32]* @nTacts, i32 0, i32 8), align 4
  %21 = add i32 %20, 1
  store i32 %21, i32* getelementptr inbounds ([32 x i32], [32 x i32]* @nTacts, i32 0, i32 8), align 4
  ret void

falseBB2:                                         ; preds = %trueBB1
  %22 = load i32, i32* getelementptr inbounds ([32 x i32], [32 x i32]* @nTacts, i32 0, i32 8), align 4
  %23 = add i32 %22, 1
  store i32 %23, i32* getelementptr inbounds ([32 x i32], [32 x i32]* @nTacts, i32 0, i32 8), align 4
  ret void
}

declare i32 @printf(i8*, ...)

declare i32 @scanf(i8*, ...)
