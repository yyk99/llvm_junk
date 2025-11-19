#
#
#

%.ll : %.c
	clang -S -emit-llvm $<
%.ll : %.cc
	clang++ -S -emit-llvm $<

TARGETS = hello.ll hello.s hello hello.bc zero.ll var1.ll var1.s \
	t1.ll \
	loop.ll \
	arr.ll \
	arr2.ll \
	arr3.ll \
	struct.ll \
	sizeof.ll \
	for_double.ll \
	lambda_main

all:    $(TARGETS)	
	@echo Done!

clean:
	rm -f hello lambda_main *.s *.bc *.ll

hello:	hello.c
	clang hello.c -o hello

hello.bc:

hello.s: hello.bc

var1.s:	var1.ll
	llc-18 $<

lambda.ll: lambda.cc

lambda_main: lambda.ll lambda_main.c
	llc-18 lambda.ll
	clang -o lambda_main lambda_main.c lambda.s
