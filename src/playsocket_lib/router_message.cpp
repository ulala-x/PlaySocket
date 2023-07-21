#include "router_message.hpp"

namespace Play
{

RouterMessage::RouterMessage(const std::string &target,
                             const std::string &Header,
                             zmq::message_t &&body)
{
    _target = zmq::message_t(target.c_str(), target.size());
    _header = zmq::message_t(Header.c_str(), Header.size());
    _body = std::move(body);
}
RouterMessage::RouterMessage(zmq::message_t &&target,
                             zmq::message_t &&Header,
                             zmq::message_t &&body)
{
    _target = std::move(target);
    _header = std::move(Header);
    _body = std::move(body);
}
RouterMessage::~RouterMessage()
{
}

zmq::message_t &RouterMessage::target()
{
    return _target;
}
zmq::message_t &RouterMessage::Header()
{
    return _header;
}
zmq::message_t &RouterMessage::body()
{
    return _body;
}

} // namespace Play
