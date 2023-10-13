#ifndef IRC_SERVER_HPP
#define IRC_SERVER_HPP

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <string.h>
#include <stdio.h>
#include "UserConn.hpp" //used to be #include <UserConn.h> why angle brackets?
#include <stdlib.h> 
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "parser.hpp"
#include <fcntl.h>
#include <algorithm>
#include "Channel.hpp"

typedef enum checkLogin
{
	e_begin = 0,
	e_pass = 1,
	e_nickname = 2,
	e_user = 3,
	e_success = 4,
	mnl = 16,
	mnll = 32,
	mnnll = 64,
	mmnnll = 128
} e_login;

class IrcServer
{
public:
							IrcServer(int port, std::string);
							~IrcServer();
	void					start();
	std::vector<UserConn *>	getUserConns() { return (user_conns); }

private:
	// Add new channel to irc server
	void							addNewChannel(UserConn *, t_command);
	// All user function here (e.g JOIN)
	void							user_process(UserConn *, t_command);
	// All login function here
	void							login_process(UserConn *, t_command);
	// Process a new connection.
	void							process_new_connection();
	// Process input from a connection.
	void							process_input(UserConn *uc);
	// Add a user connection to the server.
	void							add_user_conn(int fd, std::string address, int port);
	// Remove a user connection from the server.
	void							remove_user_conn(int fd);
	// Send a message to all connected users.
	void							send_all(std::string message);
	// Send a message to a specific user.
	void							send_user(int fd, std::string message);
	// Check channel availability.
	void							privMSG(UserConn *uc, t_command comm);
	void							privMSG(UserConn *uc, t_command comm, std::string channel_name);
	void							noticeMSG(UserConn *uc, t_command comm);
	void							kick(UserConn *uc, t_command comm);
	int								port;
	int								server_fd;
	std::vector<UserConn *>			user_conns;
	std::map<std::string, Channel>	channelList;
	std::string						password;
};

bool check_channel(std::map<std::string, Channel> channels, std::string input);

#endif // IRC_SERVER_HPP
