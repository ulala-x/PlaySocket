#pragma once

#include <iostream>
#include <server/asio/tcp_server.h>
#include <tbb/concurrent_hash_map.h>
#include <tbb/concurrent_queue.h>
#include <thread>

#include "client_message.hpp"
#include "logger_interface.hpp"
#include "ring_buffer.hpp"
#include "stream_parser.hpp"

namespace Play
{

class StreamSocket;

class Session : public CppServer::Asio::TCPSession
{
private:
    int64_t _sid = 0;

    std::shared_ptr<Play::StreamSocket> _socket;
    std::unique_ptr<Play::StreamParser> _parser;

public:
    using CppServer::Asio::TCPSession::TCPSession;

    explicit Session(std::shared_ptr<Play::StreamSocket> socket,
                     const std::shared_ptr<CppServer::Asio::TCPServer> &server);

protected:
    void onConnected() override;
    void onDisconnected() override;


    void onReceived(const void *buffer, size_t size) override;

    void onError(int error,
                 const std::string &category,
                 const std::string &message) override;
};

class StreamSocket : public std::enable_shared_from_this<StreamSocket>
{
public:
    friend class Session;
    StreamSocket();
    virtual ~StreamSocket();
    void bind(int32_t port);
    void close();
    bool send(Play::ClientMessage &&message);
    std::unique_ptr<Play::ClientMessage> recv();

    void addSession(int64_t sid, std::shared_ptr<Session> session);
    void removeSession(int64_t sid);

private:
    tbb::concurrent_queue<std::unique_ptr<ClientMessage>> _recvBuffer{};
    tbb::concurrent_hash_map<int64_t, std::shared_ptr<Session>> _sessions{};
    std::shared_ptr<CppServer::Asio::Service> _service;
    std::shared_ptr<CppServer::Asio::TCPServer> _server;
};


class StreamServer : public CppServer::Asio::TCPServer
{

private:
    std::shared_ptr<StreamSocket> _stream_socket;


public:
    StreamServer(std::shared_ptr<StreamSocket> socket,
                 const std::shared_ptr<CppServer::Asio::Service> &service,
                 int32_t port,
                 CppServer::Asio::InternetProtocol protocol =
                     CppServer::Asio::InternetProtocol::IPv4);


protected:
    std::shared_ptr<CppServer::Asio::TCPSession> CreateSession(
        const std::shared_ptr<CppServer::Asio::TCPServer> &server) override;

protected:
    void onError(int32_t error,
                 const std::string &category,
                 const std::string &message) override;
};


} // namespace Play
