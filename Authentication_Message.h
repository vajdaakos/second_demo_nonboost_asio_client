#ifndef AUTHENTICATION_MESSAGE_H
#define AUTHENTICATION_MESSAGE_H
#include "Message.h"

#include <string>

class Authentication_Message : public Message
{
public:
	Authentication_Message(std::string, unsigned int);
	~Authentication_Message();
	
private:

};

#endif // AUTHENTICATION_MESSAGE_H
