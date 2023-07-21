#include "stream_socket.hpp"

using namespace Play;

Session::Session(std::shared_ptr<StreamSocket> socket,
                 const std::shared_ptr<CppServer::Asio::TCPServer> &server)
    : _socket(socket), CppServer::Asio::TCPSession(server)
{
}


void Session::onConnected()
{
    _sid = static_cast<int64_t>(socket().native_handle());
    _parser = std::make_unique<StreamParser>(_sid);

    std::shared_ptr<Session> session =
        std::dynamic_pointer_cast<Session>(shared_from_this());
    _socket->addSession(_sid, session);


    Log::debug(std::format("session connected : {}", _sid),
               typeid(this).name());
    _socket->_recvBuffer.push(
        std::make_unique<ClientMessage>(_sid, MessageType::CONNECT));
}

void Session::onDisconnected()
{
    Log::debug(std::format("session disconnected : {}", _sid),
               typeid(this).name());

    _socket->_recvBuffer.push(
        std::make_unique<ClientMessage>(_sid, MessageType::DISCONNECT));

    _socket->removeSession(_sid);
}

void Session::onReceived(const void *buffer, size_t size)
{

    try
    {
        _parser->write(static_cast<const unsigned char *>(buffer), 0, size);

        auto messages = _parser->parse();

        for (auto &message : messages)
        {
            _socket->_recvBuffer.push(std::move(message));
        }
    }
    catch (std::exception ex)
    {
        Log::error(std::format("message exception occurred: {}", _sid),
                   typeid(this).name());
        Disconnect();
    }
}

void Session::onError(int32_t error,
                      const std::string &category,
                      const std::string &message)
{
    std::cout << "Chat TCP session caught an error with code " << error
              << " and category '" << category << "': " << message << std::endl;

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

StreamServer::StreamServer(
    std::shared_ptr<StreamSocket> stream_socket,
    const std::shared_ptr<CppServer::Asio::Service> &service,
    int32_t port,
    CppServer::Asio::InternetProtocol protocol)
    : _stream_socket(stream_socket),
      CppServer::Asio::TCPServer(service, port, protocol)
{
}

std::shared_ptr<CppServer::Asio::TCPSession> StreamServer::CreateSession(
    const std::shared_ptr<CppServer::Asio::TCPServer> &server)
{
    return std::make_shared<Session>(server);
}


void StreamServer::onError(int32_t error,
                           const std::string &category,
                           const std::string &message)
{
    Log::error(std::format("tcp stream server exception occurred with code: "
                           "error:{},category:{},message:{}",
                           error,
                           category,
                           message),
               typeid(this).name());
}


////////////// StreamSocket /////////////////////

StreamSocket::StreamSocket()
{
}
StreamSocket::~StreamSocket()
{
}
void StreamSocket::bind(int32_t port)
{
    _service = std::make_shared<CppServer::Asio::Service>();
    _service->Start();

    Log::info("stream service start!", typeid(this).name());


    _server =
        std::make_shared<StreamServer>(shared_from_this(), _service, port);

    _server->Start();

    Log::info("stream server start!", typeid(this).name());
}
void StreamSocket::close()
{

    if (_server != nullptr)
        _server->Stop();

    if (_service != nullptr)
        _service->Stop();
}
bool StreamSocket::send(Play::ClientMessage &&message)
{
    tbb::concurrent_hash_map<int64_t, std::shared_ptr<Session>>::const_accessor
        result;
    if (_sessions.find(result, message.sid()))
    {
        const std::shared_ptr<Session> session = result->second;
        auto msg = message.body();
        session->SendAsync(msg->data(), msg->size());
    }
    else
    {
        Log::debug(std::format("session is not exist {}", message.sid()),
                   typeid(this).name());
    }
}
std::unique_ptr<Play::ClientMessage> StreamSocket::recv()
{
    std::unique_ptr<Play::ClientMessage> recvMessage;

    if (_recvBuffer.try_pop(recvMessage))
    {
        return recvMessage;
    }
    return nullptr;
}

void StreamSocket::addSession(int64_t sid, std::shared_ptr<Session> session)
{
    _sessions.insert(make_pair(sid, session));
}
void StreamSocket::removeSession(int64_t sid)
{
    _sessions.erase(sid);
}
