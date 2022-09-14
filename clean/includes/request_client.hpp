#ifndef REQUEST_CLIENT_HPP
#define REQUEST_CLIENT_HPP

#include <stdio.h>
#include <string>
#include <stdlib.h>

#define GET_M		1
#define	POST_M		2
#define	DELETE_M	3

typedef struct	s_request_ser {
	int			method;
	int			port;
	std::string	route;
}				t_request_ser;

void	pars_request(t_request_ser &r, std::string &raw);
size_t	taking_method(std::string str);
size_t	taking_route(std::string str, std::string &route);
size_t	taking_port(std::string str);

#endif
