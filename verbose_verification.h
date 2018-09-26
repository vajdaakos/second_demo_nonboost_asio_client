/* verbouse_verifier class template based on Tanner Sansbury's work:
 * https://stackoverflow.com/questions/28264313/ssl-certificates-and-boost-asio
 * 
*/
#ifndef VERBOSE_VERIFICATION_H
#define VERBOSE_VERIFICATION_H
#include "asio.hpp"
#include "asio/ssl.hpp"
#include <iostream>
template <typename Verifier>
class verbose_verification
{
public:
	verbose_verification(Verifier verifier): verifier_(verifier) {};
	bool operator()(bool preverified, asio::ssl::verify_context& ctx);
	verbose_verification<Verifier> make_verbose_verification(Verifier verifier);

private:
  Verifier verifier_;

};

#endif // VERBOSE_VERIFICATION_H
