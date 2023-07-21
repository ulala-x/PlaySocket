#include "websocket.hpp"

using namespace Play;

WSSession::WSSession(std::shared_ptr<WSStreamSocket> socket,
                     const std::shared_ptr<CppServer::WS::WSServer> &server)
    : _streamSocket(socket), CppServer::WS::WSSession(server)
{
}


void WSSession::onWSConnected(const CppServer::HTTP::HTTPRequest &request)
{
    _sid = static_cast<int64_t>(socket().native_handle());
    _parser = std::make_unique<StreamParser>(_sid);

    std::shared_ptr<WSSession> session =
        std::dynamic_pointer_cast<WSSession>(shared_from_this());
    _streamSocket->addSession(_sid, session);


    Log::debug(std::format("session connected : {}", _sid),
               typeid(this).name());
    _streamSocket->_recvBuffer.push(
        std::make_unique<ClientMessage>(_sid, MessageType::CONNECT));
}

void WSSession::onWSDisconnected()
{
    Log::debug(std::format("session disconnected : {}", _sid),
               typeid(this).name());

    _streamSocket->_recvBuffer.push(
        std::make_unique<ClientMessage>(_sid, MessageType::DISCONNECT));

    _streamSocket->removeSession(_sid);
}

void WSSession::onWSReceived(const void *buffer, size_t size)
{

    try
    {
        _parser->write(static_cast<const unsigned char *>(buffer), 0, size);

        auto messages = _parser->parse();

        for (auto &message : messages)
        {
            _streamSocket->_recvBuffer.push(std::move(message));
        }
    }
    catch (std::exception ex)
    {
        Log::error(std::format("message exception occurred: {}", _sid),
                   typeid(this).name());
        Disconnect();
    }
}

void WSSession::onWSPing(const void *buffer, size_t size)
{
    SendPongAsync(buffer, size);
}

void WSSession::onError(int32_t error,
                        const std::string &category,
                        const std::string &message)
{

    Log::error(std::format("message exception occurred with code: "
                           "sid:{},error:{},category:{},message:{}",
                           _sid,
                           error,
                           category,
                           message),
               typeid(this).name());
    Disconnect();
}


////////////////////////////////////TcpStreamServer///////////////////////////

WSStreamServer::WSStreamServer(
    std::shared_ptr<WSStreamSocket> socket,
    const std::shared_ptr<CppServer::Asio::Service> &service,
    int32_t port,
    CppServer::Asio::InternetProtocol protocol)
    : _socket(socket), CppServer::WS::WSServer(service, port, protocol)
{
}

std::shared_ptr<CppServer::Asio::TCPSession> WSStreamServer::CreateSession(
    const std::shared_ptr<CppServer::Asio::TCPServer> &server)
{
    return std::make_shared<WSSession>(
        std::dynamic_pointer_cast<CppServer::WS::WSServer>(server));
}


void WSStreamServer::onError(int32_t error,
                             const std::string &category,
                             const std::string &message)
{
    Log::error(std::format("wsStream server exception occurred with code: "
                           "error:{},category:{},message:{}",
                           error,
                           category,
                           message),
               typeid(this).name());
}


////////////// StreamSocket /////////////////////

WSStreamSocket::WSStreamSocket()
{
}
WSStreamSocket::~WSStreamSocket()
{
}
void WSStreamSocket::bind(int32_t port)
{
    _service = std::make_shared<CppServer::Asio::Service>();
    _service->Start();

    Log::info("stream service start!", typeid(this).name());


    _server =
        std::make_shared<WSStreamServer>(shared_from_this(), _service, port);

    _server->Start();

    Log::info("stream server start!", typeid(this).name());
}
void WSStreamSocket::close()
{

    if (_server != nullptr)
        _server->Stop();

    if (_service != nullptr)
        _service->Stop();
}
bool WSStreamSocket::send(Play::ClientMessage &&message)
{
    tbb::concurrent_hash_map<int64_t,
                             std::shared_ptr<WSSession>>::const_accessor result;
    if (_sessions.find(result, message.sid()))
    {
        const std::shared_ptr<WSSession> session = result->second;
        auto msg = message.body();
        session->SendAsync(msg->data(), msg->size());
    }
    else
    {
        Log::debug(std::format("session is not exist {}", message.sid()),
                   typeid(this).name());
    }
}
std::unique_ptr<Play::ClientMessage> WSStreamSocket::recv()
{
    std::unique_ptr<Play::ClientMessage> recvMessage;

    if (_recvBuffer.try_pop(recvMessage))
    {
        return recvMessage;
    }
    return nullptr;
}

void WSStreamSocket::addSession(int64_t sid, std::shared_ptr<WSSession> session)
{
    _sessions.insert(make_pair(sid, session));
}
void WSStreamSocket::removeSession(int64_t sid)
{
    _sessions.erase(sid);
}
