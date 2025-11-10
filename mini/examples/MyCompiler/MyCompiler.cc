#include "MyParser.hh"
#include <iostream>

int main(int argc, char **argv)
{
    yy::MyParser parser;
    int result = parser.parse();

    std::cout << "Parsing result: " << (result ? "Error" : "OK") << std::endl;

    return result;
}
