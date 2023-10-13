#include <IrcServer.hpp>
#include <macro.hpp>

bool nick_control(std::string nickname)
{
    if (nickname.length() < 1)
        return (false);
    for (int i = 0; i < static_cast<int>(nickname.length()); i++)
    {
        if ((nickname[i] >= 0 && nickname[i] <= 47)) // should we make this so only numbers and english letters are allowed?
            return (false);
    }
    return (true);
}

bool check_nick(std::vector<UserConn *> user_conns, std::string input)
{
    for( std::vector<UserConn *>::iterator it = user_conns.begin(); it != user_conns.end() ; ++it){
        if((*it)->getNickname() == input){
            return(false);
        }
    }
    return(true);
}

IrcServer::IrcServer(int port, std::string password) : port(port), server_fd(-1), password(password)
{
}

IrcServer::~IrcServer()
{
    if (server_fd >= 0)
        close(server_fd);
    std::vector<UserConn *>::iterator it;
    for (it = user_conns.begin(); it != user_conns.end(); ++it) {
        delete *it;
    }
}

void IrcServer::start()
{
    int opt = 1;
    this->server_fd = socket(AF_INET, SOCK_STREAM, 0);
    fcntl(this->server_fd, F_SETFL, O_NONBLOCK);
    if (this->server_fd < 0)
        throw std::runtime_error("failed to create server socket");
    sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(port);
    if (setsockopt(this->server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
        throw std::runtime_error("failed to set socket options");
    if (bind(this->server_fd, reinterpret_cast<sockaddr *>(&server_addr), sizeof(server_addr)) < 0)
        throw std::runtime_error("failed to bind server socket to port");
    else 
        std::cout << "success"<< std::endl;
    if (listen(this->server_fd, 5) < 0) 
        throw std::runtime_error("listening error");
    while (true)
    {
        fd_set read_fds;
        FD_ZERO(&read_fds);
        FD_SET(this->server_fd, &read_fds);
        int max_fd = this->server_fd;

        std::vector<UserConn *>::iterator it;
        for ( it = user_conns.begin(); it != user_conns.end(); ++it)
        {
            UserConn *uc = *it;
            int fd = uc->get_fd();
            if (fd >= 0) {
                FD_SET(fd, &read_fds);
                max_fd = std::max(max_fd, fd);
            }
        }
        if (select(max_fd + 1, &read_fds, NULL, NULL, NULL) < 0)
        {
            std::perror("I/O : ");
            std::cerr << "ERROR: Unable to wait for I/O." << std::endl;
            return;
        }
        if (FD_ISSET(this->server_fd, &read_fds))
            process_new_connection();
        for (it = user_conns.begin(); it < user_conns.end(); ++it)
        {
            UserConn *uc = *it;
            int fd = uc->get_fd();
            std::cout << "      fd: " << fd << std::endl;
            if (fd >= 0 && FD_ISSET(fd, &read_fds))
                process_input(uc);
        }
    }
}


void IrcServer::login_process(UserConn *uc, t_command command){
    if (command.name == "PASS")
    {
        if(uc->check() == e_begin){
            if(this->password == "")
            {
                uc->updateLogin(e_pass);
                this->send_user(uc->get_fd(), "Server : Password Correct");
                return;
            }
            if (command.params.size() < 1 && command.args == "")
            {
                std::string message = "461 " + command.name + " :incorrect password";
                this->send_user(uc->get_fd(), message);
                return;
            }
            if(command.params.size() < 1)
                command.params.push_back(" ");
            if(command.args == this->password || command.params[0] == this->password) {
                uc->updateLogin(e_pass);
                this->send_user(uc->get_fd(), "Server : Password Correct");
                return;
            }
            else if (command.args != this->password || command.params[0] != this->password)
            {
                std::string message = "464 :Password incorrect";
                this->send_user(uc->get_fd(), message);
            }
        }
        else
        {
            std::string message = "462 :Unauthorized command (already registered)";
            this->send_user(uc->get_fd(), message);
            return;
        }
    }

    if (command.name == "NICK")
    {
        if (uc->check() < e_pass)
        {
            std::string message = "484 :Your connection is restricted!";
            this->send_user(uc->get_fd(), message);
        }
        else if (command.params.size() < 1 || !nick_control(command.params[0]))
        {
            std::string message = "432 " + command.params[0] + " :Erroneous nickname";
            this->send_user(uc->get_fd(), message); 
        }        
        else if(!check_nick(this->getUserConns(), command.params[0]))
        {
            std::string message = "433 " + command.params[0] + " :Nickname is already in use";
            this->send_user(uc->get_fd(), message);
        }
        else
        {
            uc->setNickName(command.params[0]);
            std::string successMessage = "Your nickname has been successfully set, " + uc->getNickname();
            this->send_user(uc->get_fd(),"Server :" + successMessage);
            uc->updateLogin(e_nickname);
        }

    }
    if (command.name == "USER")
    {
        if(uc->check() == e_begin)
            this->send_user(uc->get_fd(), "484 : Your connection is restricted!");
        else if(uc->check() == e_pass)
            this->send_user(uc->get_fd(), "Server : Your nickname is not set!");
        else if(uc->check() != e_nickname)
            this->send_user(uc->get_fd(), "462 : You're already in the server");
        else
        {
            if((command.params.size() == 3 && command.args != "") || (command.params.size() == 4 && command.args == "")) {
                uc->setUserName(command.params[0]);
                uc->setHostName("localhost");
                uc->setRealName(command.params.size() == 3 ? command.args : command.params[3]);
                uc->updateLogin(e_user);
            }
            else
                this->send_user(uc->get_fd(), "461 : Please use the command USER <username> * * <realname>.");
        }


    }
    if (uc->check() == e_user)
    {
        uc->updateLogin(e_success);
        std::string message = "001 " + uc->getNickname() + " :Welcome to the Internet Relay Network " + uc->getNickname() + "!" + uc->getUserName() + "@" + uc->getAddress();
        this->send_user(uc->get_fd(), message);
    }
    if (command.name == "QUIT"){
        remove_user_conn(uc->get_fd());
        return;
    }
}

void IrcServer::user_process(UserConn *uc, t_command command)
{
    if (uc->check() != (e_success) && (command.name != "PASS" && command.name != "NICK" && command.name != "USER" && command.name != "PING" && command.name != "CAP")){
        this->send_user(uc->get_fd(), "ERROR : You are not logged in");
        return ;
    }
    if (command.name == "CAP")
    {
        this->send_user(uc->get_fd(), "Server : Server command list :");
        this->send_user(uc->get_fd(), "Server : PASS    = To enter server password for server connection, must be sent before the NICK/USER registration combination.");
        this->send_user(uc->get_fd(), "Server : NICK    = To create or change IRC nickname.");
        this->send_user(uc->get_fd(), "Server : USER    = To specify the username, hostname and real name of the connecting client.");
        this->send_user(uc->get_fd(), "Server : JOIN    = To join a channel.");
        this->send_user(uc->get_fd(), "Server : PING    = To test the latency between two users.");
        this->send_user(uc->get_fd(), "Server : KICK    = To kick someone from a channel, can only be used by operators.");
        this->send_user(uc->get_fd(), "Server : PRIVMSG = To send private messages to other users.");
        this->send_user(uc->get_fd(), "Server : NOTICE  = To send private messages to other users that cannot be answered.");
        this->send_user(uc->get_fd(), "Server : LIST    = To list the channels in the server.");
        this->send_user(uc->get_fd(), "Server : QUIT    = To disconnect from the server.");
        this->send_user(uc->get_fd(), "Server : NAMES   = To list the users in a channel.");
        this->send_user(uc->get_fd(), "Server : MSG     = Same as privmsg.");
    }
    if (command.name == "JOIN")
    {
        if (command.params[0][0] != '#')
        {
            this->send_user(uc->get_fd(), "Server : Channel name first characther must be '#'");
            return ;
        }
        std::cout << "JOIN USER FD => " << uc->get_fd() << std::endl;
        if (command.params.size() < 1){
            std::string message = "461 : not enough parameters";
            this->send_user(uc->get_fd(), message);
        }
        if (check_channel(channelList, command.params[0]))
        {
            this->addNewChannel(uc, command);
            std::string message = ": you created "+ command.params[0];
            this->send_user(uc->get_fd(), message);
            this->send_user(uc->get_fd(),"353 = " + command.params[0] + " : " + uc->getNickname());
            this->send_user(uc->get_fd(), ":" + command.params[0] + " " + uc->getFullname() + " :Welcome to Channel " + command.params[0]);
        }
        else
        {
            std::vector<UserConn *> users = channelList.find(command.params[0])->second.getUserVector();
            for (std::vector<UserConn *>::iterator it = users.begin(); it != users.end(); ++it)
            {
                if ((*it)->get_fd() == uc->get_fd())
                {
                    this->send_user(uc->get_fd(), "443 : You already joined this channel " + command.params[0]);
                    return;
                }
            }
            if (channelList.find(command.params[0])->second.getChannelPass() != "")
            {
                if(command.params[1] == channelList.find(command.params[0])->second.getChannelPass())
                {
                    channelList.find(command.params[0])->second.addUser(uc);
                    this->send_user(uc->get_fd(), "353 = " + command.params[0] + " : " + uc->getNickname());
                    this->send_user(uc->get_fd(), ":" + command.params[0] + " " + uc->getFullname() + " :Welcome to Channel " + command.params[0]);
                    for(std::vector<UserConn *>::iterator it = users.begin(); it != users.end(); ++it)
                    {
                        std::string message = uc->getNickname() + " joined channel " + command.params[0];
                        this->send_user((*it)->get_fd(), message);
                    }
                }
                else
                    this->send_user(uc->get_fd(), "475 :" + command.params[0] + ":Cannot join channel");
            }
            else
            {
                channelList.find(command.params[0])->second.addUser(uc);
                this->send_user(uc->get_fd(), "332 " + command.params[0]+ " : " + channelList.find(command.params[0])->second.getTopic());
                this->send_user(uc->get_fd(), "353 = " + command.params[0] + " : " + uc->getNickname());
                this->send_user(uc->get_fd(), ":" + command.params[0] + " " + uc->getFullname() + " :Welcome to Channel " + command.params[0]);
                for(std::vector<UserConn *>::iterator it = users.begin(); it != users.end(); ++it)
                {
                    std::string message = uc->getNickname() + " joined channel " + command.params[0];
                    this->send_user((*it)->get_fd(), message);
                }
            }
        }
    }
    if (command.name == "TOPIC")
    {
        bool sender_in_channel =0;
        if (command.params.size() < 1){
            std::string message = "461 : not enough parameters";
            this->send_user(uc->get_fd(), message);
            return ;
        }
         if (command.params[0][0] != '#')
        {
            this->send_user(uc->get_fd(), "Server : Channel name first characther must be '#'");
            return ;
        }
        std::map<std::string , Channel>::iterator channelit = channelList.find(command.params[0]);
        std::vector<UserConn *> users =  (*channelit).second.getUserVector();
        for (std::vector<UserConn *>::iterator it = users.begin(); it != users.end(); ++it){
            if (uc->get_fd() == (*it)->get_fd()){
                sender_in_channel = 1;
                break;
            }
        }
        if (!sender_in_channel)
        {
            std::string message = "442 " + command.params[0] + ": You're not on that channel ";
            this->send_user(uc->get_fd(), message);
            return ;
        }
        else
        {
            if(command.params[1] != "")
                channelit->second.setTopic(command.params[1]);
            if (command.params[1] != "")
                this->send_user(uc->get_fd(), "332 :" + channelit->second.getChannelName() + " :" + command.params[1]);
            else
                this->send_user(uc->get_fd(), "331 :" + channelit->second.getChannelName() + " :No topic is set");
        }
    }
    if (command.name == "PRIVMSG" || command.name == "MSG"){
        if (command.params.size() < 1)
        {
            std::string message = "461 " + command.name + " :not enough parameters";
            this->send_user(uc->get_fd(), message);
            return;
        }
        if (command.params[0][0] == '#')
        {
            this->privMSG(uc, command, command.params[0]);
            return;
        }
        this->privMSG(uc, command);
    }
    if (command.name == "KICK"){
        this->kick(uc, command);
    }
    if (command.name == "NOTICE"){
        this->noticeMSG(uc, command);
    }
    if (command.name == "PING"){
        this->send_user(uc->get_fd(), "SERVER :" + uc->getFullname() + " PONG :" + uc->getNickname());
    }
    if (command.name == "LIST")
    {
        std::map<std::string , Channel>::iterator it = channelList.begin();
        if(it == channelList.end())
        {
            this->send_user(uc->get_fd(), "323 : No channel in server");
            return ;
        }
        for (it = channelList.begin(); it != channelList.end(); ++it)
        {
            std::string lst = it->second.getChannelName().substr(1) + " " + std::to_string(it->second.getUserCount()) + " :" + it->second.getTopic(); 
            this->send_user(uc->get_fd(), "322  server "+ lst);
            std::cout<< "322 server " + lst<<std::endl;
        }
        std::string channels;
        for (it = channelList.begin(); it != channelList.end(); ++it)
        {
            channels = channels + it->second.getChannelName() + " "; 
        }
        this->send_user(uc->get_fd(), "Server: "+ channels);
        this->send_user(uc->get_fd(), "323 ");
    }
    if (command.name == "NAMES")
    {
        if (command.params.size() < 1)
        {
            std::string message = "461 : not enough parameters";
            this->send_user(uc->get_fd(), message);
            return;
        }        
        if (command.params[0][0] != '#' && command.params[0]!="")
        {
            this->send_user(uc->get_fd(), "Server : Channel name first characther must be '#'");
            return ;
        }
        if (check_channel(channelList, command.params[0]))
        {
            std::string message = "403 " + command.params[0] + " :No such channel";
            this->send_user(uc->get_fd(), message);
        }
        else
        {
            std::vector<UserConn *> users = channelList.find(command.params[0])->second.getUserVector();
            std::vector<UserConn *>::iterator it = users.begin();
            if(it == users.end())
                this->send_user(uc->get_fd(), "Server : No users in channel");
            for (it = users.begin(); it != users.end(); ++it)
            {
                this->send_user(uc->get_fd(), "Server : " + (*it)->getNickname());
            }
        }
    }
}


void IrcServer::process_new_connection()
{
    sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int client_fd = accept(server_fd, reinterpret_cast<sockaddr *>(&client_addr), &client_len);
    fcntl(client_fd, F_SETFL, O_NONBLOCK);
    if (client_fd < 0)
    {
        std::cerr << "ERROR: Unable to accept new connection." << std::endl;
        return;
    }
    char address_buffer[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(client_addr.sin_addr), address_buffer, INET_ADDRSTRLEN);
    int port = ntohs(client_addr.sin_port);
    add_user_conn(client_fd, address_buffer, port);
}

void IrcServer::process_input(UserConn *uc)
{
    char buff[512];
    int fd = uc->get_fd();
    int n = read(fd, buff, 511);
    if (n < 0)
    {
        remove_user_conn(fd);
        return;
    }
    if (n == 0)
    {
        remove_user_conn(fd);
        return;
    }
    buff[n] = '\0';
    // std::cout << "input: " << buff <<std::endl;
    uc->buff += buff;
    std::string ret;
    while (uc->buff.find('\n') != std::string::npos)
    {
        ret = uc->buff.substr(0, uc->buff.find('\n') + 1);
        uc->buff.erase(0, uc->buff.find('\n') + 1);
        ret.pop_back();
        if (ret.back() == '\r')
            ret.pop_back();
        t_command command = parseCommand(ret);
        std::cout << "command: " << ret << std::endl;
        std::cout << "name: " << command.name << std::endl;
        std::cout << "params:" << std::endl;
        for (std::vector<std::string>::iterator it = command.params.begin(); it != command.params.end(); ++it)
            std::cout << "<" << *it << ">" << std::endl;
        std::cout << "args: " << command.args << std::endl;

        user_process(uc, command);
        login_process(uc, command);
    }
}

void IrcServer::add_user_conn(int fd, std::string address, int port) {
    UserConn *uc = new UserConn(fd, address, port);
    user_conns.push_back(uc);
    std::cout << "New connection from " << address << ":" << port << std::endl;
}

void IrcServer::remove_user_conn(int fd) {
    for (std::map<std::string, Channel>::iterator chn = this->channelList.begin(); chn != this->channelList.end(); chn++)
    {
           std::vector<UserConn *>&usr =  (*chn).second.getUserVector();
           for (std::vector<UserConn*>::iterator it = usr.begin(); it < usr.end(); ++it)
           {
                if ((*it)->get_fd() == fd)
                {
                    usr.erase(it);
                    break;
                }
           }
    }

    std::vector<UserConn *>::iterator it = user_conns.begin();
    while (it != user_conns.end()) {
        UserConn *uc = *it;
        if (uc->get_fd() == fd) {
            user_conns.erase(it);
            delete uc;
            std::cout << "Connection closed." << std::endl;
            break;
        }
        ++it;
    }
}

void IrcServer::send_all(std::string message) {
    for (std::vector<UserConn *>::iterator it = user_conns.begin(); it != user_conns.end(); ++it) {
        int fd = (*it)->get_fd();
        if (fd >= 0) {
            send_user(fd, message);
        }
    }
}

void IrcServer::privMSG(UserConn *uc, t_command comm, std::string channel_name)
{
    bool recived_channel = 0;
    bool sender_in_channel = 0;
    int i = comm.params.size() - 1;
    for (std::map<std::string , Channel>::iterator it = channelList.begin(); it != channelList.end(); ++it)
    {
        if (it->first == channel_name)
            recived_channel = 1;
    }

    if (recived_channel)
    {
        std::map<std::string , Channel>::iterator channelit = channelList.find(channel_name);
        std::vector<UserConn *> users =  (*channelit).second.getUserVector();
        for (std::vector<UserConn *>::iterator it = users.begin(); it != users.end(); ++it){
            if (uc->get_fd() == (*it)->get_fd()){
                sender_in_channel = 1;
                break;
            }
        }
        if (!sender_in_channel)
        {
            std::string message = "442 " + comm.params[0] + ": You're not on that channel ";
            this->send_user(uc->get_fd(), message);
            return ;
        }
        
        while(comm.params[i] != "" && i > 0)
            comm.args.insert(0,comm.params[i--] + " ");

        std::string channel_message = ":" + comm.params[0] + " | " + uc->getNickname() + " :";
        channel_message += comm.args;
        for (std::vector<UserConn *>::iterator it = users.begin(); it < users.end(); ++it){
            this->send_user((*it)->get_fd(), channel_message);
        }
    }
    else{
        this->send_user(uc->get_fd(), "ERROR : Channel Not Found");
    }
}

void IrcServer::privMSG(UserConn *uc, t_command comm)
{
    int reciver_fd = -1;
    int i = comm.params.size() - 1;
    std::string message = uc->getNickname() + " says: ";
    for (std::vector<UserConn *>::iterator it = this->user_conns.begin(); it != this->user_conns.end(); ++it){
        if ((*it)->getNickname() == comm.params[0])
        {
            reciver_fd = (*it)->get_fd();
            break;
        }
    }
    if (reciver_fd == -1)
    {
        std::string message = "401 " + comm.params[0] + " :No such nick/channel";
        this->send_user(uc->get_fd(), message);
    }
    while(comm.params[i] != "" && i > 0)
        comm.args.insert(0,comm.params[i--] + " ");
    std::cout<<comm.args;
    if (comm.args != "")
        message = message + " " + comm.args;
    this->send_user(reciver_fd, message);
}

void IrcServer::kick(UserConn *uc, t_command comm){
    if (comm.params.size() != 2)
    {
        this->send_user(uc->get_fd(), "461 : KICK usage 'KICK <channel> <user>'");
        return;
    }
    std::map<std::string , Channel>::iterator channelit = this->channelList.find(comm.params[0]);
    if (channelit == channelList.end())
    {
        std::string message = "403 " + comm.params[0] + " :No such channel";
        this->send_user(uc->get_fd(), message);
        return;
    }
    if (uc->get_fd() != (*channelit).second.getUserVector()[0]->get_fd())
    {
        std::string message = "401 " + comm.params[1] + " :No such nick/channel";
        this->send_user(uc->get_fd(), message);
        return ;
    }
    int user_number = 0;
    std::vector<UserConn *> &channelUserVector = (*channelit).second.getUserVector();
    for (std::vector<UserConn *>::iterator it = channelUserVector.begin(); it != channelUserVector.end(); ++it)
    {
        if ((*it)->getNickname() == comm.params[1])
        {
            send_user((*it)->get_fd(), "kicked " + comm.params[0] + " by operator");
            channelUserVector.erase(std::next(channelUserVector.begin() , user_number));
            break;
        }
        user_number++;
    }
    for (std::vector<UserConn *>::iterator it = channelUserVector.begin(); it != channelUserVector.end(); ++it)
    {
        this->send_user((*it)->get_fd(), comm.params[1] + " kicked from channel by operator");
    }
}

void IrcServer::noticeMSG(UserConn *uc, t_command comm)
{
    if (comm.params.empty())
        return;
    int reciver_fd = -1;
    for (std::vector<UserConn *>::iterator it = this->user_conns.begin(); it != this->user_conns.end(); ++it){
        if ((*it)->getNickname() == comm.params[0]){
            reciver_fd = (*it)->get_fd();
            break;
        }
    }
    std::string message = "NOTICE " + uc->getFullname() + " :";
    std::vector<std::string>::iterator it = comm.params.begin();
    ++it;
    for (; it != comm.params.end(); ++it){
        message = message + " " + *it; 
    }
    if (comm.args != "")
        message = message + " " + comm.args;
    if (reciver_fd == -1)
    {
        std::string message = "401 " + comm.params[1] + " :No such nick/channel";
        this->send_user(uc->get_fd(), message);
        return;
    }
    this->send_user(reciver_fd, message);
}


void IrcServer::send_user(int fd, std::string message)
{
    std::string output = message + "\r\n";
    int n = write(fd, output.c_str(), output.size());
    if (n < 0)
    {
        remove_user_conn(fd);
        return;
    }
}


void IrcServer::addNewChannel(UserConn *uc, t_command comm){
    Channel newChannel(comm, *uc);
    if (comm.params[1] != "")
        newChannel.setChannelPass(comm.params[1]);
    this->channelList.insert(std::pair<std::string, Channel>(comm.params[0] , newChannel));
    
}