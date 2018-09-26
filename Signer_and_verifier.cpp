/*
 * Ian Bull:
 * https://gist.github.com/irbull/08339ddcd5686f509e9826964b17bb59
 *
 * 
 * 
*/
#include "Signer_and_verifier.h"
#include <iostream>
Signer_and_verifier::Signer_and_verifier()
{
	set_privateKey();
	set_publicKey();
	
}

Signer_and_verifier::~Signer_and_verifier()
{
}

RSA* Signer_and_verifier::createPrivateRSA(std::string key) {
   RSA *rsa = NULL;
  const char* c_string = key.c_str();
  BIO * keybio = BIO_new_mem_buf((void*)c_string, -1);
  if (keybio==NULL) {
      return 0;
  }
  rsa = PEM_read_bio_RSAPrivateKey(keybio, &rsa,NULL, NULL);
return rsa;
}

RSA* Signer_and_verifier::createPublicRSA(std::string key) {
  RSA *rsa = NULL;
  BIO *keybio;
  const char* c_string = key.c_str();
  keybio = BIO_new_mem_buf((void*)c_string, -1);
  if (keybio==NULL) {
      return 0;
  }
  rsa = PEM_read_bio_RSA_PUBKEY(keybio, &rsa,NULL, NULL);
return rsa;
}

bool Signer_and_verifier::RSASign( RSA* rsa,
              const unsigned char* Msg,
              size_t MsgLen,
              unsigned char** EncMsg,
              size_t* MsgLenEnc) {
  EVP_MD_CTX* m_RSASignCtx = EVP_MD_CTX_create();
  EVP_PKEY* priKey  = EVP_PKEY_new();
  EVP_PKEY_assign_RSA(priKey, rsa);
  if (EVP_DigestSignInit(m_RSASignCtx,NULL, EVP_sha256(), NULL,priKey)<=0) {
      return false;
  }
  if (EVP_DigestSignUpdate(m_RSASignCtx, Msg, MsgLen) <= 0) {
      return false;
  }
  if (EVP_DigestSignFinal(m_RSASignCtx, NULL, MsgLenEnc) <=0) {
      return false;
  }
  *EncMsg = (unsigned char*)malloc(*MsgLenEnc);
  if (EVP_DigestSignFinal(m_RSASignCtx, *EncMsg, MsgLenEnc) <= 0) {
      return false;
  }
  EVP_MD_CTX_free(m_RSASignCtx);
return true;
}

bool Signer_and_verifier::RSAVerifySignature( RSA* rsa,
                         unsigned char* MsgHash,
                         size_t MsgHashLen,
                         const char* Msg,
                         size_t MsgLen,
                         bool* Authentic) {
  *Authentic = false;
  EVP_PKEY* pubKey  = EVP_PKEY_new();
  EVP_PKEY_assign_RSA(pubKey, rsa);
  EVP_MD_CTX* m_RSAVerifyCtx = EVP_MD_CTX_create();

  if (EVP_DigestVerifyInit(m_RSAVerifyCtx,NULL, EVP_sha256(),NULL,pubKey)<=0) {
    return false;
  }
  if (EVP_DigestVerifyUpdate(m_RSAVerifyCtx, Msg, MsgLen) <= 0) {
    return false;
  }
  int AuthStatus = EVP_DigestVerifyFinal(m_RSAVerifyCtx, MsgHash, MsgHashLen);
  if (AuthStatus==1) {
    *Authentic = true;
    EVP_MD_CTX_free(m_RSAVerifyCtx);
    return true;
  } else if(AuthStatus==0){
    *Authentic = false;
    EVP_MD_CTX_free(m_RSAVerifyCtx);
    return true;
  } else{
    *Authentic = false;
    EVP_MD_CTX_free(m_RSAVerifyCtx);
    return false;
}
}
void Signer_and_verifier::Base64Encode( const unsigned char* buffer,
                   size_t length,
                   char** base64Text) {
  BIO *bio, *b64;
  BUF_MEM *bufferPtr;

  b64 = BIO_new(BIO_f_base64());
  bio = BIO_new(BIO_s_mem());
  bio = BIO_push(b64, bio);

  BIO_write(bio, buffer, length);
  BIO_flush(bio);
  BIO_get_mem_ptr(bio, &bufferPtr);
  BIO_set_close(bio, BIO_NOCLOSE);
  BIO_free_all(bio);

*base64Text=(*bufferPtr).data;
}

