#ifndef CONNECTION_H
#define CONNECTION_H
#include "asio.hpp"
#include "asio/ssl.hpp"
#include <string>

namespace ssl = asio::ssl;
using asio::ip::tcp;
typedef ssl::stream<tcp::socket> ssl_socket;
class Connection: public std::enable_shared_from_this<Connection>
{

public:
	Connection();
	~Connection();
	void start(std::string, std::string, int, std::string);
	asio::io_service connection_io_service;
	
	
private:
	unsigned int message_seq;
	void set_server(std::string);
	void set_port(std::string);
	std::string server;
	std::string port;
	
	void authenticate(ssl_socket&,std::string);
	bool is_authenticated;
	

	
};

#endif // CONNECTION_H