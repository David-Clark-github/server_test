#include <sys/socket.h>
#include <netdb.h>
#include <sys/ioctl.h>
#include <string.h>
#include "Server.hpp"

void create_socket(cfg::Server &s) {
    int opt = 1;
    struct sockaddr_in6 addr;

	s.setSocket(socket(AF_INET6, SOCK_STREAM, getprotobyname("tcp")->p_proto));

	setsockopt(s.getSocket(), SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt));

	ioctl(s.getSocket(), FIONBIO, (char *)&opt);

	memset(&addr, 0, sizeof(addr));

	addr.sin6_family = AF_INET6;
	memcpy(&addr.sin6_addr, &in6addr_any, sizeof(in6addr_any));
	addr.sin6_port = htons(s.getListen());

	bind(s.getSocket(), (struct sockaddr *)&addr, sizeof(addr));
	listen(s.getSocket(), 42);
}