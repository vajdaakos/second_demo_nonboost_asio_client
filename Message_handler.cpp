#include "Message_handler.h"
#include <vector>
#include <type_traits>
#include <asio/buffer.hpp>
#include <cstdint>
#include <thread>
#include <fstream>

Message_handler::Message_handler(asio::io_service& io_service_, 
ssl_socket& message_socket_,std::string fqdn_)
:message_io_service(io_service_) ,socket_(message_socket_)

{
	this->fqdn=fqdn_;
	set_start_seq();
	
} 
Message_handler::~Message_handler()
{
}

unsigned long long int Message_handler::get_full_body_lenght
(unsigned char* byte_array)
  {
	int byte_position=0;
	unsigned long long int full_body_lenght;
	memcpy(&full_body_lenght,byte_array+byte_position,sizeof (unsigned long long int));
	return full_body_lenght;
  }
  unsigned int Message_handler::get_seq(unsigned char* byte_array)
  {
	unsigned int seq;
	int byte_position=sizeof(unsigned long long int)+64;
	memcpy(&seq,byte_array+byte_position,sizeof (unsigned int));
    return seq;
  }
  std::string Message_handler::get_sender(unsigned char* byte_array)
  {
	  int byte_position=sizeof(unsigned long long int);
	  char csender [64];
	  memcpy(csender,byte_array+byte_position,64);
	  std::string sender(csender);
	  return sender;
  }
  unsigned int Message_handler::get_response_to(unsigned char* byte_array)
  {
	unsigned int response_to;
	int byte_position=sizeof(unsigned long long int)+64+sizeof (unsigned int);
	memcpy(&response_to,byte_array+byte_position,sizeof (unsigned int));
    return response_to;
  }
  unsigned int Message_handler::get_type(unsigned char* byte_array)
  {
	unsigned int type;
	int byte_position=sizeof(unsigned long long int)+64+sizeof (unsigned int)+sizeof (unsigned int);
	memcpy(&type,byte_array+byte_position,sizeof (unsigned int));
    return type;
  }
std::string Message_handler::get_full_path(unsigned char* byte_array)
  {
	  int byte_position=sizeof(unsigned long long int)+64+sizeof (unsigned int)+sizeof (unsigned int)+sizeof(unsigned int);
	  char cfullpath [428];
	  memcpy(cfullpath,byte_array+byte_position,428);
	  std::string full_path(cfullpath);
	  return full_path;
  }
void Message_handler::print_message_by_fields(int bytes_transferred, unsigned long long int full_body_lenght, std::string sender, unsigned int seq,unsigned int response_to,unsigned int type, std::string full_path,char* body)
{
	  std::cout<<"bytes transferred        : "<<bytes_transferred<<std::endl;
	  std::cout<<"body_lenght              : "<<full_body_lenght<<std::endl;
	  std::cout<<"sender                   : "<<sender<<std::endl;
	  std::cout<<"seq                      : "<<seq<<std::endl;
	  std::cout<<"response to              : "<<response_to<<std::endl;
	  std::cout<<"type                     : "<<type<<std::endl;
	  std::cout<<"full path                : "<<full_path<<std::endl;
	  std::string string_body(body);
	  std::cout<<"body                     : "<<body<<std::endl; 
	  
}
void Message_handler::get_body(char* byte_array,unsigned long long int full_body_lenght, char* body)
{
	  int byte_position=sizeof(unsigned long long int)+64+sizeof (unsigned int)+sizeof (unsigned int)+sizeof(unsigned int)+428;
	  std::cout<<"header lenght: "<<byte_position<<std::endl;
	  memcpy(body,byte_array+byte_position,full_body_lenght);
	  
}

void Message_handler::set_body_lenght_message_field(Message message,
unsigned char* buffer)
{
	unsigned int byte_position=0;
	unsigned long long int body_lenght=message.get_body_lenght();
	std::cout<<"body_length: "<<body_lenght<<std::endl;
	memcpy(buffer+byte_position,&body_lenght,sizeof (unsigned long long int));
}

void Message_handler::set_sender_message_field(Message message, 
unsigned char* buffer)
{
	unsigned int byte_position=sizeof (unsigned long long int);
	std::string sender=message.get_sender();
	
	char csender[64];
	strcpy(csender,sender.c_str());
	memcpy(buffer+byte_position,csender,64);
	
}
void Message_handler::set_seq_message_field(Message message,
unsigned char* buffer)
{
	unsigned int byte_position=sizeof (unsigned long long int)+64;
	unsigned int seq=message.get_seq();
	
	memcpy(buffer+byte_position,&seq,sizeof (unsigned int));
}

void Message_handler::set_response_to_message_field(Message message,
unsigned char* buffer)
{
	unsigned int byte_position=sizeof (unsigned long long int)+64+
	sizeof (unsigned int);
	unsigned int response_to=message.get_response_to();
	memcpy(buffer+byte_position,&response_to,sizeof (unsigned int));
}

