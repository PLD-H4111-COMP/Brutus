#include "Options.h"

#include <iostream>

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
                    std::cerr << "You must specify the output file after -o." << std::endl;
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
                std::cerr << "Unknown option : " << input << std::endl;
                return false;
            }
        }
        else
        {
            input_file = input;
        }
    }
    
    return input_file.size();
}
