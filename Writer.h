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
    std::ostream& info();
    std::ostream& warning();
    std::ostream& error();
};

