#include "parserOptions.h"

#include <iostream>
#include <fstream> 

Options::Options() : input_file(""), output_file("brutus.s"), optimisation(false), generate_assembly(true), help(false)
{
	
}

bool Options::parseOptions(int nb_options, char **option_inputs)
{
	if (nb_options == 1)
	{
		return false;
	}
	for (int i = 1; i < nb_options; ++i)
	{
		std::string input(option_inputs[i]);
		if (input == "--help")
		{
			help = true;
			return true;
		}
	}
	for (int i = 1; i < nb_options-1; ++i)
	{
		std::string input(option_inputs[i]);
		if (input == "-o")
		{
			++i;
			if (i < nb_options-1)
			{
				output_file = option_inputs[i];
			}
			else
			{
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
			std::cout << "Unknown argument : " << input << std::endl;
			return false;
		}
	}
	std::string input(option_inputs[nb_options-1]);
	input_file = input;
	return true;
}