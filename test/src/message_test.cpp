#include "byte_buffer.hpp"
#include "message.hpp"
#include "messages.hpp"
#include "msg_factory.hpp"

#include "pieces.hpp"

#include "test_util.hpp"

#include "gtest/gtest.h"

using namespace tent;

TEST(message_test, ctor)
{
    constexpr uint32_t EXPECTED_INDEX = 1;
    constexpr uint32_t EXPECTED_BEGIN = 2;

    const auto data = gen_rnd_data(100);
    byte_buffer block{};
    block.write(data.data(), data.size());
    
    const msg::piece piece{EXPECTED_INDEX, EXPECTED_BEGIN, block};

    byte_buffer bb;
    msg_factory::piece(bb, piece);

    message msg{bb};

    ASSERT_EQ(msg.id_, message::id::PIECE);

    ASSERT_EQ(msg.payload_.read_32(), EXPECTED_INDEX);
    ASSERT_EQ(msg.payload_.read_32(), EXPECTED_BEGIN);

    const std::vector<uint8_t> expected_block{block.get_read(), 
        block.get_read() + block.read_available()};
    const std::vector<uint8_t> actual_block{msg.payload_.get_read(), 
        msg.payload_.get_read() + msg.payload_.read_available()};

    ASSERT_EQ(expected_block, actual_block);
}