#ifndef USERCONN_HPP
# define USERCONN_HPP

# include <iostream>
# include <string>
# include <vector>

class UserConn
{
public:
                    UserConn(int fd, std::string address, int port);
                    ~UserConn();
    int             check() { return this->checkLogin; }
    int             get_fd() const { return fd; }
    std::string     getAddress() const { return address; }
    std::string     getNickname() const { return nickname; }
    std::string     getRealName() const { return realname; }
    std::string     getFullname() const { return fullname; }
    std::string     getUserName() const { return username; }
    int             getPort() const { return port; }
    void            updateLogin(int param);
    void            setUserName(std::string username);
    void            setHostName(std::string address);
    void            setNickName(std::string nickname);
    void            setRealName(std::string realname);
    std::string     buff;
private:
    int         checkLogin;
    int         fd;
    int         port;
    std::string address;
    std::string username;
    std::string nickname;
    std::string realname;
    std::string fullname;
};

#endif //USERCONN_HPP
