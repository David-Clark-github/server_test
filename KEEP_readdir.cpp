#include <dirent.h>
#include <cstring>
#include <iostream>
#include <stdio.h>

void	usage(void) {
	std::cout << "Usage: ./program <folder>\n";
	exit(-1);
}

int main(int ac, char **av)
{
	if (ac != 2)
		usage();
	DIR				*dir;
	struct dirent	*dp;
	char			*name;

	dir = opendir(av[1]);
	while (dp = readdir(dir)) {
		name = dp->d_name;
		printf("%s\n", name);
	}
	return 0;
}

