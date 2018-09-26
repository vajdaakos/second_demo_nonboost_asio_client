#include "Auth_Message_handler.h"

Auth_Message_handler::Auth_Message_handler
(asio::io_service& message_io_service, 
ssl_socket& socket_,std::string fqdn_)
:Message_handler(message_io_service, 
socket_,fqdn_),message_io_service(message_io_service) ,
socket_(socket_)
{
	this->fqdn=fqdn_;
	set_start_seq();
} 


Auth_Message_handler::~Auth_Message_handler()
{
	free(body);
}

void Auth_Message_handler::start_authenticate()
{
	authenticated=false;
	Authentication_Message auth_message(this->fqdn,this->get_start_seq());

	send_message(auth_message);
	std::cout<<"Itt kezd futni a service."<<std::endl;
	message_io_service.run();
	std::cout<<"Itt már nem fut a service!"<<std::endl;
}

void Auth_Message_handler::send_message(Message message)
{
	unsigned char binary_message[512+message.get_body_lenght()];
	set_body_lenght_message_field(message, binary_message);
	set_sender_message_field(message, binary_message);
	set_seq_message_field(message, binary_message);
	set_response_to_message_field(message, binary_message);
	set_type_message_field(message, binary_message);
	set_full_path_message_field(message, binary_message);
	set_body_message_field(message, binary_message);
	asio::async_write(socket_,
	asio::buffer(binary_message,512+message.get_body_lenght()),
	[=,me=shared_from_this()](std::error_code const & ec, std::size_t bytes_xfer)
	{
		std::cout<<"Sent:"<<bytes_xfer<<std::endl;
		me->do_login_answer_header_read();
	});
	
	
}


void Auth_Message_handler::do_login_answer_header_read()
{

		asio::async_read(socket_,asio::buffer(header),
        [self=shared_from_this()](std::error_code ec, std::size_t bytes_transferred)
        {
          if (!ec) 
          {
			std::cout<<"Messige arrived, headerlength:"<<bytes_transferred<<std::endl;
			std::cout<<"reading header..."<<std::endl;
			unsigned long long int body_lenght=self->get_full_body_lenght(self->header);
			std::string sender=self->get_sender(self->header);
			unsigned int seq=self->get_seq(self->header);
			self->message_seq=seq;
			unsigned int response_to=self->get_response_to(self->header);
			unsigned int type=self->get_type(self->header);
			std::string full_path=self->get_full_path(self->header);
			std::cout<<"body lenght: "<<body_lenght<<std::endl;
			  if (body_lenght>0)
				self->do_login_answer_body_read(body_lenght,sender);

          }
          else
          {
			  std::cout<<ec<<std::endl;
			  
			  return;

          }
        });
}

void Auth_Message_handler::do_login_answer_body_read
(unsigned long long int full_body_lenght,std::string sender)
{
	
	body=(unsigned char*)malloc(full_body_lenght);
    asio::async_read(socket_,asio::buffer(body,full_body_lenght),
        [this, self=shared_from_this(),sender]
		(std::error_code ec, std::size_t bytes_transferred)
        {
          if (!ec) 
			{	
				std::cout<<"Messige arrived, bodylength:"<<bytes_transferred<<std::endl;
				authenticated=true;
				std::cout<<body<<std::endl;
				
				
			}
			
		else
			{
			
			}
		});
		
}
std::shared_ptr<Auth_Message_handler> Auth_Message_handler::shared_from_this()
    {
        return std::static_pointer_cast<Auth_Message_handler>
		(Message_handler::shared_from_this());
    }
	
bool Auth_Message_handler::get_authenticated()
{
	return this->authenticated;
}