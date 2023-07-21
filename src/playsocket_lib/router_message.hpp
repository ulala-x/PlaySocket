
#pragma once
#include <iostream>
#include <zmq_addon.hpp>

namespace Play
{
class RouterMessage
{
private:
    zmq::message_t _target;
    zmq::message_t _header;
    zmq::message_t _body;

public:
    RouterMessage(const std::string &target,
                  const std::string &Header,
                  zmq::message_t &&body);
    RouterMessage(zmq::message_t &&target,
                  zmq::message_t &&Header,
                  zmq::message_t &&body);
    ~RouterMessage();

    zmq::message_t &target();
    zmq::message_t &Header();
    zmq::message_t &body();
};


} // namespace Play
