#pragma once

#include <bit>
#include <cstdint>

class BitConverter
{
public:
    static uint16_t toNetwork(uint16_t value)
    {
        if constexpr (std::endian::native == std::endian::big)
        {
            return value;
        }
        else
        {
            return static_cast<uint16_t>(((value & 0x00FF) << 8) |
                                         ((value & 0xFF00) >> 8));
        }
    }

    static int16_t toNetwork(int16_t value)
    {
        return static_cast<int16_t>(toNetwork(static_cast<uint16_t>(value)));
    }

    static uint32_t toNetwork(uint32_t value)
    {
        if constexpr (std::endian::native == std::endian::big)
        {
            return value;
        }
        else
        {
            return (value << 24) | ((value << 8) & 0x00FF0000) |
                   ((value >> 8) & 0x0000FF00) | value >> 24;
        }
    }

    static int32_t toNetwork(int32_t value)
    {
        return static_cast<int32_t>(toNetwork(static_cast<uint32_t>(value)));
    }

    static uint64_t toNetwork(uint64_t value)
    {
        if constexpr (std::endian::native == std::endian::big)
        {
            return value;
        }
        else
        {
            return (value << 56) | ((value << 40) & 0x00FF000000000000) |
                   ((value << 24) & 0x0000FF0000000000) |
                   ((value << 8) & 0x000000FF00000000) |
                   ((value >> 8) & 0x00000000FF000000) |
                   ((value >> 24) & 0x0000000000FF0000) |
                   ((value >> 40) & 0x000000000000FF00) | (value >> 56);
        }
    }

    static int64_t toNetwork(int64_t value)
    {
        return static_cast<int64_t>(toNetwork(static_cast<uint64_t>(value)));
    }

    static uint16_t toHost(uint16_t value)
    {
        return toNetwork(value);
    }

    static int16_t toHost(int16_t value)
    {
        return toNetwork(value);
    }

    static uint32_t toHost(uint32_t value)
    {
        return toNetwork(value);
    }

    static int32_t toHost(int32_t value)
    {
        return toNetwork(value);
    }

    static uint64_t toHost(uint64_t value)
    {
        return toNetwork(value);
    }

    static int64_t toHost(int64_t value)
    {
        return toNetwork(value);
    }
};
