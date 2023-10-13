#include "parser.hpp"
#include <algorithm>

t_command parseCommand(const std::string& commandStr)
{
    t_command cmd;
    size_t start = 0;
    size_t end = commandStr.find(" ", start);
    if (end == std::string::npos) {
        end = commandStr.length();
    }
    cmd.name = commandStr.substr(start, end - start);
    std::transform(cmd.name.begin(), cmd.name.end(), cmd.name.begin(), toupper);
    start = end + 1;
    while (commandStr[start] && commandStr[start] == ' ')
		start++;
    while (start < commandStr.length() && commandStr[start] != ':')
    {
        end = commandStr.find(" ", start);
        if (end == std::string::npos) {
            end = commandStr.length();
        }
        cmd.params.push_back(commandStr.substr(start, end - start));
        start = end + 1;
        while (commandStr[start] && commandStr[start] == ' ')
				start++;
    }
    while (commandStr[start] && commandStr[start] == ' ')
		start++;
    if (start < commandStr.length())
        cmd.args = commandStr.substr(start + 1);
    return cmd;
}
