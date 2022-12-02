#include "Service.h"
#include "Session.h"

/*----------------
      Service
-----------------*/

Service::Service(ServiceType type, EpollCore* core, SessionFactory factory, int maxSessioinCount)
    : _type(type), _epollCore(core), _sessionFactory(factory), _maxSessionCount(maxSessioinCount)
{

}

Service::~Service()
{

}

void Service::CloseService()
{
    // TODO
}

Session* Service::CreateSession(int serverSocket)
{
    Session* session = _sessionFactory(serverSocket);

    if(_epollCore->Register(reinterpret_cast<EpollObject*>(session), EPOLLIN | EPOLLET) == false)
        return nullptr;
    
    return session;
}

void Service::AddSession(Session* session)
{
    _sessionCount++;
    _sessions.insert(make_pair(session->GetSocket(), session));
}

void Service::ReleaseSession(Session* session)
{
    _sessions.erase(session->GetSocket());
    _sessionCount--;
}

/*----------------
   ClientService
-----------------*/

ClientService::ClientService(EpollCore* core, SessionFactory factory, int maxSessioinCount)
    : Service(ServiceType::Client, core, factory, maxSessioinCount)
{

}

bool ClientService::Start()
{
    // TODO
    return true;
}

/*----------------
   ServerService
-----------------*/

ServerService::ServerService(EpollCore* core, SessionFactory factory, int maxSessioinCount)
    : Service(ServiceType::Server, core, factory, maxSessioinCount)
{

}

bool ServerService::Start()
{
    if(CanStart() == false)
        return false;

    _listener = new Listener();
    if(_listener == nullptr)
        return false;

    if(_listener->StartAccept(this) == false)
        return false;

    return true;
}

void ServerService::CloseService()
{
    Service::CloseService();
}