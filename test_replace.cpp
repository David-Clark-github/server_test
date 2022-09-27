#include <iostream>
#include <string>
int main(int ac, char **av) {
	if (ac != 4) {
		return (EXIT_FAILURE);
	}
	std::string str;
	std::string find_str;
	std::string	replace_by;

	str.append(av[1]);
	find_str.append(av[2]);
	replace_by.append(av[3]);

	std::cout << "str = [" << str << "]" << std::endl;
	str.replace(str.find(find_str), find_str.size(), replace_by);
	std::cout << "str = [" << str << "]" << std::endl;
	return (0);

}
