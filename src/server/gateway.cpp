#include "gateway.h"

Gateway::Gateway(NetworkManager &network)
    :m_network(network),m_alive(false)
{
}
