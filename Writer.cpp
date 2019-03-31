#include "Writer.h"
#include "Options.h"
#include <iostream>

Writer::Writer(const Options &options) : m_output_file_stream(options.output_file)
{
    if (!m_output_file_stream.is_open())
    {
        std::cerr << "Could not open the file" << options.output_file << std::endl;
    }
}

std::ostream& Writer::assembly(unsigned int indent)
{
    for (unsigned int i=0; i < indent*4; i++)
    {
    	m_output_file_stream << " ";
    }
    return m_output_file_stream;
}

std::ostream& Writer::info()
{
    return std::cerr << "info: ";
}

std::ostream& Writer::warning()
{
    return std::cerr << "warning: ";
}

std::ostream& Writer::error()
{
    return std::cerr << "error: ";
}

