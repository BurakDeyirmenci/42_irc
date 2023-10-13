#include "IrcServer.hpp"

int main(int argc, char **argv)
{
	int port;
	std::string pass;

	try
	{
		if (argc == 3)
			pass = argv[2];
		else if(argc == 2)
			pass = "";
		else
			throw std::runtime_error("usage: ircserv [port] [password]");
		port = atoi(argv[1]);
		if (!(port > 1024 && port < 49151))  
			throw std::runtime_error("port number should be between 1024 and 49151");
		IrcServer server(port, pass);
		server.start();
	}
	catch(const std::runtime_error& e){
		std::cerr << e.what() << '\n';}	
	return 0;
}