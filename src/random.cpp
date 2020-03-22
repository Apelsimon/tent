#include "random.hpp"

#include <random>

namespace tent
{
namespace rnd 
{

uint32_t uint32()
{
    std::random_device                  rand_dev;
    std::mt19937                        gen{rand_dev()};
    std::uniform_int_distribution<uint32_t>  dist{0, std::numeric_limits<uint32_t>::max()};

    return dist(gen);
}

uint32_t transaction_id()
{
    return uint32();
}

} // namespace rnd
} // namespace tent