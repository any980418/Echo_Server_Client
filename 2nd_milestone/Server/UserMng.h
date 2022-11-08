#pragma once

#include <map>
#include <iostream>
using namespace std;

#include "User.h"

class UserMng
{
public:
    User& FindUser(int fd);
    void AddUser(int fd);
    void DeleteUser(int fd);

protected:
    map<int, User> _users;
};