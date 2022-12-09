#include "Service.h"
#include "Session.h"

int main()
{
    ServerService* service = new ServerService(
        new EpollCore(),
        CreateSession,
        100);

    if(service->Start() == false)
    {
        return -1;
    }

    while(true)
    {
        service->GetEpollCore()->Dispatch();
    }

    return 0;
}