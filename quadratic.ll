declare i32 @printf(i8*, ...)
declare i32 @scanf(i8*, ...)

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
  %5 = call i32 (i8*, ...) @scanf(i8* getelementptr ([4 x i8], [4 x i8]* @input, i32 0, i32 0), double* nonnull @b)



  ret i32 0
}
