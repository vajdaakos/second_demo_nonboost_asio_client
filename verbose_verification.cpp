/* verbouse_verifier class template based on Tanner Sansbury's work:
 * https://stackoverflow.com/questions/28264313/ssl-certificates-and-boost-asio
 * 
*/
#include "verbose_verification.h"
	template <typename Verifier>
	bool verbose_verification<Verifier>::operator()(
    bool preverified,
    asio::ssl::verify_context& ctx
  )
  {
	char subject_name[256];
    X509* cert = X509_STORE_CTX_get_current_cert(ctx.native_handle());
    X509_NAME_oneline(X509_get_subject_name(cert), subject_name, 256);
    bool verified = verifier_(preverified, ctx);
    std::cout << "Verifying: " << subject_name << "\n"
                 "Verified: " << verified << std::endl;
    return verified;
  }

	template <typename Verifier>
	verbose_verification<Verifier>
	make_verbose_verification(Verifier verifier)
	{
		 return verbose_verification<Verifier>(verifier);
	}
