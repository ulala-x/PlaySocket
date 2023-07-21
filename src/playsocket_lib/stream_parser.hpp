#pragma once
#include <iostream>
#include <list>

#include "bit_converter.hpp"
#include "client_message.hpp"
#include "logger_interface.hpp"
#include "ring_buffer.hpp"

namespace Play
{

const int MAX_PACKET_SIZE = 65535;
const int HEADER_SIZE = 11;

class StreamParser
{
private:
    int64_t _sid = 0;
    RingBuffer _buffer{1024 * 8, 1024 * 64 * 8};

public:
    StreamParser(int64_t sid) : _sid(sid)
    {
    }

    void write(const unsigned char *buffer, size_t offset, size_t count)
    {
        _buffer.write(buffer, offset, count);
    }
    std::list<std::unique_ptr<ClientMessage>> parse()
    {
        auto messages = std::list<std::unique_ptr<ClientMessage>>();

        while (_buffer.size() >= HEADER_SIZE)
        {
            uint16_t body_size = static_cast<uint16_t>(
                BitConverter::toHost(_buffer.peekInt16()));

            if (body_size > MAX_PACKET_SIZE)
            {
                Log::error(std::format("body size is over : {}", body_size),
                           typeid(this).name());
                throw std::out_of_range("body size is over");
            }

            if (_buffer.size() < body_size + HEADER_SIZE)
            {
                return messages;
            }
            _buffer.clear(2);

            int16_t service_id = BitConverter::toHost(_buffer.readInt16());

            int32_t msg_id = BitConverter::toHost(_buffer.readInt32());

            int16_t msg_seq = BitConverter::toHost(_buffer.readInt16());

            int8_t stage_index = _buffer.readInt8();

            auto body = std::make_unique<zmq::message_t>(body_size);
            _buffer.read(static_cast<uint8_t *>(body->data()), 0, body_size);

            auto message = std::make_unique<ClientMessage>(
                _sid,
                Header(service_id, msg_id, msg_seq, stage_index),
                std::move(body));

            messages.push_back(std::move(message));
        }
        return messages;
    }
};
} // namespace Play
