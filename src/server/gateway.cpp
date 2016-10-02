#include "logger.h"
#include "gateway.h"
#include "networkmanager.h"
#include <raknet/MessageIdentifiers.h>
#include <raknet/BitStream.h>
#include <raknet/RakSleep.h>

constexpr static const unsigned int max_client = 100; // TODO: get it from settingsmanager.

Gateway::Gateway(NetworkManager &network)
    :mNetwork(network)
{
}

Gateway::~Gateway()
{
    infostream << "Waiting for the raknet thread.";
    mThread.join();
    RakNet::RakPeerInterface::DestroyInstance(mPeer);
}

bool Gateway::run(const char *addr,unsigned short port)
{
    infostream << "Raknet initializating...";
    mPeer = RakNet::RakPeerInterface::GetInstance();
    if(mPeer == nullptr)
    {
        fatalstream << "Get RakPeerInterface failed!";
        return false;
    }
    auto sd = RakNet::SocketDescriptor(port,addr);
    sd.socketFamily = AF_INET; // IPv4
    RakNet::StartupResult ret = mPeer->Startup(max_client,&sd,1);
    mPeer->SetMaximumIncomingConnections(max_client);
    if(ret == RakNet::StartupResult::RAKNET_STARTED)
    {
        mThread = std::thread([this] {loop();});
        return true;
    }
    else
    {
        // TODO: throw an exception
        return false;
    }
}

void Gateway::close()
{
    mPeer->Shutdown(300);
}

void Gateway::loop()
{
    RakNet::Packet* p = nullptr;
    while(mPeer->IsActive())
    {
        for (p = mPeer->Receive(); p; mPeer->DeallocatePacket(p),p = mPeer->Receive())
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
                // TODO: Create a `Connection` instance for the connection
                debugstream << "Connection request has been accepted";
                RakNet::BitStream bsOut;
                bsOut.Write(ID_USER_PACKET_ENUM);
                bsOut.Write("Hello world");
                mPeer->Send(&bsOut,HIGH_PRIORITY,RELIABLE_ORDERED,0,p->systemAddress,false);
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