void Message_handler::set_type_message_field(Message message,
unsigned char* buffer)
{
	unsigned int byte_position=sizeof (unsigned long long int)+64+
	sizeof (unsigned int)+sizeof (unsigned int);
	unsigned int type=message.get_type();
	std::cout<<"type: "<<type<<std::endl;
	memcpy(buffer+byte_position,&type,sizeof (unsigned int));
}

void Message_handler::set_full_path_message_field(Message message, 
unsigned char* buffer)
{
	unsigned int byte_position=sizeof (unsigned long long int)+64+
	sizeof (unsigned int)+sizeof (unsigned int)+sizeof (unsigned int);
	std::string full_path=message.get_full_path();
	char cfullpath[428];
	strcpy(cfullpath,full_path.c_str());
	memcpy(buffer+byte_position,cfullpath,428);
	
	
}
void Message_handler::set_body_message_field(Message message, 
unsigned char* buffer)
{
	
	unsigned char* body=message.get_body();
	
	int n=message.get_body_lenght();
	memcpy(buffer+512,body,n);
	
}



void Message_handler::set_start_seq()
{
		srand((int)time(0));
		this->start_seq= (rand() % 1024) + 1;
}
unsigned int Message_handler::get_start_seq()
{
	return start_seq;
}
void Message_handler::start_with_asking_job(int thred_pool_size)
{
	
	for(int i=0;i<thred_pool_size;i++)
	{
		worker=std::make_shared<asio::io_service::work>(worker_service);
		work_threads.push_back(std::thread([&](){
		std::cout<<"working thread started"<<std::endl;
		worker_service.run();
		std::cout<<"working thread stoped"<<std::endl;
		}));
	
	}
	std::cout<<"Poolsize:"<<thred_pool_size<<std::endl;
	/*std::vector<Message> messages;
	for(int i=0;i<thred_pool_size;++i)
		{
			std::cout<<i<<std::endl;
			Ask_job_message ask_job_message(fqdn,message_seq);
			message_seq++;
			messages.push_back(ask_job_message);
			//this->send_message(ask_job_message);
			//std::cout<<ask_job_message.get_seq()<<std::endl;
			
			
		
		}
	send_multiple_message(messages,thred_pool_size);*/
	
		Ask_job_message ask_job_message(fqdn,message_seq);
		message_seq++;
		send_incremetntly_job_reqs(ask_job_message);
	
	std::cout<<"Itt kezd futni a service."<<std::endl;
	
	message_io_service.run();
	std::cout<<"Itt már nem fut a service!"<<std::endl;
}
void Message_handler::send_new_job_req_at_beg(){
	Ask_job_message ask_job_message(fqdn,message_seq);
		message_seq++;
		send_incremetntly_job_reqs(ask_job_message);
}
void Message_handler::send_new_job_req(){
	Ask_job_message ask_job_message(fqdn,message_seq);
		message_seq++;
		send_job_reqs(ask_job_message);
}

void Message_handler::send_job_reqs(Message message)
{
	if(!no_new_send)
	{
		
	unsigned char binary_message[512];
	set_body_lenght_message_field(message, binary_message);
	set_sender_message_field(message, binary_message);
	set_seq_message_field(message, binary_message);
	set_response_to_message_field(message, binary_message);
	set_type_message_field(message, binary_message);
	set_full_path_message_field(message, binary_message);
	
		asio::async_write(socket_,
	asio::buffer(binary_message,512),
	[=,me=shared_from_this()](std::error_code const & ec, std::size_t bytes_xfer)
	{
		
		std::cout<<"Sent:"<<bytes_xfer<<std::endl;
		
	});
	}
	else{
		std::cout<<"No more jobs"<<std::endl;
	}
	
	
	
}
void Message_handler::send_incremetntly_job_reqs(Message message)
{
	unsigned char binary_message[512];
	set_body_lenght_message_field(message, binary_message);
	set_sender_message_field(message, binary_message);
	set_seq_message_field(message, binary_message);
	set_response_to_message_field(message, binary_message);
	set_type_message_field(message, binary_message);
	set_full_path_message_field(message, binary_message);
	if(job_ask_count<thread_pool_size){
		asio::async_write(socket_,
	asio::buffer(binary_message,512),
	[=,me=shared_from_this()](std::error_code const & ec, std::size_t bytes_xfer)
	{
		me->job_ask_count++;
		std::cout<<"Sent:"<<bytes_xfer<<std::endl;
		me->send_new_job_req_at_beg();
	});
	}
	else
	{
		std::cout<<"Waiting for job:"<<std::endl;
		do_job_answer_header_read();
	}
	
	
}

