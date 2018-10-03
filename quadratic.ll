@a = common global double 0.0e+0, align 8
@b = common global double 0.0e+0, align 8
@c = common global double 0.0e+0, align 8
@x1 = common global double 0.0e+0, align 8
@x2 = common global double 0.0e+0, align 8

@a.format = private constant [4 x i8] c"a: \00"
@b.format = private constant [4 x i8] c"b: \00"
@c.format = private constant [4 x i8] c"c: \00"
@input = private constant [4 x i8] c"%lf\00"
@x1.format = private constant [10 x i8] c"x1 = %lf\0A\00"
@x2.format = private constant [10 x i8] c"x2 = %lf\0A\00"

define i32 @main() {
entry:
  %0 = call i32 (i8*, ...) @printf(i8* getelementptr ([4 x i8], [4 x i8]* @a.format, i32 0, i32 0))
  %1 = call i32 (i8*, ...) @scanf(i8* getelementptr ([4 x i8], [4 x i8]* @input, i32 0, i32 0), double* nonnull @a)
  
  %2 = call i32 (i8*, ...) @printf(i8* getelementptr ([4 x i8], [4 x i8]* @b.format, i32 0, i32 0))
  %3 = call i32 (i8*, ...) @scanf(i8* getelementptr ([4 x i8], [4 x i8]* @input, i32 0, i32 0), double* nonnull @b)
  
  %4 = call i32 (i8*, ...) @printf(i8* getelementptr ([4 x i8], [4 x i8]* @c.format, i32 0, i32 0))
  %5 = call i32 (i8*, ...) @scanf(i8* getelementptr ([4 x i8], [4 x i8]* @input, i32 0, i32 0), double* nonnull @c)

  %6 = load double, double* @b, align 8        ; B
  %7 = fmul double %6, %6                      ; B * B

  %8 = load double, double* @a, align 8        ; A
  %9 = load double, double* @c, align 8        ; C

  %10 = fmul double %8, %9
  %11 = fmul double %10, 4.000000e+00          ; 4 * A * C

  %12 = fsub double %7, %11                    ; B * B - 4 * A * C

  %13 = call double @sqrt(double %12)          ; D

  %14 = fmul double %8, 2.000000e+00           ; 2 * A
  %15 = fsub double 0.000000e+00, %6           ; -B

  %16 = fadd double %15, %13                   ; -B + D
  %17 = fdiv double %16, %14                   ; (-B + D)/(2 * A)

  store double %17, double* @x1, align 8

  %18 = fsub double %15, %13                   ; -B - D
  %19 = fdiv double %18, %14                   ; (-B - D)/(2 * A)

  store double %19, double* @x2, align 8

  %20 = call i32 (i8*, ...) @printf(i8* getelementptr ([10 x i8], [10 x i8]* @x1.format, i32 0, i32 0), double %17)
  %21 = call i32 (i8*, ...) @printf(i8* getelementptr ([10 x i8], [10 x i8]* @x2.format, i32 0, i32 0), double %19)

  ret i32 0
}

declare i32 @printf(i8*, ...)
declare i32 @scanf(i8*, ...)
declare double @sqrt(double)
