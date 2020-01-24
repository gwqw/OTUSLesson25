#include "join_server.h"

#include <iostream>
#include <utility>

#include "command.h"
#include "command_handler.h"

using namespace std;

void RequestHandler::start() {
    do_read();
}

void RequestHandler::do_read() {
    auto self(shared_from_this());
    socket_.async_read_some(
            ba::buffer(data_),
            [this, self](boost::system::error_code ec, std::size_t length)
            {
                if (!ec) {
                    if (length) {
//                        cerr << "Read " << length << "bytes: "
//                             << string(data_.begin(), data_.begin() + length)
//                             << endl;
                        auto input_cmds = cmd_parser_.parseCommands(data_.data(), length);
                        // TODO: move run task to thread_pool
#ifdef MULTITHREAD
#ifdef OLD_BOOST
                        post(
#else
                        ba::post(
#endif
                            pool_, [this, input_cmds = move(input_cmds) /*<--coolest lifehack*/]() mutable {
                                lock_guard<mutex> lk(mtx_);
                                for (auto &cmd : input_cmds) {
                                    auto respHolder = getResponseCommand(move(cmd));
                                    bool write_in_progress = !responses_.empty();
                                    responses_.push_back(respHolder->runCommand(db_));
                                    if (!write_in_progress) {
                                        do_write();
                                    }
                                }
                            }
                        );
#else
                        for (auto &cmd : input_cmds) {
                            auto respHolder = getResponseCommand(move(cmd));
                            bool write_in_progress = !responses_.empty();
                            responses_.push_back(respHolder->runCommand(db_));
                            if (!write_in_progress) {
                                do_write();
                            }
                        }
#endif
                    }

                    // handle data: async::receive(handle_, data_.data(), length);
                    do_read();
                }
#ifdef MULTITHREAD
                else {
                    pool_.join();
                }
#endif
            }
    );
}

void RequestHandler::do_write() {
    auto self(shared_from_this());
    boost::asio::async_write(socket_,
            boost::asio::buffer(responses_.front().data(), responses_.front().length()),
            [this, self](boost::system::error_code ec, std::size_t /*length*/)
                     {
                         if (!ec) {
#ifdef MULTITHREAD
                             lock_guard<mutex> lk(mtx_);
#endif
                             responses_.pop_front();
                             if (!responses_.empty())
                             {
                                 do_write();
                             } else {
                                 //cerr << "done" << endl;
                             }
                         }
                     }
     );
}

void JoinServer::do_accept() {
    acceptor_.async_accept(socket_,
            [this](boost::system::error_code ec)
            {
                if (!ec) {
                    std::make_shared<RequestHandler>(std::move(socket_), db_)->start();
                }

                do_accept();
            }
    );
}