#ifndef PARSER_HPP
#define PARSER_HPP

#include <iostream>
#include <string>
#include <vector>

 typedef struct Command {
    std::string name;
    std::vector<std::string> params;
    std::string args;
} t_command;

t_command parseCommand(const std::string& );

#endif // PARSER_HPP