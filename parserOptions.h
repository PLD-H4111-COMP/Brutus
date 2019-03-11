#pragma once

#include <string>

struct Options
{
	Options();
    std::string input_file;
    std::string output_file;
    bool optimisation;
    bool generate_assembly;
    bool help;
    bool parseOptions(int nb_options, char **option_inputs);
};
