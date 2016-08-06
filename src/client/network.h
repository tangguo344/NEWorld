#ifndef NETWORK_H__
#define NETWORK_H__

#include <networkshared.h>
#include <packet.h>

class Packet;
void addRequest(Packet p);
void networkThread();
void disconnect();

#endif // NETWORK_H__
