#include "Options.h"
#include "Writer.h"

Options::Options() : input_file(""), output_file("brutus.s"), optimisation(false), generate_assembly(true), help(false)
{
    
}

bool Options::parseOptions(int nb_options, char **option_inputs)
{
    for (int i = 1; i < nb_options; ++i)
    {
        std::string input(option_inputs[i]);
        if (input == "--help")
        {
            help = true;
            return true;
        }
    }
    
    for (int i = 1; i < nb_options; ++i)
    {
        std::string input(option_inputs[i]);
        
        if (input.empty())
            continue;
        
        if (input[0] == '-')
        {
            if (input == "-o")
            {
                ++i;
                if (i < nb_options)
                {
                    output_file = option_inputs[i];
                }
                else
                {
                    Writer::error() << "you must specify the output file after -o." << std::endl;
                    return false;
                }
            }
            else if (input == "-O")
            {
                optimisation = true;
            }
            else if (input == "-a")
            {
                generate_assembly = false;
            }
            else
            {
                Writer::error() << "unknown option : " << input << std::endl;
                return false;
            }
        }
        else
        {
            input_file = input;
        }
    }

    if (!generate_assembly)
        output_file = "";
    
    return input_file.size();
}
