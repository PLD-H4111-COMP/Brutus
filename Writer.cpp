#include "Writer.h"
#include "Options.h"
#include <iostream>

constexpr static auto RESET = "\033[0m";
constexpr static auto BOLD = "\033[1m";
constexpr static auto YELLOW = "\033[33m";
constexpr static auto RED = "\033[31m";

bool Writer::errorOccurred = false;


Writer::Writer(const Options &options) : m_output_file_stream(options.output_file)
{
    if (!m_output_file_stream.is_open())
    {
        error() << "could not write into " << options.output_file << std::endl;
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
    return std::cerr << BOLD << "info: " << RESET;
}

std::ostream& Writer::warning()
{
    return std::cerr << BOLD << YELLOW << "warning: " << RESET;
}

std::ostream& Writer::error()
{
    errorOccurred = true;
    return std::cerr << BOLD << RED << "error: " << RESET;
}

