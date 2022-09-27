#include <sys/socket.h>
#include <netdb.h>
#include <sys/ioctl.h>
#include <string.h>
#include <Create_socket.hpp>
#include "Server.hpp"

int	 create_socket(cfg::Server &s) {
    int opt = 1;
	int	res;
    struct sockaddr_in6 addr;

	res = socket(AF_INET6, SOCK_STREAM, getprotobyname("tcp")->p_proto);
	if (res == -1) {
		perror("socket() failed");
		return (-1);
	} else {
		s.setSocket(res);
	}

	res = setsockopt(s.getSocket(), SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt));
	if (res == -1) {
		perror("setsockopt() failed");
		return (res);
	}

	res = ioctl(s.getSocket(), FIONBIO, (char *)&opt);
	if (res == -1) {
		perror("ioctl() failed");
		return (res);
	}

	memset(&addr, 0, sizeof(addr));

	addr.sin6_family = AF_INET6;
	memcpy(&addr.sin6_addr, &in6addr_any, sizeof(in6addr_any));
	addr.sin6_port = htons(s.getListen());

	res = bind(s.getSocket(), (struct sockaddr *)&addr, sizeof(addr));
	if (res == -1) {
		perror("bind() failed");
		return (res);
	}

	res = listen(s.getSocket(), 42);
	if (res == -1) {
		perror("listen() failed");
		return (res);
	}

	return (1);
}
