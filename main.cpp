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
        << "-o <output_file> : définit le nom du fichier de sortie" << endl
        << "-O : optimise la représentation en mémoire du programme" << endl
        << "-a : s'arrête avant la génération du fichier assembleur" << endl
        << "--help : affiche l'utilisation du programme" << endl << endl
        << "Comportement par défaut :" << endl
        << argv[0] << " <input_file>"  << endl
        << "analyse le fichier d'entrée, renvoie toutes les erreurs trouvées sur la sortie" << endl
        << "standard et génère un fichier assembleur nomme brutus.s s'il ne trouve pas d'erreur"  << endl;
        return 0;
    }

    ifstream file(options.input_file);
    if (!file)
    {
        Writer::error() << "cannot open " << options.input_file << std::endl;
        return 1;
    }
    ANTLRInputStream input(file);
    CProgLexer lexer(&input);
    CommonTokenStream tokens(&lexer);
    CProgParser parser(&tokens);
    tree::ParseTree *tree = parser.program();

    Writer writer(options);
    CProgCSTVisitor visitor;
    CProgASTProgram *ast = visitor.visit(tree).as<CProgASTProgram*>();
    if(!ast)
        return 1;

    IR ir(writer, options.input_file);
    ast->build_ir(ir);
    // ir.print_debug_infos();
    if(!writer.error_occurred)
        ir.gen_asm();
    return 0;
}
