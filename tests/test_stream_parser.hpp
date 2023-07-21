#pragma once

#include <catch2/catch_test_macros.hpp>

#include "client_message.hpp"
#include "stream_parser.hpp"

using namespace Play;

TEST_CASE("StreamParser - Single complete message parsing", "[StreamParser]")
{
    const int64_t sid = 1234; // Replace with an appropriate value
    Play::StreamParser parser(sid);

    // Simulate the input data for a single message (header + body)
    unsigned char data[] = {
        0x00,
        0x0B, // Body size (11 bytes)
        0x00,
        0x01, // Service ID
        0x00,
        0x00,
        0x00,
        0x02, // Message ID
        0x00,
        0x03, // Message sequence
        0x01, // Stage index
        // Add body data here (if any) based on the body size provided above
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00};

    size_t data_size = sizeof(data);

    parser.write(data, 0, data_size);

    // Parse the data and get the messages
    std::list<std::unique_ptr<ClientMessage>> messages = parser.parse();

    REQUIRE(messages.size() == 1);

    const ClientMessage &message = *messages.front();
    REQUIRE(message.sid() == sid);

    const Header &header = message.header();
    REQUIRE(header.service_id == 1);
    REQUIRE(header.msg_id == 2);
    REQUIRE(header.msg_seq == 3);
    REQUIRE(header.stage_index == 1);
}

TEST_CASE("StreamParser - Two complete messages at once", "[StreamParser]")
{
    const int64_t sid = 5678; // Replace with an appropriate value
    Play::StreamParser parser(sid);

    // Simulate the input data for two messages received together (header + body)
    unsigned char data[] = {
        // First message
        0x00,
        0x0B, // Body size (11 bytes)
        0x00,
        0x11, // Service ID
        0x00,
        0x00,
        0x00,
        0x22, // Message ID
        0x00,
        0x33, // Message sequence
        0x02, // Stage index
              // Add body data for the first message here (if any)
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,

        // Second message
        0x00,
        0x0C, // Body size (12 bytes)
        0x00,
        0x44, // Service ID
        0x00,
        0x00,
        0x00,
        0x55, // Message ID
        0x00,
        0x66, // Message sequence
        0x03, // Stage index
              // Add body data for the second message here (if any)
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00};
    size_t data_size = sizeof(data);

    parser.write(data, 0, data_size);

    // Parse the data and get the messages
    std::list<std::unique_ptr<ClientMessage>> messages = parser.parse();

    REQUIRE(messages.size() == 2);

    // First message
    const ClientMessage &message1 = *messages.front();
    REQUIRE(message1.sid() == sid);

    const Header &header1 = message1.header();
    REQUIRE(header1.service_id == 17);
    REQUIRE(header1.msg_id == 34);
    REQUIRE(header1.msg_seq == 51);
    REQUIRE(header1.stage_index == 2);

    // Second message
    messages.pop_front();
    const ClientMessage &message2 = *messages.front();
    REQUIRE(message2.sid() == sid);

    const Header &header2 = message2.header();
    REQUIRE(header2.service_id == 68);
    REQUIRE(header2.msg_id == 85);
    REQUIRE(header2.msg_seq == 102);
    REQUIRE(header2.stage_index == 3);
}

TEST_CASE("StreamParser - Partial message received", "[StreamParser]")
{
    const int64_t sid = 91011; // Replace with an appropriate value
    Play::StreamParser parser(sid);

    // Simulate receiving only the header of a message, not the complete body
    unsigned char data[] = {
        0x00,
        0x0B, // Body size (11 bytes)
        0x00,
        0x01, // Service ID
        0x00,
        0x00,
        0x00,
        0x02, // Message ID
        0x00,
        0x03, // Message sequence
        0x01  // Stage index
              // Note: The body data is not included here

    };
    size_t data_size = sizeof(data);

    parser.write(data, 0, data_size);

    // Parse the data and get the messages
    std::list<std::unique_ptr<ClientMessage>> messages = parser.parse();

    REQUIRE(messages.size() == 0); // No message should be parsed yet

    // Simulate receiving the rest of the body data in the next chunk of data
    // (The total body size is 11 bytes as specified in the header)
    unsigned char body_data[] = {
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00
        // Add the body data here (11 bytes) to complete the first message
    };
    size_t body_data_size = sizeof(body_data);

    parser.write(body_data, 0, body_data_size);

    // Parse the data and get the messages again
    messages = parser.parse();

    REQUIRE(messages.size() == 1);

    const ClientMessage &message = *messages.front();
    REQUIRE(message.sid() == sid);

    const Header &header = message.header();
    REQUIRE(header.service_id == 1);
    REQUIRE(header.msg_id == 2);
    REQUIRE(header.msg_seq == 3);
    REQUIRE(header.stage_index == 1);
}

// Add more test cases here to cover different scenarios
