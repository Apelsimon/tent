#include "file_reader.hpp"

#include <fstream>
#include <iterator>

namespace tent
{

std::vector<char> file_reader::to_buffer(const std::string& filename)
{
    std::ifstream ifs(filename, std::ios::binary);
    return {(std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>()};
}

}