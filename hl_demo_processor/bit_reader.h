#pragma once

#include "serialization/io_streams.h"
#include <boost/dynamic_bitset.hpp>

struct bit_reader
{
    typedef boost::dynamic_bitset<uint8_t> bitset_t;

    explicit bit_reader(binary::input_stream& is)
        : is_(is)
    {
    }

    bitset_t read_bits(size_t n)
    {
        if (n > remaining_.size())
            buy(n - remaining_.size());

        Assert(remaining_.size() >= n);
        bitset_t result;

        for (size_t i = 0; i < n; ++i)
            result.push_back(remaining_[i]);

        remaining_ >>= n;
        remaining_.resize(remaining_.size() - n);

        return result;
    }

    size_t remaining() const
    {
        return remaining_.size();
    }

private:
    void buy(size_t n)
    {
        size_t const bytes_to_extract = (n + 7) / 8;

        Verify(is_.left() >= bytes_to_extract);

        auto const src_ptr = reinterpret_cast<uint8_t const *>(is_.curr());
        remaining_.append(src_ptr, src_ptr + bytes_to_extract);
        is_.skip(bytes_to_extract);
    }

private:
    binary::input_stream &is_;
    bitset_t remaining_;
};