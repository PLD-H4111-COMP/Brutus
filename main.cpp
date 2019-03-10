#include "antlr4-runtime.h"
#include "CProgASVisitor.h"
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
        return 1;
    }

    ifstream file(argv[1]);
    ANTLRInputStream input(file);
    CProgLexer lexer(&input);
    CommonTokenStream tokens(&lexer);
    CProgParser parser(&tokens);
    tree::ParseTree *tree = parser.prog();
    
    CProgASVisitor visitor;
    visitor.visit(tree);
    return 0;
}
