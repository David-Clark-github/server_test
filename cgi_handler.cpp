/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi_handler.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asaboure <asaboure@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/05 19:42:19 by asaboure          #+#    #+#             */
/*   Updated: 2022/09/06 19:21:02 by asaboure         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <string>
#include <sys/wait.h>
#include <iostream>
#include <string.h>
#define BUFFERSIZE 32

// https://forhjy.medium.com/42-webserv-cgi-programming-66d63c3b22db
std::string cgiHandler(char **args, char **env)
{
    int         fd[2];
    pid_t       cgiPID;
    std::string retBody;
    char        buf[BUFFERSIZE];

    pipe(fd);
    if ((cgiPID = fork()) == 0)
    {
        close(STDOUT_FILENO);
        close(fd[0]);
        dup(fd[1]);
        execve(args[0], args, env);
    }
    else
    {
        wait(NULL);
        close(STDIN_FILENO);
        close(fd[1]);
        dup(fd[0]);
        lseek(fd[0], 0, SEEK_SET);
        int retRead = 1;
        while (retRead > 0)
        {
            bzero(buf, BUFFERSIZE);
            retRead = read(fd[0], buf, BUFFERSIZE - 1);
            retBody += buf;
        }
    }
    if (cgiPID == 0)
        exit(0);
    return (retBody);
}

int main(){
    char *args[2];
    args[0] = (char *)"/usr/bin/php-cgi7.4";
    args[1] = (char *)"hello.php";

    std::string body = cgiHandler(args, NULL);
    std::cout << body << std::endl;
}