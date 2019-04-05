#pragma once

#include <fstream>

struct Options;

class Writer
{
private:
    std::ofstream m_output_file_stream;

public:
    Writer(const Options &options);
    std::ostream& assembly(unsigned int indent);
    static std::ostream& info();
    static std::ostream& warning();
    static std::ostream& error();
};

