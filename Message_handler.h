#ifndef MESSAGE_HANDLER_H
#define MESSAGE_HANDLER_H
#include "Message.h"
#include "Authentication_Message.h"
#include "Ask_job_message.h"
#include <iostream>
#include <string>
#include "asio.hpp"
#include "asio/ssl.hpp"
#include <vector>
#include <thread>
#include <string>
namespace ssl = asio::ssl;
using asio::ip::tcp;
typedef ssl::stream<tcp::socket> ssl_socket;
namespace ssl = asio::ssl;

typedef ssl::stream<tcp::socket> ssl_socket;

class Message_handler:public std::enable_shared_from_this<Message_handler>
{
public:
	Message_handler(asio::io_service&, ssl_socket&,std::string);
	~Message_handler();
	void send_multiple_message(std::vector<Message>,int);
	void send_incremetntly_job_reqs(Message);
	void send_job_reqs(Message);
	Authentication_Message make_message(std::string fqdn);
	unsigned int get_start_seq();
	void start();
	bool start_authenticate();
	void start_with_asking_job(int);
	unsigned int message_seq;
	void do_job_answer_header_read();
	void do_job_answer_body_read(unsigned long long int ,std::string,std::string);
	void send_new_job_req_at_beg();
	void send_new_job_req();
	void set_pool_size(unsigned int);
	void stop_pool();
	
private:
	asio::io_service worker_service;
	void shutdown();
	
	std::shared_ptr<asio::io_service::work> worker;
	
protected:
bool no_new_send=false;
   

	asio::io_service& message_io_service;
	ssl_socket& socket_;
	bool authenticated=true;
	unsigned int start_seq;
	void set_start_seq();
	std::string fqdn;
	unsigned int thread_pool_size;
	unsigned char large_buffer[5120];
	void set_body_lenght_message_field(Message, unsigned char*);
	void set_sender_message_field(Message,unsigned char* );
	void set_seq_message_field(Message, unsigned char* );
	void set_response_to_message_field(Message, unsigned char* );
	void set_type_message_field(Message, unsigned char* );
	void set_full_path_message_field(Message, unsigned char* );
	void set_body_message_field(Message, unsigned char* );
	unsigned int job_ask_count=0;
	unsigned long long int get_full_body_lenght(unsigned char*);
	unsigned int get_seq(unsigned char*);
	std::string get_sender(unsigned char*);
	unsigned int get_response_to(unsigned char*);
	unsigned int get_type(unsigned char*);
	std::string get_full_path(unsigned char*);
	
	void get_body(char*,unsigned long long int, char*);

	unsigned char header[512] ;
	unsigned char* body;
	void print_message_by_fields(int, unsigned long long int, 
	std::string, unsigned int,unsigned int,unsigned int, 
	std::string, char*);
	
	void send_message_postprocess(std::error_code const &, 
	std::size_t);
	void runjob(std::string);
	std::vector<std::thread> work_threads;
	void postrunjob(std::string);
	};

#endif // MESSAGE_HANDLER_H
