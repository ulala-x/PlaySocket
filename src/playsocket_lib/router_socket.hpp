#pragma once
#include <cxxopts.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <zmq.hpp>

#include "logger_interface.hpp"
#include "ring_buffer.hpp"
#include "router_message.hpp"

namespace Play
{

class SocketConfig
{
public:
    SocketConfig(std::string option)
    {
        // cxxopts를 사용하여 옵션 파싱
        try
        {
            cxxopts::Options options("play_socket");
            options.add_options()("immediate",
                                  "Immediate option",
                                  cxxopts::value<bool>())(
                "router_handover",
                "Router handover option",
                cxxopts::value<bool>())("router_mandatory",
                                        "Router mandatory option",
                                        cxxopts::value<bool>())(
                "tcp_keepalive",
                "TCP keepalive option",
                cxxopts::value<int>())("tcp_keepalive_count",
                                       "TCP keepalive count option",
                                       cxxopts::value<int>())(
                "tcp_keepalive_interval",
                "TCP keepalive interval option",
                cxxopts::value<int>())("backlog",
                                       "Backlog option",
                                       cxxopts::value<int>())(
                "linger",
                "Linger option",
                cxxopts::value<int>())("send_buffer_size",
                                       "Send buffer size option",
                                       cxxopts::value<int>())(
                "receive_buffer_size",
                "Receive buffer size option",
                cxxopts::value<int>())("send_high_watermark",
                                       "Send high watermark option",
                                       cxxopts::value<int>())(
                "receive_high_watermark",
                "Receive high watermark option",
                cxxopts::value<int>());

            std::vector<std::string> optionTokens;
            std::istringstream iss(option);
            std::string token;
            while (std::getline(iss, token, ','))
            {
                optionTokens.push_back(token);
            }

            std::vector<char *> fake_argv;
            fake_argv.push_back(const_cast<char *>("play_socket"));
            for (const auto &opt : optionTokens)
            {
                fake_argv.push_back(const_cast<char *>(opt.c_str()));
            }

            auto result = options.parse(fake_argv.size(), fake_argv.data());

            // 파싱된 값을 멤버 변수에 설정
            _immediate = result["immediate"].as<bool>();
            _routerHandOver = result["router_handover"].as<bool>();
            _routerMandatory = result["router_mandatory"].as<bool>();
            _tcpKeepAlive = result["tcp_keepalive"].as<int>();
            _tcpKeepAliveCount = result["tcp_keepalive_count"].as<int>();
            _tcpKeepAliveInterval = result["tcp_keepalive_interval"].as<int>();
            _backLog = result["backlog"].as<int>();
            _linger = result["linger"].as<int>();
            _sendBufferSize = result["send_buffer_size"].as<int>();
            _receiveBufferSize = result["receive_buffer_size"].as<int>();
            _sendHighWatermark = result["send_high_watermark"].as<int>();
            _receiveHighWatermark = result["receive_high_watermark"].as<int>();
        }
        catch (std::exception ex)
        {
            Log::error(
                std::format("socket option parsing error ex:{}", ex.what()),
                typeid(this).name());
        }

        Log::info(toString(), typeid(this).name());
    }

    virtual ~SocketConfig()
    {
    }

    bool immediate() const
    {
        return _immediate;
    }

    bool routerHandOver() const
    {
        return _routerHandOver;
    }

    bool routerMandatory() const
    {
        return _routerMandatory;
    }

    int32_t tcpKeepAlive() const
    {
        return _tcpKeepAlive;
    }

    int32_t tcpKeepAliveCount() const
    {
        return _tcpKeepAliveCount;
    }

    int32_t tcpKeepAliveInterval() const
    {
        return _tcpKeepAliveInterval;
    }

    int32_t backLog() const
    {
        return _backLog;
    }

    int32_t linger() const
    {
        return _linger;
    }

    int32_t sendBufferSize() const
    {
        return _sendBufferSize;
    }

    int32_t receiveBufferSize() const
    {
        return _receiveBufferSize;
    }

    int32_t sendHighWatermark() const
    {
        return _sendHighWatermark;
    }

    int32_t receiveHighWatermark() const
    {
        return _receiveHighWatermark;
    }

    std::string toString() const
    {
        return std::format("SocketConfig:\n"
                           "  immediate: {}\n"
                           "  routerHandOver: {}\n"
                           "  routerMandatory: {}\n"
                           "  tcpKeepAlive: {}\n"
                           "  tcpKeepAliveCount: {}\n"
                           "  tcpKeepAliveInterval: {}\n"
                           "  backLog: {}\n"
                           "  linger: {}\n"
                           "  sendBufferSize: {}\n"
                           "  receiveBufferSize: {}\n"
                           "  sendHighWatermark: {}\n"
                           "  receiveHighWatermark: {}",
                           _immediate,
                           _routerHandOver,
                           _routerMandatory,
                           _tcpKeepAlive,
                           _tcpKeepAliveCount,
                           _tcpKeepAliveInterval,
                           _backLog,
                           _linger,
                           _sendBufferSize,
                           _receiveBufferSize,
                           _sendHighWatermark,
                           _receiveHighWatermark);
    }

private:
    bool _immediate = true;
    bool _routerHandOver = true;
    bool _routerMandatory = true;
    int32_t _tcpKeepAlive = 1;
    int32_t _tcpKeepAliveCount = 5;
    int32_t _tcpKeepAliveInterval = 1;
    int32_t _backLog = 1000;
    int32_t _linger = 0;
    int32_t _sendBufferSize = 1024 * 1024;
    int32_t _receiveBufferSize = 1024 * 1024;
    int32_t _sendHighWatermark = 1000000;
    int32_t _receiveHighWatermark = 1000000;
};


class RouterSocket
{
private:
    zmq::context_t _ctx{};
    zmq::socket_t _socket;
    const std::string _endpoint;
    const SocketConfig _config;
    RingBuffer _buffer{64 * 1024, 64 * 1024 * 2};

public:
    RouterSocket(const std::string &options, const std::string &address);
    ~RouterSocket();

    void bind();
    bool send(Play::RouterMessage &message);
    Play::RouterMessage *recv();
    void connect(const std::string &target);
    void disconnect(const std::string &target);

    std::unique_ptr<zmq::message_t> makeClientMessageBody(
        uint16_t bodySize,
        int16_t serviceId,
        int32_t msgId,
        int16_t msgSeq,
        int16_t errorCode,
        int8_t stageIndex,
        const unsigned char *body);
};
} // namespace Play
