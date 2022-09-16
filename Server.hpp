#ifndef SERVER_HPP
# define SERVER_HPP

#include <string>
#include <map>
#include <vector>
#include <iterator>

namespace cfg
{
	class Server
	{
	public :

		typedef std::vector<std::map< std::string, std::vector< std::string> > >	t_location;

	private:

		std::string															_server_name;
		int																	_listen;
		t_location															_location;

	public:

		//	Constructor and destructor
		Server();
		~Server();

		//	Getters
		std::string												getServerName(void);
		int														getListen(void);
		t_location												getLocation(void);

		//	Setters
		void													setServerName(std::string server_name);
		void													setListen(int listen);
		void													setLocation(t_location location);
	};
}

# endif