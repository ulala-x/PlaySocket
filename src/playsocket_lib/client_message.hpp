#pragma once

#include <zmq.hpp>

namespace Play
{

struct Header
{
    Header(int16_t service_id,
           int32_t msg_id,
           int16_t msg_seq,
           int8_t stage_index)
        : service_id(service_id), msg_id(msg_id), msg_seq(msg_seq),
          stage_index(stage_index){};
    int16_t service_id;
    int32_t msg_id;
    int16_t msg_seq;
    int8_t stage_index;
};

enum MessageType
{
    CONNECT = 1,
    DISCONNECT = 2,
    NORMAL = 3
};

class ClientMessage
{
private:
    int64_t _sid;
    Header _header;
    std::unique_ptr<zmq::message_t> _body;
    MessageType _type = MessageType::NORMAL;

public:
    explicit ClientMessage(int64_t _sid, const MessageType &type);
    explicit ClientMessage(int64_t _sid,
                           const Header &header,
                           std::unique_ptr<zmq::message_t> body);
    // explicit ClientMessage(ClientMessage &&other) noexcept;

    const int64_t sid() const;
    const Header &header() const;
    std::unique_ptr<zmq::message_t> body();
    const MessageType &type() const;
};

} // namespace Play
