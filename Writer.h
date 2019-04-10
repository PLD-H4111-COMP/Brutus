#pragma once

#include <fstream>

struct Options;

class Writer
{
public:
    Writer(const Options &options);
    std::ostream& assembly(unsigned int indent);
    static std::ostream& info();
    static std::ostream& warning();
    static std::ostream& error();
    static bool errorOccurred;

private:
    std::ofstream m_output_file_stream;
    
};

