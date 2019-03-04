#include "antlr4-runtime.h"
#include "CProgBaseVisitor.h"
#include "CProgLexer.h"
#include "CProgParser.h"
#include <istream>
#include <iostream>
#include <string>

using namespace std;
using namespace antlr4;

int main(int argc, char **argv)
{
    if(argc != 2)
    {
        cout << "Usage: ./BRUTUS <filename.c>" << endl;
    }
    
    ifstream file(argv[1]);
    string line, prog;
    while(getline(file, line))
    {
        prog += line;
    }
    ANTLRInputStream input(prog);
    CProgLexer lexer(&input);
    CommonTokenStream tokens(&lexer);
    CProgParser parser(&tokens);
    tree::ParseTree *tree = parser.prog();
    
    CProgBaseVisitor visitor;
    visitor.visit(tree);
    return 0;
}
