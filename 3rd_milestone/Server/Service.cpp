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

Session* Service::CreateSession()
{
    Session* session = _sessionFactory();
    session->SetService(this);
    AddSession(session);

    return session;
}

void Service::AddSession(Session* session)
{
    _sessionCount++;
    _sessions.insert(session);
}

void Service::ReleaseSession(Session* session)
{
    _sessions.erase(session);
    _sessionCount--;
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

    ServerService* service = this;
    if(_listener->StartAccept(service) == false)
        return false;

    return true;
}

void ServerService::CloseService()
{
    Service::CloseService();
}


Session* ServerService::ConnectSession(int serverSocket)
{
    Session* session = _sessionFactory();
    session->SetService(this);
    session->Connect(serverSocket);
    AddSession(session);

    if(_epollCore->Register(reinterpret_cast<EpollObject*>(session), EPOLLIN | EPOLLET) == false)
        return nullptr;
    
    return session;
}