#ifndef MACRO_HPP
# define MACRO_HPP
# include <string>
# include "UserConn.hpp"

// std::string RPL_WELCOME(UserConn *uc) {return("001 " + uc->getNickname() + " :Welcome to the Internet Relay Network " + uc->getNickname() + "!" + uc->getUserName() + "@" + uc->getAddress());}
// std::string ERR_NEEDMOREPARAMS(std::string command) {return ("461 " + command + " :Not enough parameters");}
// std::string ERR_ALREADYREGISTRED() {return ("462 :Unauthorized command (already registered)");}
// std::string ERR_NONICKNAMEGIVEN() {return ("431 :No nickname given");} // yok zaten
// std::string ERR_NICKNAMEINUSE(std::string nick) { return ("433 " + nick + " :Nickname is already in use");} // yok zaten
// std::string ERR_ERRONEUSNICKNAME(std::string nick) {return ("432 " + nick + " :Erroneous nickname (Not Invalid Character)");}
// std::string ERR_NOSUCHNICK(std::string nick) {return ("401 " + nick + " :No such nick/channel");}
// std::string ERR_PASSWDMISMATCH() {return ("464 :Password incorrect");}
// std::string ERR_RESTRICTED() {return ("484    :Your connection is restricted!");}

#endif // MACRO_HPP