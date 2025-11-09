#
#
#




%.ll : %.c
	clang -S -emit-llvm $<


TARGETS = hello.ll hello.s hello hello.bc zero.ll var1.ll var1.s \
	t1.ll \
	loop.ll \
	arr.ll \
	arr2.ll \
	arr3.ll \
	for_double.ll

all:    $(TARGETS)	
	@echo Done!

clean:
	rm -f hello *.s *.bc *.ll

hello:	hello.c
	clang hello.c -o hello

hello.bc:

hello.s: hello.bc

var1.s:	var1.ll
	llc-18 $<
