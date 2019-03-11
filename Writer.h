#ifndef WRITER_H
#define WRITER_H

#include <fstream>

class Writer
{
private:
    std::ostream m_stream;

public:
    Writer(string options);
    std::ostream& stream();
};

#endif // WRITER_H
