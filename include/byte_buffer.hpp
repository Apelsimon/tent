#ifndef BYTE_BUFFER_HPP_
#define BYTE_BUFFER_HPP_

#include <cstdint>
#include <limits>
#include <vector>

namespace tent
{

class byte_buffer
{
public:
    byte_buffer();
    byte_buffer(size_t size);

    const uint8_t* get_read() const;
    uint8_t* get_write();
    void inc_read(size_t inc);
    void inc_write(size_t inc);
    size_t read_available() const;
    size_t write_available() const;
    void reset() { read_ = 0; write_ = 0; }
    void write(const uint8_t* data, size_t size);
    void write_8(uint8_t data);
    void write_32(uint32_t data);
    void write_64(uint64_t data);
    uint8_t* read(size_t size);
    uint8_t read_8();
    uint32_t read_32();
    uint8_t peek_8(size_t offset = 0) const;
    uint32_t peek_32(size_t offset = 0) const;
    size_t size() const { return buff_.size(); }
    byte_buffer slice(size_t begin, size_t end) const;

    static constexpr auto MAX_SIZE = std::numeric_limits<uint32_t>::max();

private:
    void expand(size_t size);
    bool invariant() const;
    const uint8_t* data() const { return buff_.data(); }

    std::vector<uint8_t> buff_;
    size_t read_;
    size_t write_;
};

}

#endif
