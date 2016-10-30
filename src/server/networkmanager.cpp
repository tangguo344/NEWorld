#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "logger.h"
#include "networkmanager.h"
#include <raknet/MessageIdentifiers.h>
#include <raknet/BitStream.h>
#include <raknet/RakSleep.h>

constexpr static const unsigned int max_client = 100; // TODO: get it from settingsmanager.

NetworkManager::NetworkManager()
{
    infostream << "Raknet initializating...";
    mPeer = RakNet::RakPeerInterface::GetInstance();
    infostream << "Raknet initialized.";
}

NetworkManager::~NetworkManager()
{
    infostream << "Waiting for the raknet thread.";
    mThread.join();
    RakNet::RakPeerInterface::DestroyInstance(mPeer);
}

bool NetworkManager::run(const char *addr,unsigned short port)
{
    if(mPeer == nullptr)
    {
        fatalstream << "Get RakPeerInterface failed!";
        return false;
    }
    auto sd = RakNet::SocketDescriptor(port, addr);
    sd.socketFamily = AF_INET; // IPv4
    RakNet::StartupResult ret = mPeer->Startup(max_client,&sd,1);
    mPeer->SetMaximumIncomingConnections(max_client);
    mConns.reserve(max_client);
    if(ret == RakNet::StartupResult::RAKNET_STARTED)
    {
        mThread = std::thread([this] {loop();});
        return true;
    }
    else
    {
        fatalstream << "Failed to start Network Manager. Error code: " << ret;
        assert(false);
        return false;
    }
}

void NetworkManager::close()
{
    mPeer->Shutdown(300);
}

void NetworkManager::loop()
{
    RakNet::Packet* p = nullptr;
    while(mPeer->IsActive())
    {
        for (p = mPeer->Receive(); p; mPeer->DeallocatePacket(p),p = mPeer->Receive())
        {
            switch(p->data[0])
            {
            case ID_USER_PACKET_ENUM:
                break;
            case ID_NEW_INCOMING_CONNECTION:
            {
                newConnection(p->systemAddress);
                break;
            }
            case ID_DISCONNECTION_NOTIFICATION:
            case ID_CONNECTION_LOST:
                deleteConnection(p->systemAddress);
                break;
            default:
                warningstream << "Unexcepted packet is received. Packet ID:" << static_cast<int>(p->data[0])
                              << " From " << p->systemAddress.ToString(true);
                break;
            }
        }
        RakSleep(30);
    }
}

Connection* NetworkManager::newConnection(RakNet::SystemAddress addr)
{
    Connection *c = new Connection(*this,mPeer,addr);
    mConns.insert(mConns.begin()+mPeer->GetIndexFromSystemAddress(addr), c);
    return c;
}

void NetworkManager::deleteConnection(RakNet::SystemAddress addr)
{
    delete mConns[mPeer->GetIndexFromSystemAddress(addr)];
}

Connection::Connection(NetworkManager &network,RakNet::RakPeerInterface *peer, RakNet::SystemAddress addr)
    :mNetwork(network),mPeer(peer),mAddr(addr)
{
    infostream << inet_ntoa(mAddr.address.addr4.sin_addr) << ':' << mAddr.address.addr4.sin_port << " connected.";
}

Connection::~Connection()
{
    infostream << inet_ntoa(mAddr.address.addr4.sin_addr) << ':' << mAddr.address.addr4.sin_port << " disconnected.";
}

void Connection::sendRawData(RakNet::MessageID id, const unsigned char *data, int len, PacketPriority priority, PacketReliability reliability)
{
    RakNet::BitStream bsOut;
    bsOut.Write(id);
    bsOut.WriteBits(data, len);
    mPeer->Send(&bsOut, priority, reliability, 0, mAddr, false);
}
