#pragma once

#include <memory>
#include <algorithm>
#include <deque>
#include <mutex>

#include <boost/asio.hpp>

#define MULTITHREAD

#include "database.h"
#include "command_parser.h"

namespace ba = boost::asio;

class RequestHandler : public std::enable_shared_from_this<RequestHandler> {
public:
    explicit RequestHandler(ba::ip::tcp::socket socket, DataBase& db)
            : socket_(std::move(socket)), db_(db)
#ifdef MULTITHREAD
            , pool_(4)
#endif
    {
        std::fill(data_.begin(), data_.end(), 0);
    }

    void start();

private:
    ba::ip::tcp::socket socket_;
    std::array<char, 1024> data_{};
    DataBase& db_;
    CommandParser cmd_parser_;
    std::deque<std::string> responses_;
#ifdef MULTITHREAD
    ba::thread_pool pool_;
    std::mutex mtx_;
#endif

    //some handler async::handle_t handle_;

    // methods
    void do_read();
    void do_write();
};


class JoinServer {
public:
    JoinServer(ba::io_service& io_context, const ba::ip::tcp::endpoint& endpoint,
                DataBase& db)
            : acceptor_(io_context, endpoint), db_(db)
    {
        do_accept();
    }

private:
    void do_accept();

    boost::asio::ip::tcp::acceptor acceptor_;
    DataBase& db_;
};