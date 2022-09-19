#include <request_client.hpp>

size_t	taking_method(std::string str) {
	size_t	res = -1;
	if (str.find("GET") == 0)
		res = GET_M;
	else if (str.find("POST") == 0)
		res = POST_M;
	else if (str.find("DELETE") == 0)
		res = DELETE_M;
	return (res);
}

size_t	taking_port(std::string str) {
	std::string tmp;
	size_t	find_host;
	size_t	start;
	size_t	end;

	find_host = str.find("Host: ");
	if (find_host == std::string::npos)
		return (-1);
	start = str.find(":", find_host + 5);
	tmp = str.substr(start);
	end = tmp.find("\n");
	tmp = tmp.substr(0, end);
	tmp.erase(0, 1);
	return (atoi(tmp.c_str()));
}

size_t	taking_route(std::string str, std::string &route) {
	size_t	start	= str.find("/");
	size_t	end		= str.find(" HTTP/1.1");

	if (start == std::string::npos || end == std::string::npos)
		return (0);
	
	route = str.substr(start, (end - start));
	return (1);
}

void	pars_request(t_request_ser &r, std::string &raw) {

	r.method = taking_method(raw);			// Method

	if (!taking_route(raw, r.route)) {		// Route
		perror("taking_route() failed");
	}

	printf("route = [%s]\n", r.route.c_str());

	r.port = taking_port(raw);				// Port
	if (r.port == -1)
		return;
	printf("port = %d\n", r.port);

	switch (r.method) {
		case GET_M:
			printf("Method = GET\n");
			break;
		case POST_M:
			printf("Method = POST\n");
			break;
		case DELETE_M:
			printf("Method = DELETE\n");
			break;
		default:
			printf("Method unknow\n");
	}
}
