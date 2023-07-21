#pragma once
#include "ring_buffer.hpp"
#include <catch2/catch_test_macros.hpp>

using namespace Play;

TEST_CASE("RingBuffer functionality", "[RingBuffer]")
{
    SECTION("Initialization")
    {
        RingBuffer buf(10, 20);
        REQUIRE(buf.capacity() == 10);
        REQUIRE(buf.size() == 0);
    }

    SECTION("Push and pop single values")
    {
        RingBuffer buf(5);
        buf.push(static_cast<unsigned char>(5));
        REQUIRE(buf.size() == 1);
        REQUIRE(buf.peek() == 5);

        unsigned char result = buf.pop();
        REQUIRE(result == 5);
        REQUIRE(buf.size() == 0);
    }

    SECTION("Push beyond capacity and resize buffer")
    {
        RingBuffer buf(2, 5);
        buf.push(static_cast<unsigned char>(1));
        buf.push(static_cast<unsigned char>(2));
        REQUIRE(buf.capacity() == 2);

        buf.push(static_cast<unsigned char>(3));
        REQUIRE(buf.size() == 3);
        REQUIRE(buf.capacity() == 4);
        buf.push(static_cast<unsigned char>(4));
        // REQUIRE(buf.size() == 4);
        //buf.push(static_cast<unsigned char>(5));
        REQUIRE_THROWS_AS(buf.push(static_cast<unsigned char>(5)),
                          std::out_of_range);
    }

    SECTION("Push and pop multiple values")
    {
        std::vector<unsigned char> data = {1, 2, 3, 4, 5};
        RingBuffer buf(10);
        buf.push(data);

        REQUIRE(buf.size() == 5);

        for (size_t i = 0; i < data.size(); i++)
        {
            REQUIRE(buf.pop() == data[i]);
        }

        REQUIRE(buf.size() == 0);
    }

    SECTION("Clear buffer")
    {
        std::vector<unsigned char> data = {1, 2, 3, 4, 5};
        RingBuffer buf(10);
        buf.push(data);

        buf.clear();
        REQUIRE(buf.size() == 0);
        REQUIRE_THROWS_AS(buf.pop(), std::runtime_error);
    }

    SECTION("Read and write methods")
    {
        unsigned char inputBuffer[5] = {1, 2, 3, 4, 5};
        unsigned char outputBuffer[5] = {0};

        RingBuffer buf(10);
        buf.write(inputBuffer, 0, 5);

        REQUIRE(buf.size() == 5);

        size_t bytesRead = buf.read(outputBuffer, 0, 5);
        REQUIRE(bytesRead == 5);

        for (size_t i = 0; i < 5; i++)
        {
            REQUIRE(outputBuffer[i] == inputBuffer[i]);
        }

        REQUIRE(buf.size() == 0);
    }

    SECTION("Reading and writing integers")
    {
        RingBuffer buf(10);

        // Boundary tests for 16-bit integers
        int16_t smallValue = -32768;
        int16_t largeValue = 32767;

        buf.write(smallValue);
        REQUIRE(buf.peekInt16() == smallValue);
        REQUIRE(buf.readInt16() == smallValue);

        buf.write(largeValue);
        REQUIRE(buf.peekInt16() == largeValue);
        REQUIRE(buf.readInt16() == largeValue);

        // Similarly, you can test for 32-bit integers
    }

    SECTION("Clear specific count")
    {
        std::vector<unsigned char> data = {1, 2, 3, 4, 5};
        RingBuffer buf(10);
        buf.push(data);

        buf.clear(3);
        REQUIRE(buf.size() == 2);
        REQUIRE(buf.pop() == 4);
        REQUIRE(buf.pop() == 5);
    }

    SECTION("Read with offset")
    {
        unsigned char inputBuffer[5] = {1, 2, 3, 4, 5};
        unsigned char outputBuffer[5] = {0};

        RingBuffer buf(10);
        buf.write(inputBuffer, 0, 5);

        REQUIRE(buf.size() == 5);

        size_t bytesRead = buf.read(outputBuffer, 2, 3);
        REQUIRE(bytesRead == 3);

        REQUIRE(outputBuffer[0] == 0);
        REQUIRE(outputBuffer[1] == 0);
        REQUIRE(outputBuffer[2] == 1);
        REQUIRE(outputBuffer[3] == 2);
        REQUIRE(outputBuffer[4] == 3);

        REQUIRE(buf.size() == 2);
    }

    SECTION("Write with std::span")
    {
        std::vector<unsigned char> inputBuffer = {1, 2, 3, 4, 5};
        unsigned char outputBuffer[5] = {0};

        RingBuffer buf(10);
        buf.write(std::span<const unsigned char>(inputBuffer));

        REQUIRE(buf.size() == 5);

        size_t bytesRead = buf.read(outputBuffer, 0, 5);
        REQUIRE(bytesRead == 5);

        for (size_t i = 0; i < 5; i++)
        {
            REQUIRE(outputBuffer[i] == inputBuffer[i]);
        }

        REQUIRE(buf.size() == 0);
    }

    SECTION("Read and write with invalid parameters")
    {
        RingBuffer buf(5);

        unsigned char buffer[5] = {1, 2, 3, 4, 5};

        buf.write(buffer, 0, 5);

        REQUIRE(buf.size() == 5);

        buf.pop();
        buf.pop();
        buf.pop();
        buf.pop();
        buf.pop();
        REQUIRE(buf.size() == 0);
        REQUIRE_THROWS_AS(buf.pop(), std::runtime_error);
    }
}
