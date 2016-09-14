#include "logger.h"
#include "raknet.h"
#include <raknet/RakPeerInterface.h>
#include <raknet/MessageIdentifiers.h>
#include <raknet/BitStream.h>
#include <raknet/RakSleep.h>

constexpr static const unsigned int max_client = 100; // TODO: get it from settingsmanager.

RaknetGateway::RaknetGateway(NetworkManager &network)
    : Gateway(network)
{
    setAlive(false);
}

RaknetGateway::~RaknetGateway()
{
    setAlive(false);
    infostream << "[RakNet]Waiting for the raknet thread.";
    m_thread.join();
    RakNet::RakPeerInterface::DestroyInstance(m_peer);
}

bool RaknetGateway::run(const char *addr,unsigned short port)
{
    infostream << "[RakNet]Gateway initilzating...";
    m_peer = RakNet::RakPeerInterface::GetInstance();
    if(m_peer == nullptr)
    {
        fatalstream << "[RakNet]RakPeerInterface failed!";
        return false;
    }
    auto sd = RakNet::SocketDescriptor(port,addr);
    sd.socketFamily = AF_INET; // IPv4
    RakNet::StartupResult ret = m_peer->Startup(max_client,&sd,1);
    m_peer->SetMaximumIncomingConnections(max_client);
    if(ret == RakNet::StartupResult::RAKNET_STARTED)
    {
        setAlive(true);
        m_thread = std::thread([this] {loop();});
        return true;
    }
    else
    {
        // TODO: throw an exception
        return false;
    }
}

void RaknetGateway::close()
{
    m_peer->Shutdown(300);
    setAlive(false);
}

void RaknetGateway::loop()
{
    RakNet::Packet* p = nullptr;
    while(isAlive())
    {
        for (p = m_peer->Receive(); p; m_peer->DeallocatePacket(p),p = m_peer->Receive())
        {
            switch(p->data[0])
            {
            case ID_REMOTE_DISCONNECTION_NOTIFICATION:
                debugstream << "Another client has disconnected";
                break;
            case ID_REMOTE_CONNECTION_LOST:
                debugstream << "Another client has lost the connection";
                break;
            case ID_REMOTE_NEW_INCOMING_CONNECTION:
                debugstream << "Another client has connected";
                break;
            case ID_CONNECTION_REQUEST_ACCEPTED:
            {
                // TODO: Make a `Connection` instance for the connection
                debugstream << "Connection request has been accepted";
                RakNet::BitStream bsOut;
                bsOut.Write(ID_USER_PACKET_ENUM);
                bsOut.Write("Hello world");
                m_peer->Send(&bsOut,HIGH_PRIORITY,RELIABLE_ORDERED,0,p->systemAddress,false);
            }
            break;
            case ID_NEW_INCOMING_CONNECTION:
                debugstream << "A connection is incoming";
                break;
            case ID_NO_FREE_INCOMING_CONNECTIONS:
                debugstream << "The server is full";
                break;
            case ID_DISCONNECTION_NOTIFICATION:
                debugstream << "A client has disconnected";
                break;
            case ID_CONNECTION_LOST:
                debugstream << "A client lost the connection";
                break;
            case ID_USER_PACKET_ENUM:
                break;
            default:
                debugstream << "Bad Package arrived";
                break;
            }
        }
        RakSleep(30);
    }
}