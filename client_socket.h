//
// Created by artem on 06.02.2025.
//

#ifndef CLIENT_SOCKET_H
#define CLIENT_SOCKET_H
#include <basetsd.h>
#include <psdk_inc/_socket_types.h>


class client_socket {
    public:
    SOCKET client = INVALID_SOCKET;
    void connect(SOCKET l_sock);
    void disconnect();
};



#endif //CLIENT_SOCKET_H
