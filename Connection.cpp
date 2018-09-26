#include "Connection.h"
#include <string>
#include <iostream>
#include <thread>
#include "Signer_and_verifier.h"
#include "verbose_verification.h"
#include "verbose_verification.cpp"
#include "Message_handler.h"
#include "Authentication_Message.h"

#include "Auth_Message_handler.h"

using asio::ip::tcp;
namespace ssl = asio::ssl;
typedef ssl::stream<tcp::socket> ssl_socket;

Connection::Connection()=default;

Connection::~Connection()=default;
void Connection::authenticate(ssl_socket& socket,std::string fqdn_)
{
	auto auth_message_handler=std::make_shared<Auth_Message_handler>(connection_io_service,socket,fqdn_);
	auth_message_handler->start_authenticate();
	if(auth_message_handler->get_authenticated())
	{
		this->is_authenticated=true;
	}
	this->message_seq=auth_message_handler->message_seq;
}
void Connection::set_server(std::string server)
{
	this->server=std::move(server);
}

void Connection::set_port(std::string server)
{
	this->port=std::move(port);
}
void Connection::start(std::string server, std::string port, int thred_pool_size, std::string fqdn_)
{
	std::string fqdn=std::move(fqdn_);
	
	set_server(server);
	set_port(port);
	ssl::context ctx(ssl::context::tlsv1);
	//ctx.set_default_verify_paths();
	//ctx.load_verify_file("../certs/server.crt");
	ctx.add_verify_path("../certs");
	tcp::resolver resolver(connection_io_service);
	tcp::resolver::query query(server,port);
	tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
	ssl_socket socket(connection_io_service,ctx);
	asio::connect(socket.lowest_layer(), endpoint_iterator);
	socket.lowest_layer().set_option(tcp::no_delay(true));
	socket.set_verify_mode(ssl::verify_peer);
	//socket.set_verify_callback(ssl::rfc2818_verification(server));
	socket.set_verify_callback(make_verbose_verification(ssl::rfc2818_verification(server)));
	
	socket.handshake(ssl_socket::client);
	
	authenticate(socket,fqdn);
		
	if(is_authenticated)
	{
		
		connection_io_service.reset();
		message_seq++;
		std::cout<<"I am authenticated :)"<<std::endl;
		std::cout<<message_seq<<std::endl;
		
		auto message_handler=std::make_shared<Message_handler>
		(connection_io_service,socket,fqdn);
		message_handler->set_pool_size(thred_pool_size);
		message_handler->message_seq=this->message_seq;
		message_handler->start_with_asking_job(thred_pool_size);
		
		message_handler->stop_pool();
		
		
	}
	
	
	
	//auto message_handler=std::make_shared<Message_handler>(connection_io_service,socket,fqdn);
	//message_handler->start();

	
	
	

	
}
