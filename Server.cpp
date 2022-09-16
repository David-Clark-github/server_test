#include <Server.hpp>

cfg::Server::Server() : _server_name(""), _listen(), _location()
{
	return ;
};

cfg::Server::~Server()
{
	return ;
};

std::string						cfg::Server::getServerName(void)
{
	return _server_name;
}

int								cfg::Server::getListen(void)
{
	return _listen;
}

cfg::Server::t_location			cfg::Server::getLocation(void)
{
	return _location;
}


void							cfg::Server::setServerName(std::string server_name)
{
	_server_name = server_name;
};

void							cfg::Server::setListen(int listen)
{
	_listen = listen;
};

void							cfg::Server::setLocation(t_location location)
{
	_location = location;
};