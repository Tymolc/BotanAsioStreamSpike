//
// Copyright (c) 2016-2017 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/beast
//

//------------------------------------------------------------------------------
//
// Example: HTTP SSL client, synchronous
//
//------------------------------------------------------------------------------

#include <boost/asio/error.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>

#include <cstdlib>
#include <iostream>
#include <string>

#include <botan/asio_stream.h>
#include <botan/auto_rng.h>
#include <botan/certstor_system.h>
#include <botan/credentials_manager.h>
#include <botan/tls_client.h>

#include <cpprest/http_client.h>

#include "botan_stream_helpers.hpp"

using tcp = boost::asio::ip::tcp;  // from <boost/asio/ip/tcp.hpp>
// namespace ssl = boost::asio::ssl;     // from <boost/asio/ssl.hpp>
namespace http = boost::beast::http;  // from <boost/beast/http.hpp>

// Performs an HTTP GET and prints the response
int main(int argc, char **argv)
{
    try {
        // Check command line arguments.
        if (argc != 4 && argc != 5) {
            std::cerr << "Usage: http-client-sync-ssl <host> <port> <target>\n"
                      << "Example:\n"
                      << "    http-client-sync-ssl www.example.com 443 /\n";
            return EXIT_FAILURE;
        }
        auto const host    = argv[1];
        auto const port    = argv[2];
        auto const target  = argv[3];
        const int  version = 11;

        boost::asio::io_context ioc;
        tcp::resolver           resolver{ioc};
        tcp::socket             socket{ioc};

        Botan::AutoSeeded_RNG                 rng;
        Botan::TLS::Session_Manager_In_Memory sessionManager(rng);
        TLS_Policy policy(Botan::TLS::Protocol_Version::TLS_V12);
        Botan::TLS::Server_Information serverInformation =
            Botan::TLS::Server_Information(host, port);

        CredentialsManager credentialsManager;

        web::http::client::http_client_config config(
            Botan::TLS::Context(credentialsManager, rng, sessionManager, policy,
                                serverInformation));

        web::http::client::http_client client(host, std::move(config));

        web::http::http_request request{web::http::methods::GET};
        request.set_request_uri(target);

        client.request(request)
            .then([](web::http::http_response response) {
                return response.extract_string();
            })
            .then([](utility::string_t response) {
                std::cout << response << std::endl;
            })
            .get();

    } catch (std::exception const &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
