#ifndef NETWORK_H__
#define NETWORK_H__

#include <packet.h>
#include "network.h"
#include "session.h"
#include <networkstructures.h>
#include <thread>

class Connection
{
public:
    Connection(std::string ip, int port) :m_hostIP(ip), m_port(port) {}
    ~Connection() { disconnect(); m_thread.join(); }

    void connect() { m_thread = std::thread([this] { mainLoop(); }); connected = true; }
    void disconnect() { if (connected) m_ioService.stop(); }
    void addRequest(Packet& p) { m_session->addRequest(p); }

private:
    void mainLoop();
    bool init(boost::asio::ip::tcp::socket& socket);

    boost::asio::io_service m_ioService;
    std::shared_ptr<Session> m_session;
    std::thread m_thread;

    bool connected = false;

    const std::string m_hostIP;
    const int m_port;
};

#endif // NETWORK_H__