void Message_handler::send_multiple_message(std::vector<Message> messages, int i)
{
	
	for(int j=0;j<i;j++){
	std::cout<<j<<std::endl;
	Message message=messages.at(j);
	unsigned char binary_message[512];
	set_body_lenght_message_field(message, binary_message);
	set_sender_message_field(message, binary_message);
	set_seq_message_field(message, binary_message);
	set_response_to_message_field(message, binary_message);
	set_type_message_field(message, binary_message);
	set_full_path_message_field(message, binary_message);
	memcpy(large_buffer+j*512,binary_message,512);
	}
	
	//std::cout<<"Message type:"<<message.get_type()<<std::endl;
	//std::cout<<"body_lenght:"<<message.get_body_lenght()<<std::endl;
	

	asio::async_write(socket_,
	asio::buffer(large_buffer,512*i),
	[me=shared_from_this()](std::error_code const & ec, std::size_t bytes_xfer)
	{
		if(!ec)
		{
			std::cout<<"Message sent, bytes transferred"<<bytes_xfer<<std::endl;
			//me->do_job_answer_header_read();
		}
		else
		{
			std::cout<<ec<<std::endl;
		}
		//check_message_answer(ec, bytes_xfer);
		//me->send_message_postprocess(ec, bytes_xfer);
	});
	
	
	
	
}
void Message_handler::start(){
	
}
void Message_handler::do_job_answer_header_read()
{

		asio::async_read(socket_,asio::buffer(header),
        [self=shared_from_this()](std::error_code ec, std::size_t bytes_transferred)
        {
          if (!ec) 
          {
			  
			std::cout<<"Messige arrived, headerlength:"<<bytes_transferred<<std::endl;
			std::cout<<"reading header..."<<std::endl;
			unsigned long long int body_lenght=self->get_full_body_lenght(self->header);
			std::cout<<"body lenght: "<<body_lenght<<std::endl;
			std::string sender=self->get_sender(self->header);
			std::cout<<"sender: "<<sender<<std::endl;
			unsigned int seq=self->get_seq(self->header);
			std::cout<<"seq: "<<seq<<std::endl;
			self->message_seq=seq;
			unsigned int response_to=self->get_response_to(self->header);
			std::cout<<"response_to: "<<response_to<<std::endl;
			unsigned int type=self->get_type(self->header);
			std::cout<<"type_be: "<<type<<std::endl;
			std::string full_path=self->get_full_path(self->header);
			std::cout<<"path: "<<full_path<<std::endl;
			std::cout<<"body lenght: "<<body_lenght<<std::endl;
			  if(type==11)
			{
				self->shutdown();
				  self->no_new_send=true;
				  return;
			  }
			  if (body_lenght>0)
				self->do_job_answer_body_read(body_lenght,sender,full_path);

          }
          else
          {
			  std::cout<<ec<<std::endl;
			  
			  return;

          }
        });
}

void Message_handler::do_job_answer_body_read
(unsigned long long int full_body_lenght,std::string sender
,std::string full_path_)
{
	
	body=(unsigned char*)malloc(full_body_lenght);
    asio::async_read(socket_,asio::buffer(body,full_body_lenght),
        [self=shared_from_this(),sender,full_path_]
		(std::error_code ec, std::size_t bytes_transferred)
        {
          if (!ec) 
			{	
				std::cout<<"Messige arrived, bodylength:"<<bytes_transferred<<std::endl;
				std::ofstream jobfile(full_path_, 
				std::ios::out| std::ios::binary);
				jobfile<<self->body;
				jobfile.close();
				self->do_job_answer_header_read();
				self->postrunjob(full_path_);
				
			}
			
		else
			{
				//self->do_job_answer_header_read();
			}
		});
		
}
void Message_handler::shutdown()
{
	worker.reset();
	message_io_service.stop();
}
void Message_handler::runjob(std::string jobfile)
{
	std::string commandstring="chmod +x "+jobfile;
	std::cout<<commandstring<<std::endl;
	
	char command[jobfile.size()+10];
	strncpy(command, commandstring.c_str(), sizeof(commandstring));
	command[sizeof(commandstring) + 1] = 0;
	system(command);
	commandstring=jobfile;
	strncpy(command, commandstring.c_str(), sizeof(commandstring));
	command[sizeof(commandstring) + 1] = 0;
	std::cout<<"command:"<<command<<std::endl;
	
	system(command);
	std::cout<<"-------------------\n"<<std::endl;
	if(!no_new_send)
		{
			send_new_job_req();
			do_job_answer_header_read();	
		}
	
	
	
}

void Message_handler::stop_pool()
{
	
	for (auto& th : work_threads){
			
    th.join();
	} 
	
}
void Message_handler::set_pool_size(unsigned int size){
	this->thread_pool_size=size;
}
void Message_handler::postrunjob(std::string full_path_)
{
	worker_service.post([self=shared_from_this(),full_path_](){
					self->runjob(full_path_);
				});
}