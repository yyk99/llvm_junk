#
#
#

TARGETS = hello.ll hello.s hello hello.bc zero.ll var1.ll var1.s \
	t1.ll \
	loop.ll \
	for_double.ll

all:    $(TARGETS)	
	@echo Done!

clean:
	rm -f hello *.s *.bc *.ll

hello.ll:	hello.c
	clang -S -emit-llvm hello.c

for_double.ll:	for_double.c
	clang -S -emit-llvm for_double.c

loop.ll:	loop.c
	clang -S -emit-llvm loop.c

var1.ll:	var1.c
	clang -S -emit-llvm var1.c

t1.ll:	t1.c
	clang -S -emit-llvm t1.c

var1.s:	var1.c
	clang -S var1.c

zero.ll:	zero.c
	clang -S -emit-llvm zero.c

hello:	hello.c
	clang hello.c -o hello

hello.bc:
	clang -O3 -emit-llvm hello.c -c -o hello.bc

hello.s: hello.bc
	llc hello.bc -o hello.s

