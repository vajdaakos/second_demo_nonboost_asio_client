#ifndef MESSAGE_H
#define MESSAGE_H
#include <string>
#include <cstring>
#include <cstdint>
class Message
{

public:
	~Message();
	void set_body_lenght(unsigned long long int);
	void set_body(unsigned char*);
	unsigned long long int get_body_lenght();
	std::string get_sender();
	int get_seq();
	unsigned int get_response_to();
	unsigned int get_type();
	std::string get_full_path();
	void set_sender(std::string);
	void set_seq(unsigned int);
	void set_response_to(unsigned int);
	void set_full_path(std::string);
	
	unsigned char* get_body();
	

protected:
	Message();
	unsigned long long int body_lenght;
	std::string sender;
	unsigned int seq;
	unsigned int response_to;
	unsigned int type;
	std::string full_path;
	void set_type(unsigned int);
	unsigned char* body=NULL;
	
	
	
	

};

#endif // MESSAGE_H
