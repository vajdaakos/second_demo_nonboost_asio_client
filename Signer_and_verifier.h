/*based on
 * Ian Bull:
 * https://gist.github.com/irbull/08339ddcd5686f509e9826964b17bb59
 *
 * 
 * 
*/
#ifndef SIGNER_AND_VERIFIER_H
#define SIGNER_AND_VERIFIER_H
#include <iostream>
#include <openssl/aes.h>
#include <openssl/evp.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/bio.h>
#include <openssl/err.h>
#include <assert.h>
#include <cstring>
class Signer_and_verifier
{
public:
	Signer_and_verifier();
	~Signer_and_verifier();
	void set_privateKey();
	char* signMessage(std::string);
	std::string privateKey;
	std::string publicKey;
	void set_publicKey();
	bool verifySignature(std::string, char*);
private:
	//void std::string publicKey;
	RSA* createPrivateRSA(std::string);
	RSA* createPublicRSA(std::string);
	bool RSASign( RSA*, const unsigned char*, size_t, unsigned char**, 
	size_t*);
	bool RSAVerifySignature( RSA*, unsigned char*, size_t, const char*, size_t,
	bool*);
	void Base64Encode(const unsigned char* , size_t, char** );
	void Base64Decode(const char*, unsigned char**, size_t*);
	size_t calcDecodeLength(const char*);
 
	

};

#endif // SIGNER_AND_VERIFIER_H
