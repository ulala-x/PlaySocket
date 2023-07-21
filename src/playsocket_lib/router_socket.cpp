
#include <format>
#include <spdlog/spdlog.h>
#include <zmq_addon.hpp>

#include "bit_converter.hpp"
#include "router_message.hpp"
#include "router_socket.hpp"
#include "stream_parser.hpp"

namespace Play
{

RouterSocket::RouterSocket(const std::string &options,
                           const std::string &endpoint)
    : _config(SocketConfig(options)), _endpoint(endpoint),
      _socket(zmq::socket_t(_ctx, zmq::socket_type::router))
{

    _socket.set(zmq::sockopt::routing_id, endpoint);
    _socket.set(zmq::sockopt::immediate, _config.immediate());
    _socket.set(zmq::sockopt::router_handover, _config.routerHandOver());
    _socket.set(zmq::sockopt::backlog, _config.backLog());
    _socket.set(zmq::sockopt::linger, _config.linger());
    _socket.set(zmq::sockopt::tcp_keepalive, _config.tcpKeepAlive());
    _socket.set(zmq::sockopt::tcp_keepalive_cnt, _config.tcpKeepAliveCount());
    _socket.set(zmq::sockopt::tcp_keepalive_intvl,
                _config.tcpKeepAliveInterval());
    _socket.set(zmq::sockopt::sndbuf, _config.sendBufferSize());
    _socket.set(zmq::sockopt::rcvbuf, _config.receiveBufferSize());
    _socket.set(zmq::sockopt::rcvhwm, _config.receiveHighWatermark());
    _socket.set(zmq::sockopt::sndhwm, _config.sendHighWatermark());
    _socket.set(zmq::sockopt::router_mandatory, _config.routerMandatory());
}
RouterSocket::~RouterSocket()
{
}
void RouterSocket::bind()
{
    _socket.bind(_endpoint);
}
bool RouterSocket::send(RouterMessage &message)
{
    zmq::send_result_t sendresult;

    zmq::multipart_t send_msgs;
    send_msgs.add(std::move(message.target()));
    send_msgs.add(std::move(message.Header()));
    send_msgs.add(std::move(message.body()));

    if (!zmq::send_multipart(_socket, send_msgs))
    {
        return false;
    };

    return true;
}

RouterMessage *RouterSocket::recv()
{
    std::vector<zmq::message_t> recv_msgs;
    const auto ret =
        zmq::recv_multipart(_socket, std::back_inserter(recv_msgs));

    if (!ret)
    {
        return {};
    }
    if (recv_msgs.size() != 3)
    {
        spdlog::info(
            std::format("message size is invalid : {}", recv_msgs.size()));
        return {};
    }


    RouterMessage *message = new RouterMessage(std::move(recv_msgs[0]),
                                               std::move(recv_msgs[1]),
                                               std::move(recv_msgs[2]));

    recv_msgs.clear();
    return message;
}
void RouterSocket::connect(const std::string &target)
{
    _socket.connect(target.c_str());
}
void RouterSocket::disconnect(const std::string &target)
{
    _socket.disconnect(target.c_str());
}

std::unique_ptr<zmq::message_t> RouterSocket::makeClientMessageBody(
    uint16_t bodySize,
    int16_t serviceId,
    int32_t msgId,
    int16_t msgSeq,
    int16_t errorCode,
    int8_t stageIndex,
    const unsigned char *body)
{

    if (bodySize > MAX_PACKET_SIZE)
    {
        throw std::out_of_range(
            std::format("packet size is over Max - bodysize:{}", bodySize));
    }

    _buffer.clear();
    _buffer.write(BitConverter::toNetwork(bodySize));
    _buffer.write(BitConverter::toNetwork(serviceId));
    _buffer.write(BitConverter::toNetwork(msgId));
    _buffer.write(BitConverter::toNetwork(msgSeq));
    _buffer.write(BitConverter::toNetwork(errorCode));
    _buffer.write(stageIndex);
    _buffer.write(body, 0, bodySize);

    auto message = std::make_unique<zmq::message_t>(bodySize);
    _buffer.read(static_cast<unsigned char *>(message->data()),
                 0,
                 _buffer.size());
    return message;
}

} // namespace Play
