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
#include "Server.hpp"
#include <request_client.hpp>
#include "Create_socket.hpp"
#include "ft_itoa_string.hpp"
#include <vector>

#define SERVER_PORT1	12345
#define SERVER_PORT2	8080

#define TRUE             1
#define FALSE            0

int	check_init_sock(int comp, std::vector<cfg::Server> s, int server_len) {
	for (int i = 0; i < server_len; i++) {
		if (comp == s[i].getSocket())
			return (s[i].getSocket());
	}
	return (-1);
}

void	usage(char *str) {
	printf("Usage:\n%s [fichier de configuration]\n", str);
	exit(EXIT_FAILURE);
}

int main (int argc, char *argv[])
{
	if (argc != 2) {
		usage(argv[0]);
	}
	std::string page_upload;

	std::vector<cfg::Server>	server_list;
	cfg::Server					s1;
	cfg::Server					s2;
	cfg::Server					s3;

	s1.setListen(8080);
	s2.setListen(5050);
	s3.setListen(12345);

	server_list.push_back(s1);
	server_list.push_back(s2);
	server_list.push_back(s3);

	struct pollfd fds[server_list.size() * 10];
	memset(fds, 0, sizeof(fds));

	for (size_t i = 0; i < server_list.size(); i++) {
		create_socket(server_list[i]);
		fds[i].fd = server_list[i].getSocket();
		fds[i].events = POLLIN;
	}


	page_upload.append("HTTP/1.1 200 OK\n");
	page_upload.append("Content-type: text/html\n");
	page_upload.append("Content-Length: ");

	std::fstream f;

	f.open("home/form.html", std::ios::in);

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

	int		server_len = server_list.size();
	int    content_len, rc, on = 1;
	int    new_sd = -1;
	int    end_server = FALSE, compress_array = FALSE;
	int    close_conn;
	char   buffer[65535];
	int    timeout;

	int    nfds = server_len, current_size = 0, i, j;

	timeout = -1;

	do
	{
		printf("\nWaiting on poll()...\n");
		rc = poll(fds, nfds, -1);

		if (rc < 0)
		{
			perror("  poll() failed");
			break;
		}

		if (rc == 0)
		{
			printf("  poll() timed out.  End program.\n");
			break;
		}


		current_size = nfds;
		for (i = 0; i < current_size; i++)
		{
			if(fds[i].revents == 0)
				continue;

			if(fds[i].revents != POLLIN)
			{
				printf("  Error! revents = %d\n", fds[i].revents);
				end_server = TRUE;
				break;

			}
			if (check_init_sock(fds[i].fd, server_list, server_len) != -1)
			{
				int	ok_fd = check_init_sock(fds[i].fd, server_list, server_len);

				printf("  Listening socket is readable\n");

				do
				{
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

					printf("  New incoming connection - %d\n", new_sd);
					fds[nfds].fd = new_sd;
					fds[nfds].events = POLLIN;
					nfds++;

				} while (new_sd != -1);
			}
			else
			{
				//printf("  Descriptor %d is readable\n", fds[i].fd);
				close_conn = FALSE;

				do
				{
					rc = recv(fds[i].fd, buffer, sizeof(buffer), 0);
					if (rc < 0)
					{
					//	if (errno != EWOULDBLOCK)
					//	{
					//		perror("  recv() failed");
					//		close_conn = TRUE;
					//	}
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

					if (rc == 0)
					{
						printf("  Connection closed\n");
						close_conn = TRUE;
						break;
					}
					content_len = rc;

					printf("*******************\n");
					printf("| Server respond: |\n");
					printf("*******************\n");
					//std::cout << page_upload << std::endl;
					if (r_s.route == "/favicon.ico")
						rc = send(fds[i].fd, NULL, 0, 0);
					else {
						rc = send(fds[i].fd, page_upload.c_str(), page_upload.size(), 0);
					}
					//
					//      CCCCC    GGGGG    IIIII
					//      C        G          I
					//      C        G  GG      I
					//      C        G   G      I
					//      C        G   G      I
					//      CCCCC    GGGGG    IIIII
					//

					// SEND(FDS[I].FD, STRING_CGI.C_STR(), STRING_CGI.SIZE(), 0);

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

				if (close_conn)
				{
					printf("close_conn\n");
					close(fds[i].fd);
					fds[i].fd = -1;
					compress_array = TRUE;
				}


			}  /* End of existing connection is readable             */
		} /* End of loop through pollable descriptors                */

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

	for (i = 0; i < nfds; i++)
	{
		if(fds[i].fd >= 0)
			close(fds[i].fd);
	}
}
