#ifndef AUTH_MESSAGE_HANDLER_H
#define AUTH_MESSAGE_HANDLER_H
#include "Message_handler.h"
#include <string>
class Auth_Message_handler : 
public Message_handler

{
public:
	Auth_Message_handler(asio::io_service& , 
	ssl_socket& ,std::string );
	~Auth_Message_handler();
	void start_authenticate();
	void send_message(Message);
	std::shared_ptr<Auth_Message_handler> shared_from_this();
	bool get_authenticated();
	unsigned int message_seq;
private:
	asio::io_service& message_io_service;
	ssl_socket& socket_;
	
	
	
	bool authenticated=false;
	void do_login_answer_header_read();
	void do_login_answer_body_read
(unsigned long long int,std::string);
	
	
};

#endif // AUTH_MESSAGE_HANDLER_H
