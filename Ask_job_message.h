#ifndef ASK_JOB_MESSAGE_H
#define ASK_JOB_MESSAGE_H
#include "Message.h"

#include <string>
class Ask_job_message:public Message
{
public:
	Ask_job_message(std::string, 
unsigned int );
	~Ask_job_message();

};

#endif // ASK_JOB_MESSAGE_H
