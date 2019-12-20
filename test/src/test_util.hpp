#ifndef TEST_UTIL_HPP_
#define TEST_UTIL_HPP_

#include <algorithm>
#include <climits>
#include <functional>
#include <random>
#include <vector>


// TODO: not random for multiple calls
static std::vector<uint8_t> gen_rnd_data(size_t size)
{
    using random_bytes_engine = std::independent_bits_engine<
        std::default_random_engine, CHAR_BIT, unsigned char>;

    std::vector<uint8_t> data(size);

    random_bytes_engine rbe;
    std::generate(std::begin(data), std::end(data), std::ref(rbe));
    
    return data;
}


#endif