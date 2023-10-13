#include "IrcServer.hpp"

Channel::Channel(){}

Channel::Channel(t_command comm, UserConn &user)
{
    this->channelName = comm.params[0];
    if (comm.params.size() == 2){
        this->channelPass = comm.params[1];
    }
    else{
        this->channelPass = "";
    }
    this->channelUsers.push_back(&user);
}

Channel::~Channel(){}

bool check_channel(std::map<std::string , Channel> channels, std::string input)
{
    for (std::map<std::string , Channel>::iterator it = channels.begin(); it != channels.end(); ++it){
        if(it->second.getChannelName() == input){
            return(false);
        }
    }
    return(true);
}

void Channel::addUser(UserConn *uc){
    this->channelUsers.push_back(uc);
}