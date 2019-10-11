#pragma once

#include <iostream>

#include <boost/asio/buffer.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/asio/write.hpp>
#include <boost/bind.hpp>

using tcp = boost::asio::ip::tcp;

class async_socket_handler
{
  public:
    async_socket_handler(tcp::socket socket, boost::asio::io_context &ctx)
        : _stopped(false)
        , _stream(socket, ctx)
    {
    }

    virtual ~async_socket_handler() = default;

    void connect(tcp::resolver::results_type endpoints)
    {
        _endpoints = endpoints;
        start_connect(_endpoints.begin());
    }

    void stop()
    {
        _stopped = true;
        boost::system::error_code ignored_ec;
        _stream.close(ignored_ec);
    }

  protected:
    tcp::resolver::results_type _endpoints;
    Botan::TLS::Stream          _stream;
    bool                        _stopped;
    char                        _input_buffer[4096];

    virtual void init_client()                          = 0;
    virtual void received_data(const std::string &data) = 0;

    void start_connect(tcp::resolver::results_type::iterator endpoint_iter)
    {
        if (endpoint_iter != _endpoints.end()) {
            std::cout << "Trying " << endpoint_iter->endpoint() << "...\n";

            // Start the asynchronous connect operation.
            _stream.async_connect(
                endpoint_iter->endpoint(),
                boost::bind(&async_socket_handler::handle_connect, this, _1,
                            endpoint_iter));
        } else {
            // There are no more endpoints to try. Shut down the client.
            std::cout << "closing socket" << std::endl;
            stop();
        }
    }

    void handle_connect(const boost::system::error_code &     ec,
                        tcp::resolver::results_type::iterator endpoint_iter)
    {
        if (_stopped)
            return;

        // The async_connect() function automatically opens the socket at
        // the start of the asynchronous operation. If the socket is closed
        // at this time then the timeout handler must have run first.
        if (!_stream.is_open()) {
            std::cout << "Connect timed out\n";

            // Try the next available endpoint.
            start_connect(++endpoint_iter);
        }

        // Check if the connect operation failed before the deadline
        // expired.
        else if (ec) {
            std::cout << "Connect error: " << ec.message() << "\n";

            // We need to close the socket used in the previous connection
            // attempt before starting a new one.
            _stream.close();

            // Try the next available endpoint.
            start_connect(++endpoint_iter);
        }

        // Otherwise we have successfully established a connection.
        else {
            std::cout << "Connected to " << endpoint_iter->endpoint() << "\n";
            init_client();
            // Start the input actor.
            // start_read();

            // Start the heartbeat actor.
            // start_write();
        }
    }

    void start_read()
    {
        // Start an asynchronous operation to read a newline-delimited message.
        _stream.async_read_some(
            boost::asio::buffer(_input_buffer, 4 * 1024),
            boost::bind(&async_socket_handler::handle_read, this, _1, _2));
    }

    void handle_read(const boost::system::error_code &ec, std::size_t n)
    {
        if (_stopped)
            return;

        if (!ec) {
            // Extract the newline-delimited message from the buffer.
            if (n > 0) {
                received_data(std::string(_input_buffer, n));
            }

            start_read();
        } else {
            std::cout << "Error on receive: " << ec.message() << "\n";

            stop();
        }
    }

    void start_write(const std::string &msg)
    {
        if (_stopped)
            return;

        // Start an asynchronous operation to send a heartbeat message.
        boost::asio::async_write(
            _stream, boost::asio::buffer(msg),
            boost::bind(&async_socket_handler::handle_write, this, _1));
    }

    void handle_write(const boost::system::error_code &ec)
    {
        if (_stopped)
            return;

        if (!ec) {
            // // Wait 10 seconds before sending the next heartbeat.
            // heartbeat_timer_.expires_after(boost::asio::chrono::seconds(10));
            // heartbeat_timer_.async_wait(
            //     boost::bind(&client::start_write, this));
        } else {
            std::cout << "Error on write: " << ec.message() << "\n";
            stop();
        }
    }
};
