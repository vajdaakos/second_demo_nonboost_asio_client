#include "Ask_job_message.h"

Ask_job_message::Ask_job_message(std::string fqdn_, 
unsigned int seq_)
{
	set_type(2);
	set_sender(fqdn_);
	set_full_path("default");
	set_response_to(0);
	set_seq(seq_);
	this->body_lenght=0;
	
}

Ask_job_message::~Ask_job_message()
{
	free(body);
}

