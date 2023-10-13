#include "IrcServer.hpp"

UserConn::UserConn(int fd, std::string address, int port)
{
    this->fd = fd;
    this->address = address;
    this->port = port;
    this->checkLogin = 0;
    username = "";
    nickname = "";
}

UserConn::~UserConn()
{
    close(fd);
}

void UserConn::updateLogin(int param) 
{ 
    this->checkLogin = param;
}

void UserConn::setUserName(std::string username) 
{ 
    this->username = username;
    this->fullname = this->getNickname() + "!" + this->getUserName() + "@" + this->getAddress();
}

void UserConn::setHostName(std::string address) 
{ 
    this->address = address; 
    this->fullname = this->getNickname() + "!" + this->getUserName() + "@" + this->getAddress();
}

void UserConn::setNickName(std::string nickname) 
{ 
    this->nickname = nickname;
    this->fullname = this->getNickname() + "!" + this->getUserName() + "@" + this->getAddress();

}

void UserConn::setRealName(std::string realname) 
{ 
    this->realname = realname;
    this->fullname = this->getNickname() + "!" + this->getUserName() + "@" + this->getAddress();
}
