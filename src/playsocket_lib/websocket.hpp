#pragma once

#include <iostream>
#include <server/ws/ws_server.h>
#include <tbb/concurrent_hash_map.h>
#include <tbb/concurrent_queue.h>
#include <thread>

#include "client_message.hpp"
#include "logger_interface.hpp"
#include "ring_buffer.hpp"
#include "stream_parser.hpp"

namespace Play
{

class WSStreamSocket;

class WSSession : public CppServer::WS::WSSession
{
private:
    int64_t _sid = 0;

    std::shared_ptr<WSStreamSocket> _streamSocket;
    std::unique_ptr<StreamParser> _parser;

public:
    using CppServer::WS::WSSession::WSSession;

    explicit WSSession(std::shared_ptr<WSStreamSocket> socket,
                       const std::shared_ptr<CppServer::WS::WSServer> &server);

protected:
    void onWSConnected(const CppServer::HTTP::HTTPRequest &request) override;
    void onWSDisconnected() override;


    void onWSReceived(const void *buffer, size_t size) override;

    void onWSPing(const void *buffer, size_t size) override;

    void onError(int error,
                 const std::string &category,
                 const std::string &message) override;
};

class WSStreamSocket : public std::enable_shared_from_this<WSStreamSocket>
{
public:
    friend class WSSession;
    WSStreamSocket();
    virtual ~WSStreamSocket();
    void bind(int32_t port);
    void close();
    bool send(ClientMessage &&message);
    std::unique_ptr<ClientMessage> recv();

    void addSession(int64_t sid, std::shared_ptr<WSSession> session);
    void removeSession(int64_t sid);

private:
    tbb::concurrent_queue<std::unique_ptr<ClientMessage>> _recvBuffer{};
    tbb::concurrent_hash_map<int64_t, std::shared_ptr<WSSession>> _sessions{};
    std::shared_ptr<CppServer::Asio::Service> _service;
    std::shared_ptr<CppServer::Asio::TCPServer> _server;
};


class WSStreamServer : public CppServer::WS::WSServer
{
    using CppServer::WS::WSServer::WSServer;

private:
    std::shared_ptr<WSStreamSocket> _socket;


public:
    WSStreamServer(std::shared_ptr<WSStreamSocket> socket,
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
