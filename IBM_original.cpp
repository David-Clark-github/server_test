#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <errno.h>
#include <netdb.h>
#include <string>
#include <iostream>
#include <fstream>
#include <cstdlib>

#define SERVER_PORT1	12345
#define SERVER_PORT2	8080
#define	GET_M			1
#define	POST_M			2
#define	DELETE_M		3

#define TRUE             1
#define FALSE            0

typedef struct	s_request_ser {
	int			method;
	int 		port;
	std::string	route;
}				t_request_ser;

static int		malloc_len(long nb)
{
	int	res;

	res = 1;
	if (nb == 0)
		return (2);
	if (nb < 0)
	{
		res++;
		nb *= -1;
	}
	while (nb)
	{
		nb /= 10;
		res++;
	}
	return (res);
}

void alloc_dest(char *dest, long nb, int len)
{
	if (nb == 0)
		dest[0] = '0';
	if (nb < 0)
	{
		nb *= -1;
		dest[0] = '-';
	}
	while (nb)
	{
		dest[len] = (nb % 10) + '0';
		nb /= 10;
		len--;
	}
}

void	ft_itoa_string(int n, std::string &s)
{
	long	nb;
	int		m_len;
	char	dest[16];

	memset(dest, 0, sizeof(dest));
	nb = (long)n;
	m_len = malloc_len(nb);
	dest[m_len - 1] = '\0';
	alloc_dest(dest, nb, m_len - 2);
	s.append(dest);
}

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
		return (0);
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
	r.method = taking_method(raw);		// Method
	if (!taking_route(raw, r.route)) {	// Route
		perror("taking_route");
	}
	r.port = taking_port(raw);			// Port

	printf("Port = %d\n", r.port);
	std::cout << "route = [" << r.route << "]\n";
	if (r.method == GET_M)
		printf("Method = GET\n");
	else if (r.method == POST_M)
		printf("Method = POST\n");
	else if (r.method == DELETE_M)
		printf("Method = DELETE\n");
	else
		printf("method unknow = %d\n", r.method);
}

typedef struct	s_server {
	int						port;
	struct	sockaddr_in6	addr;
	int						init_sock;
	int						err;
	int						opt;
}				t_server;

int	create_server(t_server *s, int port) {
	s->opt = 1;
	s->port = port;

	/* socket */
	s->init_sock = socket(AF_INET6, SOCK_STREAM, getprotobyname("tcp")->p_proto);
	if (s->init_sock == -1) {
		perror("socket() failed");
		return (-1);
	}

	/* setsockopt */
	s->err = setsockopt(s->init_sock, SOL_SOCKET, SO_REUSEADDR, (char *)&s->opt, sizeof(s->opt));
	if (s->err == -1) {
		perror("setsockopt() failed");
		close(s->init_sock);
		return (-1);
	}

	/* ioctl */
	s->err = ioctl(s->init_sock, FIONBIO, (char *)&s->opt);
	if (s->err == -1) {
		perror("ioctl() failed");
		close(s->init_sock);
		return (-1);
	}

	/* memset addr */
	memset(&s->addr, 0, sizeof(s->addr));

	s->addr.sin6_family = AF_INET6;
	memcpy(&s->addr.sin6_addr, &in6addr_any, sizeof(in6addr_any));
	s->addr.sin6_port = htons(port);

	/* bind */
	s->err = bind(s->init_sock, (struct sockaddr *)&s->addr, sizeof(s->addr));
	if (s->err == -1) {
		perror("bind() failed");
		close(s->init_sock);
		return (-1);
	}

	/* listen */
	s->err = listen(s->init_sock, 42);
	if (s->err == -1) {
		perror("listen() failed");
		close(s->init_sock);
		return (-1);
	}

	return (1);

}

int	check_init_sock(int comp, t_server *list, int server_len) {
	for (int i = 0; i < server_len; i++) {
		if (comp == list[i].init_sock)
			return (list[i].init_sock);
	}
	return (-1);
}



