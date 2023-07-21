#include "client_message.hpp"

namespace Play
{

ClientMessage::ClientMessage(int64_t sid, const MessageType &type)
    : _sid(sid), _type(type), _header(Play::Header(0, 0, 0, 0)), _body(nullptr)
{
}
ClientMessage::ClientMessage(int64_t sid,
                             const Play::Header &header,
                             std::unique_ptr<zmq::message_t> body)
    : _sid(sid), _header(header), _body(std::move(body))
{
}
// ClientMessage::ClientMessage(ClientMessage &&other) noexcept
//     : _sid(std::move(other._sid)), _header(std::move(other._header)),
//       _body(std::move(other._body)), _type(std::move(other._type))
// {
// }

const int64_t ClientMessage::sid() const
{
    return _sid;
};
const Header &ClientMessage::header() const
{
    return _header;
};
std::unique_ptr<zmq::message_t> ClientMessage::body()
{
    return std::move(_body);
};
const MessageType &ClientMessage::type() const
{
    return _type;
}
} // namespace Play
