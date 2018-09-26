#include "Authentication_Message.h"
#include "Signer_and_verifier.h"
Authentication_Message::Authentication_Message(std::string fqdn_, 
unsigned int start_seq_)
{
	set_type(0);
	set_sender(fqdn_);
	set_full_path("default");
	set_response_to(0);
	set_seq(start_seq_);

	Signer_and_verifier verifier;

	char* signature = verifier.signMessage(fqdn_);
	set_body(reinterpret_cast<unsigned char *>(signature));
	int s = strlen((char*)signature);
	this->body_lenght=s;
	
	bool authentic=verifier.verifySignature(fqdn_,reinterpret_cast<char *>(this->body));
	if ( !authentic ) {
    std::cout << "NOT Authentic!!!" << std::endl;
  } 
	
	
}

Authentication_Message::~Authentication_Message()
{
}
