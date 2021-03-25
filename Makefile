all: compiler.cc parser.cc inputbuf.cc lexer.cc
	g++ -std=c++11 compiler.cc parser.cc inputbuf.cc lexer.cc -o exc