#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include "IrcServer.hpp"

class Channel
{
public:
							Channel();
							Channel(t_command comm, UserConn &user);
							~Channel();
	void					setChannelName(std::string name) { this->channelName = name; }
	void					setTopic(std::string topicName) { this->topic = topicName; }
	void					setChannelPass(std::string pass) { this->channelPass = pass; }
	void					addUser(UserConn *);
	void					updateMember(std::vector<UserConn *> var) {this->channelUsers = var;}
	int						getUserCount() { return channelUsers.size(); }
	std::string				getChannelName() { return channelName; }
	std::string				getTopic() { return topic; }
	std::string				getChannelPass() { return channelPass; }
	std::vector<UserConn *> &getUserVector() {return (this->channelUsers);}

private:
	std::vector<UserConn *>	channelUsers;
	std::string				channelName;
	std::string				topic;
	std::string				channelPass;
};

#endif // CHANNEL_HPP