#include "UserMng.h"

User& UserMng::FindUser(int fd)
{
    return _users.find(fd)->second;
}

void UserMng::AddUser(int fd)
{
    User user(fd);
    _users.insert(make_pair(fd, user));
}

void UserMng::DeleteUser(int fd)
{
    _users.erase(fd);
}