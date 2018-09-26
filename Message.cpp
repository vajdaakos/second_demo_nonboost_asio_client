#include "Message.h"
Message::Message()
{
}

Message::~Message()
{
	
}

void Message::set_sender(std::string sender_)
{
	
	this->sender=sender_;
}
void Message::set_seq(unsigned int seq)
{
	this->seq=seq;
}
void Message::set_response_to(unsigned int response_to_)
{
	this->response_to=response_to_;
}
void Message::set_type(unsigned int type){
	this->type=type;
	
}
void Message::set_full_path(std::string full_path_)
{
	this->full_path=full_path_;
}

unsigned long long int Message::get_body_lenght()
{
	return body_lenght;
}
std::string Message::get_sender()
{	
	
	return sender;
}
int Message::get_seq()
{
	return seq;
}
unsigned int Message::get_response_to()
{
	return response_to;
}
unsigned int Message::get_type()
{
	return type;
}
std::string Message::get_full_path()
{
	return full_path;
}
unsigned char* Message::get_body()
{
	
	return this->body;
	
}
void Message::set_body_lenght(unsigned long long int lenght)
{
	this->body_lenght=lenght;
}

void Message::set_body(unsigned char* body)
{
	this->body=body;
}