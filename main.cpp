/*
 * Signing and verification with OpenSSL is based on
 * Ian Bull:
 * https://gist.github.com/irbull/08339ddcd5686f509e9826964b17bb59
 */
 ////////////////////////////////////////////////////////////////////////
/* verbouse_verifier class template based on Tanner Sansbury's work:
 * https://stackoverflow.com/questions/28264313/ssl-certificates-and-boost-asio
 * 
*/

#include <cstdio>
#include <iostream>
#include "Connection.h"
#include "Signer_and_verifier.h"
#include <string>
#include <memory>

int main(int argc, char **argv)
{
if (argc != 5)
	{
  std::cout << argc;
  std::cout << "Usage: demo <server> <port> thread_count fqdn\n";
  std::cout << "Example:\n";
  std::cout << "./demo asrock.local 443 5 client01.secudit.hu";
  return 1;
	}

//Connection c;
std::shared_ptr<Connection> c(new Connection);
std::string fqdn(argv[4]);
c->start(argv[1],argv[2],std::atoi(argv[3]),fqdn);
return 0;

}
