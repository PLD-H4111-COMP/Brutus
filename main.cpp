#include "antlr4-runtime.h"
#include "CProgCSTVisitor.h"
#include "CProgLexer.h"
#include "CProgParser.h"
#include "Options.h"
#include "Writer.h"
#include "IR.h"
#include "CProgAST.h"
#include <istream>
#include <iostream>
#include <string>

using namespace std;
using namespace antlr4;

int main(int argc, char **argv)
{
    Options options;
    if (!options.parseOptions(argc, argv))
    {
        cout << "usage : " << argv[0] << " [options] <input_file>" << endl
             << "[options] : -o <output_file> | -O | -a | --help" << endl;
        return 1;
    }

    if (options.help)
    {
        cout << argv[0] << " [options] <input_file>" << endl
        << "[options] : -o <output_file> | -O | -a | --help" << endl << endl
        << "-o <output_file> :" << endl
        << "définit le nom du fichier de sortie" << endl
        << "-O :" << endl
        << "optimise la représentation en mémoire du programme" << endl
        << "-a :" << endl
        << "s'arrête avant la génération du fichier assembleur" << endl
        << "--help :" << endl
        << "affiche l'utilisation du programme" << endl << endl
        << "Comportement par défaut :" << endl
        << argv[0] << " <input_file>"  << endl
        << "analyse le fichier d'entrée, renvoie toutes les erreurs trouvées sur la sortie" << endl
        << "standard et génère un fichier assembleur nomme brutus.s s'il ne trouve pas d'erreur"  << endl;
        return 0;
    }

    ifstream file(options.input_file);
    ANTLRInputStream input(file);
    CProgLexer lexer(&input);
    CommonTokenStream tokens(&lexer);
    CProgParser parser(&tokens);
    tree::ParseTree *tree = parser.program();

    Writer writer(options);
    CProgCSTVisitor visitor;
    CProgASTProgram *ast = nullptr;
    ast = (CProgASTProgram*) visitor.visit(tree);
    ast->build_ir();
    return 0;
}
