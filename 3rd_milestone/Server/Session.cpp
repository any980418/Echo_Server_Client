#include "Session.h"
#include "SocketUtils.h"

/*----------------
      Session
-----------------*/

Session::Session(int serverSocket)
{
    int clientSocket = SocketUtils::Accept(serverSocket);
    SocketUtils::SetNonblockingMode(clientSocket);
    _socket = clientSocket;
}

Session::~Session()
{
    SocketUtils::Close(_socket);
}

int Session::GetSocket()
{
    return _socket;
}

void Session::Dispatch()
{
    // TODO
}