#
#
#

all:	hello.ll hello.s hello hello.bc zero.ll
	@echo Done!

clean:
	rm -f hello.ll hello.s hello hello.bc zero.ll

hello.ll:	hello.c
	clang -S -emit-llvm hello.c

zero.ll:	zero.c
	clang -S -emit-llvm zero.c

hello:	hello.c
	clang hello.c -o hello

hello.bc:
	clang -O3 -emit-llvm hello.c -c -o hello.bc

hello.s: hello.bc
	llc hello.bc -o hello.s

