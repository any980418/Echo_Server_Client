#include "ErrorHandle.h"

void ErrorHandle(const char* errmsg)
{
    cout << errmsg << endl;
    cout << "errno : " << errno << endl;
    exit(1);
}