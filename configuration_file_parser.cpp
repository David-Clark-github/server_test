#include <Server.hpp>
#include <iostream>
#include <string>
#include <fstream>
#include <algorithm>
#include <cstring>
#include <list>

#define	NPOS std::string::npos

typedef struct					s_cfg
{
	std::string					key;
	std::string					value;
	s_cfg						*children;
}								t_cfg;

bool						findServers(std::string str, std::string key, std::vector<t_cfg> &servers)
{
	size_t							i = 0;
	size_t							tmp;
	size_t							it = 0;

	for (; i < str.length(); i++)
	{
		tmp = i;
		for (; isspace(str[i]) && i < str.length(); i++);
		for (size_t j = 0; i < str.length() && j < key.length(); i++, j++)
			if (str[i] != key[j])
				return false;
		servers.push_back(t_cfg());
		std::cout << servers.size() << std::endl;
		// servers[i].key = key;
		for (; isspace(str[i]) && i < str.length(); i++);
		if (str[i] != '{')
			return false;
		if (i < str.length())
			i++;
		size_t size = 0;
		for (size_t flag = 1; i < str.length() && flag != 0; i++, size++)
		{
			if (str[i] == '{')
				flag++;
			else if (str[i] == '}')
				flag--;
		}
		servers[it].value = str.substr(tmp, size);
		it++;
	}
	return true;
}

std::list<std::string>		*split(std::string str, std::string delimiter)
{
	size_t						begin = 0;
	size_t						end = 0;
	std::list<std::string>		*sp = new std::list<std::string>;
	std::string					buff;

	while (end != NPOS)
	{
		end = str.find(delimiter, begin);
		buff = str.substr(begin, end - begin);
		if (buff.length() > 0)
			sp->push_back(buff);
		begin = end + delimiter.length();
	}
	return sp;
}

std::string		*getFileContent(char *file_name)
{
	std::string					*file_content = new std::string;
	std::string					buffer;
	std::ifstream				file;

	try
	{
		file.open(file_name, std::ios_base::in);
		file.exceptions(std::ifstream::failbit);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return NULL;
	}
	while (!file.eof())
	{
		std::getline(file, buffer);
		buffer.append("\n");
		file_content->append(buffer);
	}
	return file_content;
}

int		main(int ac, char **av)
{
	(void)av;
	if (ac != 2)
		return 1;
	std::string *s = getFileContent(av[1]);
	std::vector<t_cfg> sv;
	findServers(*s, "server", sv);
	for (std::vector<t_cfg>::iterator it = sv.begin(); it != sv.end(); it++)
		std::cout << it->key << std::endl;
}