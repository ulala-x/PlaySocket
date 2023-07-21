#pragma once

#include "bit_converter.hpp" // BitConverter 헤더 파일을 포함합니다.
#include <catch2/catch_test_macros.hpp>

TEST_CASE("BitConverter toNetwork and toHost", "[BitConverter]")
{
    SECTION("uint16_t conversion")
    {
        uint16_t value = 0x1234;
        uint16_t networkValue = BitConverter::toNetwork(value);
        uint16_t hostValue = BitConverter::toHost(networkValue);
        REQUIRE(networkValue == 0x3412);
        REQUIRE(hostValue == value);
    }

    SECTION("int16_t conversion")
    {
        int16_t value = -10;
        int16_t networkValue = BitConverter::toNetwork(value);
        int16_t hostValue = BitConverter::toHost(networkValue);
        REQUIRE(networkValue == -2305);
        REQUIRE(hostValue == value);
    }

    SECTION("uint32_t conversion")
    {
        uint32_t value = 0x12345678;
        uint32_t networkValue = BitConverter::toNetwork(value);
        uint32_t hostValue = BitConverter::toHost(networkValue);
        REQUIRE(networkValue == 0x78563412);
        REQUIRE(hostValue == value);
    }

    SECTION("int32_t conversion")
    {
        int32_t value = -10;
        int32_t networkValue = BitConverter::toNetwork(value);
        int32_t hostValue = BitConverter::toHost(networkValue);
        REQUIRE(networkValue == -150994945);
        REQUIRE(hostValue == value);
    }

    SECTION("uint64_t conversion")
    {
        uint64_t value = 0x1234567890ABCDEF;
        uint64_t networkValue = BitConverter::toNetwork(value);
        uint64_t hostValue = BitConverter::toHost(networkValue);
        REQUIRE(networkValue == 0xEFCDAB9078563412);
        REQUIRE(hostValue == value);
    }

    SECTION("int64_t conversion")
    {
        int64_t value = -10;
        int64_t networkValue = BitConverter::toNetwork(value);
        int64_t hostValue = BitConverter::toHost(networkValue);
        REQUIRE(networkValue == -648518346341351425);
        REQUIRE(hostValue == value);
    }
}
