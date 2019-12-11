#ifndef BYTE_BUFFER_HPP_
#define BYTE_BUFFER_HPP_

#include <cstdint>
#include <vector>

namespace tent
{

class byte_buffer
{
public:
    byte_buffer(size_t size);

    const uint8_t* get_read() const;
    uint8_t* get_write();
    void inc_read(size_t inc);
    void inc_write(size_t inc);
    size_t read_available() const;
    size_t write_available() const;
    void reset() { read_ = write_ = 0; }
    void append(const uint8_t* data, size_t size);
    void write(uint8_t* data, size_t size);
    void write_8(uint8_t data);
    void write_32(uint32_t data);
    void write_64(uint64_t data);

private:
    bool invariant() const;

    std::vector<uint8_t> buff_;
    size_t read_;
    size_t write_;
};

}

#endif