int main (int argc, char *argv[])
{
	std::string page_upload;

	page_upload.append("HTTP/1.1 200 OK\n");
	page_upload.append("Content-type: text/html\n");
	page_upload.append("Content-Length: ");

	std::fstream f;

	f.open("home/default.html", std::ios::in);

	std::string s;
	std::string data;

	while (1) {
		//f >> c;
		getline(f, s);
		if (f.eof())
			break;
		data.append(s);
	}
	ft_itoa_string(data.size(), page_upload);
	page_upload.append("\n\n");
	page_upload.append(data);
	f.close();

	int		server_len = 3;
	int    content_len, rc, on = 1;
	int    listen_sd1 = -1, new_sd = -1, listen_sd2 = -1;
	int    desc_ready, end_server = FALSE, compress_array = FALSE;
	int    close_conn;
	char   buffer[65535];
	struct sockaddr_in6   addr;
	int    timeout;
	struct pollfd fds[200];

	int    nfds = server_len, current_size = 0, i, j;

	t_server	list_s[server_len];
	memset(list_s, 0, sizeof(list_s));

	create_server(&list_s[0], 8080);
	create_server(&list_s[1], 12345);
	create_server(&list_s[2], 5050);

	/*************************************************************/
	/* Initialize the pollfd structure                           */
	/*************************************************************/
	memset(fds, 0 , sizeof(fds));

	/*************************************************************/
	/* Set up the initial listening socket                        */
	/*************************************************************/
	fds[0].fd = list_s[0].init_sock;
	fds[0].events = POLLIN;
	fds[1].fd = list_s[1].init_sock;
	fds[1].events = POLLIN;
	fds[2].fd = list_s[2].init_sock;
	fds[2].events = POLLIN;

	/*************************************************************/
	/* Initialize the timeout to 3 minutes. If no                */
	/* activity after 3 minutes this program will end.           */
	/* timeout value is based on milliseconds.                   */
	/*************************************************************/
	timeout = (3 * 60 * 1000);

	/*************************************************************/
	/* Loop waiting for incoming connects or for incoming data   */
	/* on any of the connected sockets.                          */
	/*************************************************************/
	do
	{
		/***********************************************************/
		/* Call poll() and wait 3 minutes for it to complete.      */
		/***********************************************************/
		printf("\nWaiting on poll()...\n");
		rc = poll(fds, nfds, -1);

		/***********************************************************/
		/* Check to see if the poll call failed.                   */
		/***********************************************************/
		if (rc < 0)
		{
			perror("  poll() failed");
			break;
		}

		/***********************************************************/
		/* Check to see if the 3 minute time out expired.          */
		/***********************************************************/
		if (rc == 0)
		{
			printf("  poll() timed out.  End program.\n");
			break;
		}


		/***********************************************************/
		/* One or more descriptors are readable.  Need to          */
		/* determine which ones they are.                          */
		/***********************************************************/
		current_size = nfds;
		for (i = 0; i < current_size; i++)
		{
			/*********************************************************/
			/* Loop through to find the descriptors that returned    */
			/* POLLIN and determine whether it's the listening       */
			/* or the active connection.                             */
			/*********************************************************/
			if(fds[i].revents == 0)
				continue;

			/*********************************************************/
			/* If revents is not POLLIN, it's an unexpected result,  */
			/* log and end the server.                               */
			/*********************************************************/
			if(fds[i].revents != POLLIN)
			{
				printf("  Error! revents = %d\n", fds[i].revents);
				end_server = TRUE;
				break;

			}
			if (check_init_sock(fds[i].fd, list_s, server_len) != -1)
			{
				int	ok_fd = check_init_sock(fds[i].fd, list_s, server_len);
				/*******************************************************/
				/* Listening descriptor is readable.                   */
				/*******************************************************/
				printf("  Listening socket is readable\n");

				/*******************************************************/
				/* Accept all incoming connections that are            */
				/* queued up on the listening socket before we         */
				/* loop back and call poll again.                      */
				/*******************************************************/
				do
				{
					/*****************************************************/
					/* Accept each incoming connection. If               */
					/* accept fails with EWOULDBLOCK, then we            */
					/* have accepted all of them. Any other              */
					/* failure on accept will cause us to end the        */
					/* server.                                           */
					/*****************************************************/
					new_sd = accept(ok_fd, NULL, NULL);
					if (new_sd < 0)
					{
						if (errno != EWOULDBLOCK)
						{
							perror("  accept() failed");
							end_server = TRUE;
						}
						break;
					}
					ioctl(new_sd, FIONBIO, (char *)&on);

					/*****************************************************/
					/* Add the new incoming connection to the            */
					/* pollfd structure                                  */
					/*****************************************************/
					printf("  New incoming connection - %d\n", new_sd);
					fds[nfds].fd = new_sd;
					fds[nfds].events = POLLIN;
					nfds++;

					/*****************************************************/
					/* Loop back up and accept another incoming          */
					/* connection                                        */
					/*****************************************************/
				} while (new_sd != -1);
			}
			else
			{
				//printf("  Descriptor %d is readable\n", fds[i].fd);
				close_conn = FALSE;
				/*******************************************************/
				/* Receive all incoming data on this socket            */
				/* before we loop back and call poll again.            */
				/*******************************************************/

				do
				{
					/*****************************************************/
					/* Receive data on this connection until the         */
					/* recv fails with EWOULDBLOCK. If any other         */
					/* failure occurs, we will close the                 */
					/* connection.                                       */
					/*****************************************************/
					rc = recv(fds[i].fd, buffer, sizeof(buffer), 0);
					if (rc < 0)
					{
						if (errno != EWOULDBLOCK)
						{
							perror("  recv() failed");
							close_conn = TRUE;
						}
						break;
					}

					/* Affichage des requêtes clients */
					printf("\n*******************\n");
					printf("| Client request: |\n");
					//printf("*******************\n");
					printf("*******************\n%s", buffer);

					t_request_ser	r_s;	
					std::string client;
					client.append(buffer);
					pars_request(r_s, client);

					

					/*****************************************************/
					/* Check to see if the connection has been           */
					/* closed by the client                              */
					/*****************************************************/
					if (rc == 0)
					{
						printf("  Connection closed\n");
						close_conn = TRUE;
						break;
					}

					/*****************************************************/
					/* Data was received                                 */
					/*****************************************************/
					content_len = rc;

					//printf("  %d bytes received\n", content_len);

					/*****************************************************/
					/* Echo the data back to the client                  */
					/*****************************************************/
					printf("*******************\n");
					printf("| Server respond: |\n");
					printf("*******************\n");
					//std::cout << page_upload << std::endl;
					if (r_s.route == "/")
						rc = send(fds[i].fd, page_upload.c_str(), page_upload.size(), 0);
					if (r_s.route == "/favicon.ico")
						rc = send(fds[i].fd, NULL, 0, 0);
					//rc = send(fds[i].fd, buffer, content_len, 0);
					if (rc < 0)
					{
						perror("  send() failed");
						close_conn = TRUE;
						break;
					}
					//str = "\n";
					//send(fds[i].fd, str, strcontent_len(str), 0);

				} while(TRUE);

				/*******************************************************/
				/* If the close_conn flag was turned on, we need       */
				/* to clean up this active connection. This            */
				/* clean up process includes removing the              */
				/* descriptor.                                         */
				/*******************************************************/
				if (close_conn)
				{
					printf("close_conn\n");
					close(fds[i].fd);
					fds[i].fd = -1;
					compress_array = TRUE;
				}


			}  /* End of existing connection is readable             */
		} /* End of loop through pollable descriptors              */

		/***********************************************************/
		/* If the compress_array flag was turned on, we need       */
		/* to squeeze together the array and decrement the number  */
		/* of file descriptors. We do not need to move back the    */
		/* events and revents fields because the events will always*/
		/* be POLLIN in this case, and revents is output.          */
		/***********************************************************/
		if (compress_array)
		{
			compress_array = FALSE;
			printf("compress_array\n");
			for (i = 0; i < nfds; i++)
			{
				if (fds[i].fd == -1)
				{
					for(j = i; j < nfds; j++)
					{
						fds[j].fd = fds[j+1].fd;
					}
					i--;
					nfds--;
				}
			}
		}

	} while (end_server == FALSE); /* End of serving running.    */

	/*************************************************************/
	/* Clean up all of the sockets that are open                 */
	/*************************************************************/
	for (i = 0; i < nfds; i++)
	{
		if(fds[i].fd >= 0)
			close(fds[i].fd);
	}
}
