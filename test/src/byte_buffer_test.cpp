#include "byte_buffer.hpp"

#include "gtest/gtest.h"

#include <vector>
#include <random>
#include <climits>
#include <algorithm>
#include <functional>

using namespace tent;

static std::vector<uint8_t> gen_rnd_data(size_t size);

TEST(byte_buffer_test, inc_read)
{
    constexpr auto SIZE = 10;
    
    byte_buffer bb{SIZE};
    auto write_data = gen_rnd_data(SIZE / 2);

    
    ASSERT_EQ(bb.read_available(), 0);
    bb.write(write_data.data(), write_data.size());
    ASSERT_EQ(bb.read_available(), write_data.size());
    bb.inc_read(write_data.size());
    ASSERT_EQ(bb.read_available(), 0);
    
    ASSERT_THROW(bb.inc_read(10), std::range_error);
}

TEST(byte_buffer_test, inc_write)
{
    constexpr auto SIZE = 10;
    
    byte_buffer bb{SIZE};

    ASSERT_EQ(bb.write_available(), SIZE);
    bb.inc_write(SIZE);
    ASSERT_EQ(bb.write_available(), 0);
    ASSERT_THROW(bb.inc_write(1), std::range_error);
}

TEST(byte_buffer_test, write)
{
    constexpr size_t size = 1000;

    auto expected_data = gen_rnd_data(size);
    byte_buffer bb{size};
    bb.write(expected_data.data(), size);

    ASSERT_EQ(bb.read_available(), size);

    std::vector<uint8_t> actual_data{bb.get_read(), bb.get_read() + bb.read_available()};
    ASSERT_EQ(actual_data, expected_data);
}

TEST(byte_buffer_test, write_and_expand)
{
    constexpr size_t EXPECTED_SIZE = 2000;

    auto expected_data = gen_rnd_data(EXPECTED_SIZE);
    byte_buffer bb{EXPECTED_SIZE / 2};
    
    bb.write(expected_data.data(), EXPECTED_SIZE);
    ASSERT_EQ(bb.size(), EXPECTED_SIZE);
    ASSERT_EQ(bb.read_available(), EXPECTED_SIZE);

    std::vector<uint8_t> actual_data{bb.get_read(), bb.get_read() + EXPECTED_SIZE};
    ASSERT_EQ(actual_data, expected_data);
}

TEST(byte_buffer_test, exceed_write_limit)
{
    {
        auto data = gen_rnd_data(byte_buffer::MAX_SIZE + 1);
        byte_buffer bb{0};
        ASSERT_THROW(bb.write(data.data(), data.size()), std::length_error);
    }
    
    // ctor
    {
        ASSERT_NO_THROW(byte_buffer{byte_buffer::MAX_SIZE});
        ASSERT_THROW(byte_buffer{byte_buffer::MAX_SIZE + 1}, std::length_error);
    }
}

TEST(byte_buffer_test, size)
{
    constexpr size_t EXPECTED_SIZE = 100;
    
    byte_buffer bb(EXPECTED_SIZE);
    ASSERT_EQ(bb.size(), EXPECTED_SIZE);
}

TEST(byte_buffer_test, peek_32)
{
    constexpr uint32_t Expected = 123;
    byte_buffer bb(10);

    bb.write_32(Expected);
    ASSERT_EQ(bb.peek_32(), Expected);
}

std::vector<uint8_t> gen_rnd_data(size_t size)
{
    using random_bytes_engine = std::independent_bits_engine<
        std::default_random_engine, CHAR_BIT, unsigned char>;

    std::vector<uint8_t> data(size);
    random_bytes_engine rbe;
    std::generate(std::begin(data), std::end(data), std::ref(rbe));

    return data;
}