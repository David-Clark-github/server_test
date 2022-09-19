#include <Server.hpp>
#include <iostream>
#include <string>
#include <fstream>
#include <algorithm>
#include <cstring>
#include <list>
#include <stdio.h>

#define	NPOS std::string::npos

bool						findKeyWithBrace(std::string str, std::string key, std::list<std::string> &servers)
{
	size_t							i = 0;
	size_t							tmp;

	for (; i < str.length(); i++)
	{
		for (; !isalpha(str[i]) && i < str.length();)
		{
			for (; isspace(str[i]) && i < str.length(); i++);
			if (str[i] == '#')
				for (; str[i] != '\n' && i < str.length(); i++);
		}
		for (size_t j = 0; i < str.length() && j < key.length(); i++, j++)
			if (str[i] != key[j])
				return false;
		for (; isspace(str[i]) && i < str.length(); i++);
		if (str[i] != '{')
			return false;
		if (i < str.length())
			i++;
		size_t size = 0;
		tmp = i;
		for (size_t flag = 1; i < str.length() && flag != 0; i++)
		{
			if (str[i] == '{')
				flag++;
			else if (str[i] == '}')
				flag--;
			if (flag != 0)
				size++;
		}
		servers.push_back(str.substr(tmp, size));
	}
	return (servers.size()) ? true : false;
}

// bool						findKeyWithSemicolon(std::string str, std::string key, std::map<std::string, std::list<std::string> > &attributes)
// {

// }

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
	std::list<std::string>	sv;
	findKeyWithBrace(*s, "server", sv);
}