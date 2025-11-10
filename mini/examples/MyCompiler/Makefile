
.SUFFIXES : .cc .y .l $(SUFFIXES)

.cc.o :
	g++ -g -I . -c $*.cc

# COMPILER SAMPLE
all:    mycompiler
	@echo Done!

clean:
	rm -f *.o MyParser.cc MyScanner.cc MyParser.hh location.hh position.hh stack.hh mycompiler

MyCompiler.o : MyCompiler.cc MyParser.hh

MyParser.o : MyParser.cc MyParser.hh

MyScanner.o : MyScanner.cc MyParser.hh

MyParser.cc MyParser.hh : MyParser.y
	bison --defines=MyParser.hh -o MyParser.cc MyParser.y

MyScanner.cc : MyScanner.l MyParser.hh
	flex -o MyScanner.cc MyScanner.l

mycompiler : MyCompiler.o MyParser.o MyScanner.o
	g++ -o $@ MyCompiler.o MyParser.o MyScanner.o

