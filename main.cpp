#include <iostream>

#include "join_server.h"
#include "database.h"

using namespace std;
namespace ba = boost::asio;
using boost::asio::ip::tcp;

int main(int argc, char* argv[]) {
    try
    {
        if (argc < 2)
        {
            std::cerr << "Usage: join_server <port>\n";
            return 1;
        }

        ba::io_service io_context;

        tcp::endpoint endpoint(tcp::v4(), stol(argv[1]));
        DataBase db;

        JoinServer server(io_context, endpoint, db);

        io_context.run();
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}
