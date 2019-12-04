#ifndef FILE_READER_HPP_
#define FILE_READER_HPP_

#include <string>
#include <vector>

namespace tent
{

class file_reader
{
public:
    static std::vector<char> to_buffer(const std::string& filename);
};

}

#endif