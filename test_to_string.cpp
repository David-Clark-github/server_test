#include <cstdlib>
#include <string>

int main(int ac, char **av) {
	std::string str;

	str.append(std::to_string(atoi(av[1])));
}
