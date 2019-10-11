#include <iostream>

#include <botan/credentials_manager.h>
#include <botan/tls_policy.h>
#include <botan/certstor_system.h>


class CredentialsManager : public Botan::Credentials_Manager
{
public:
    //! \brief Return the list of trusted certificate authorities
    std::vector<Botan::Certificate_Store *>
    trusted_certificate_authorities(const std::string &type,
                                    const std::string &hostname) override 
    {
        std::cout << "Getting Certs" << std::endl;
        return {&_systemCertStore};
    }

  private:
    Botan::System_Certificate_Store _systemCertStore;
};

class TLS_Policy : public Botan::TLS::Policy
{
  public:
    TLS_Policy(Botan::TLS::Protocol_Version req_version)
        : m_version(req_version)
    {
    }

    bool allow_tls10() const override
    {
        return m_version == Botan::TLS::Protocol_Version::TLS_V10;
    }
    bool allow_tls11() const override
    {
        return m_version == Botan::TLS::Protocol_Version::TLS_V11;
    }
    bool allow_tls12() const override
    {
        return m_version == Botan::TLS::Protocol_Version::TLS_V12;
    }

    bool require_cert_revocation_info() const override
    {
        return false;
    }

  private:
    Botan::TLS::Protocol_Version m_version;
};
