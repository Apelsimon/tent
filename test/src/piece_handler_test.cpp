#include "piece_handler.hpp"

#include "byte_buffer.hpp"
#include "file_reader.hpp"
#include "net_reactor.hpp"
#include "session.hpp"

#include "libtorrent/torrent_info.hpp"

#include "gtest/gtest.h"

#include <bitset>
#include <cstdint>

using namespace tent;

class piece_handler_test : public ::testing::Test
{
protected:

    void SetUp() override 
    {
        auto file_buffer = tent::file_reader::to_buffer("charlie.torrent");
        lt::error_code error;
    
        info_ = std::make_unique<lt::torrent_info>(file_buffer.data(), static_cast<int>(file_buffer.size()), error);
        ASSERT_EQ(error.value(), 0);
        session_ = std::make_unique<session>(reactor_, *info_);
        handler_ = std::make_unique<piece_handler>(*session_, *info_);
    }

    byte_buffer buildBitfield();
    
    std::unique_ptr<lt::torrent_info> info_;
    net_reactor reactor_;
    std::unique_ptr<session> session_;
    std::unique_ptr<piece_handler> handler_;
};

// TEST_F(piece_handler_test, tmp)
// {
//     const std::string PEER_ID = "-tent1234-1234567890";
//     auto bitfield = buildBitfield();
//     handler_->have(PEER_ID, bitfield);
// }

byte_buffer piece_handler_test::buildBitfield()
{
    constexpr uint16_t BLOCK_LEN = 1 << 14;

    byte_buffer bitfield;
    for(auto it = 0; it < info_->num_pieces(); it += 8)
    {
        bitfield.write_8(0xff);
    }    

    return bitfield;
}