#include "byte_buffer.hpp"

#include "test_util.hpp"

#include "gtest/gtest.h"

#include <vector>

using namespace tent;

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

    auto EXPECTED_data = gen_rnd_data(size);
    byte_buffer bb{size};
    bb.write(EXPECTED_data.data(), size);

    ASSERT_EQ(bb.read_available(), size);

    std::vector<uint8_t> actual_data{bb.get_read(), bb.get_read() + bb.read_available()};
    ASSERT_EQ(actual_data, EXPECTED_data);
}

TEST(byte_buffer_test, write_and_expand)
{
    constexpr size_t EXPECTED_SIZE = 2000;

    auto EXPECTED_data = gen_rnd_data(EXPECTED_SIZE);
    byte_buffer bb{EXPECTED_SIZE - 1};
    
    bb.write(EXPECTED_data.data(), EXPECTED_SIZE);
    ASSERT_EQ(bb.size(), 2 *EXPECTED_SIZE);
    ASSERT_EQ(bb.read_available(), EXPECTED_SIZE);

    std::vector<uint8_t> actual_data{bb.get_read(), bb.get_read() + EXPECTED_SIZE};
    ASSERT_EQ(actual_data, EXPECTED_data);
}
// TODO: to slow
// TEST(byte_buffer_test, exceed_write_limit)
// {
//     uint64_t MAX = byte_buffer::MAX_SIZE;
    
//     { 
//         std::vector<uint8_t> data(MAX + 1, 0);

//         byte_buffer bb{0};        
//         ASSERT_THROW(bb.write(data.data(), data.size()), std::length_error);
//     }
    
//     // ctor
//     {
//         ASSERT_NO_THROW(byte_buffer{MAX});
//         ASSERT_THROW(byte_buffer{MAX + 1}, std::length_error);
//     }
// }

TEST(byte_buffer_test, size)
{
    constexpr size_t EXPECTED_SIZE = 100;
    
    byte_buffer bb(EXPECTED_SIZE);
    ASSERT_EQ(bb.size(), EXPECTED_SIZE);
}

TEST(byte_buffer_test, write_read_8)
{
    byte_buffer bb(10);

    constexpr uint8_t EXPECTED = 0x12;

    ASSERT_EQ(bb.read_available(), 0);
    bb.write_8(EXPECTED);
    ASSERT_EQ(bb.read_available(), 1);
    ASSERT_EQ(bb.read_8(), EXPECTED);
    ASSERT_EQ(bb.read_available(), 0);
}

TEST(byte_buffer_test, write_read_16)
{
    byte_buffer bb(10);

    constexpr uint16_t EXPECTED = 0x1234;

    ASSERT_EQ(bb.read_available(), 0);
    bb.write_16(EXPECTED);
    ASSERT_EQ(bb.read_available(), 2);
    ASSERT_EQ(bb.read_16(), EXPECTED);
    ASSERT_EQ(bb.read_available(), 0);
}

TEST(byte_buffer_test, write_read_32)
{
    byte_buffer bb(10);

    constexpr uint32_t EXPECTED = 0x12345678;

    ASSERT_EQ(bb.read_available(), 0);
    bb.write_32(EXPECTED);
    ASSERT_EQ(bb.read_available(), 4);
    ASSERT_EQ(bb.read_32(), EXPECTED);
    ASSERT_EQ(bb.read_available(), 0);
}

TEST(byte_buffer_test, write_read_64)
{
    byte_buffer bb(10);

    constexpr uint64_t EXPECTED = 0x1234567890123456;
    
    ASSERT_EQ(bb.read_available(), 0);
    bb.write_64(EXPECTED);
    ASSERT_EQ(bb.read_available(), 8);
    ASSERT_EQ(bb.read_64(), EXPECTED);
    ASSERT_EQ(bb.read_available(), 0);
}

TEST(byte_buffer_test, write_read)
{
    constexpr auto SIZE = 8;
    const uint8_t data[SIZE] = {0x12, 0x34, 0x56, 0x78, 0x12, 0x34, 0x56, 0x78};
    uint8_t  EXPECTED_U8  = 0x12;
    uint16_t EXPECTED_U16 = 0x1234;
    uint32_t EXPECTED_U32 = 0x12345678;
    uint64_t EXPECTED_U64 = 0x1234567812345678;
    
    {
        byte_buffer bb(10);
        bb.write(data, SIZE);

        ASSERT_EQ(bb.read_8(), EXPECTED_U8);
    }

    {
        byte_buffer bb(10);
        bb.write(data, SIZE);

        ASSERT_EQ(bb.read_16(), EXPECTED_U16);
    }

    {
        byte_buffer bb(10);
        bb.write(data, SIZE);

        ASSERT_EQ(bb.read_32(), EXPECTED_U32);
    }

    {
        byte_buffer bb(10);
        bb.write(data, SIZE);
        
        ASSERT_EQ(bb.read_64(), EXPECTED_U64);
    }
}

TEST(byte_buffer_test, write_peek_8)
{
    constexpr uint8_t EXPECTED = 0x12;
    byte_buffer bb(10);

    bb.write_8(EXPECTED);
    ASSERT_EQ(bb.peek_8(), EXPECTED);
}

TEST(byte_buffer_test, write_peek_16)
{
    constexpr uint16_t EXPECTED = 0x1234;
    byte_buffer bb(10);

    bb.write_16(EXPECTED);
    ASSERT_EQ(bb.peek_16(), EXPECTED);
}

TEST(byte_buffer_test, write_peek_32)
{
    constexpr uint32_t EXPECTED = 0x12345678;
    byte_buffer bb(10);

    bb.write_32(EXPECTED);
    ASSERT_EQ(bb.peek_32(), EXPECTED);
}

TEST(byte_buffer_test, write_peek_64)
{
    constexpr uint64_t EXPECTED = 0x1234567812345678;
    byte_buffer bb(10);

    bb.write_64(EXPECTED);
    ASSERT_EQ(bb.peek_64(), EXPECTED);
}

TEST(byte_buffer_test, equal)
{
    {
        byte_buffer bb1{10}, bb2{10};
        ASSERT_EQ(bb1, bb2);

        const auto data = gen_rnd_data(100);
        bb1.write(data.data(), data.size());
        bb2.write(data.data(), data.size());

        ASSERT_EQ(bb1, bb2);
    }   

    {
        byte_buffer bb1{10}, bb2{12};
        ASSERT_NE(bb1, bb2);

        const auto data1 = gen_rnd_data(100);
        auto data2 = data1;
        data2[0] = ~data1[0];

        bb1.write(data1.data(), data1.size());
        bb2.write(data2.data(), data2.size());

        ASSERT_NE(bb1, bb2);
    }    
}