char* Signer_and_verifier::signMessage(std::string plainText) {
  RSA* privateRSA = createPrivateRSA(this->privateKey); 
  unsigned char* encMessage;
  char* base64Text;
  size_t encMessageLength;
  RSASign(privateRSA, (unsigned char*) plainText.c_str(), plainText.length(), &encMessage, &encMessageLength);
  Base64Encode(encMessage, encMessageLength, &base64Text);
  free(encMessage);
return base64Text;
  
  
}
size_t Signer_and_verifier::calcDecodeLength(const char* b64input) {
  size_t len = strlen(b64input), padding = 0;

  if (b64input[len-1] == '=' && b64input[len-2] == '=') //last two chars are =
    padding = 2;
  else if (b64input[len-1] == '=') //last char is =
    padding = 1;
return (len*3)/4 - padding;
}
void Signer_and_verifier::Base64Decode(const char* b64message, unsigned char** buffer, size_t* length) {
  BIO *bio, *b64;

  int decodeLen = calcDecodeLength(b64message);
  *buffer = (unsigned char*)malloc(decodeLen + 1);
  (*buffer)[decodeLen] = '\0';

  bio = BIO_new_mem_buf(b64message, -1);
  b64 = BIO_new(BIO_f_base64());
  bio = BIO_push(b64, bio);

  *length = BIO_read(bio, *buffer, strlen(b64message));
BIO_free_all(bio);
}
bool Signer_and_verifier::verifySignature(std::string plainText, char* signatureBase64) {
RSA* publicRSA = createPublicRSA(this->publicKey);
  unsigned char* encMessage;
  size_t encMessageLength;
  bool authentic;
  Base64Decode(signatureBase64, &encMessage, &encMessageLength);
  bool result = RSAVerifySignature(publicRSA, encMessage, encMessageLength, plainText.c_str(), plainText.length(), &authentic);
return result & authentic;
}

void Signer_and_verifier::set_privateKey()
{
	this->privateKey="-----BEGIN RSA PRIVATE KEY-----\n"
"MIICXQIBAAKBgQCwctaZ4mCZV+gqXwLgavyFbqDM+xeFKNTLAdoqrZb3IsBV7DDc\n"
"uu11sQ1xsdBhygOXcnnknn3WO9YuzCOYH3j/O77WHPER/qfGcU5cyNJI3x5ot9Se\n"
"/HjXV3lOQCcgfkCzltGJjIUh/KUaUXCwlbHx+Ie7gbXlZCdwvHALtZ7YWwIDAQAB\n"
"AoGAKGQBT/SMHE4NLTXWmLyk1X5/fKy1s5unKllgMYjPF/9ye7i3bq+zj89Zvn7v\n"
"rpLpw49G69iEAbhszQWZ2EbMEW9P8fdJkiO1xnDiRwcBdxdYrQualv0ZiHXlW+hX\n"
"YVx0fQ68K/4747JU5V5Jd/7JBIlJAfbKPZ9dTtUN+Th/rRECQQDodlHrMt6QiVMZ\n"
"cm6MO/yApvbDP/Fh9iD7GytUYxAHnomTSStSUhrqIClXCvjNdwRm3i85bpu8ypeu\n"
"ROyg7UC9AkEAwlCVRZRnJcRzTD45F+Pnz8/vNAhL3TB4MBsToum/YFDX8oP8tuhs\n"
"XOA08UVKbd+/Wnvcwu8OydEfDBtiIsYK9wJAYfViReu00FGeHA4VubCZkhlJyTOL\n"
"440Vjtt/Idtw6NUNAm0F/XWt89VDtnNgLy0lRLgCTIBozmDNFFnlIVd5PQJBAKV8\n"
"h4Q4fRgXFLKME4ebGyNKzUcdAa8gxlsB1FlchRX8A82QRrtnelImyC/qeRbWBVuE\n"
"PWDuw0Nxi+V9tLg0vcUCQQDZigb14f++BByB5qb7yG3Hg+CSbONEjqb5c/tL7nGe\n"
"rRxVgqzbw0aslVjoBGsEJ/RH0mfoPcMVisLGR1Dcsf7/\n"
"-----END RSA PRIVATE KEY-----\n\0";
}

void Signer_and_verifier::set_publicKey()
{
	this->publicKey ="-----BEGIN PUBLIC KEY-----\n"
"MIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQCwctaZ4mCZV+gqXwLgavyFbqDM\n"
"+xeFKNTLAdoqrZb3IsBV7DDcuu11sQ1xsdBhygOXcnnknn3WO9YuzCOYH3j/O77W\n"
"HPER/qfGcU5cyNJI3x5ot9Se/HjXV3lOQCcgfkCzltGJjIUh/KUaUXCwlbHx+Ie7\n"
"gbXlZCdwvHALtZ7YWwIDAQAB\n"
"-----END PUBLIC KEY-----\n";
}