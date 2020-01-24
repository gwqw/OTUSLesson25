#include "command_parser.h"

#include <string_view>

#include "str_utils.h"

using namespace std;

CommandParser::Commands CommandParser::parseCommands(char* data, std::size_t data_size) {
    string_view sv(data, data_size);
    Commands res;
    while (!sv.empty()) {
        auto pos = sv.find('\n');
        if (!buffer_.empty()) {
            buffer_ += sv.substr(0, pos);
            if (pos != string_view::npos) {
                res.push_back(parseCommandFrom(buffer_));
            }
            buffer_.clear();
        }
        if (pos != string_view::npos) {
            res.push_back(parseCommandFrom(sv.substr(0, pos)));
        } else {
            buffer_ += sv.substr(0, pos);
            break;
        }
        sv.remove_prefix(min(pos+1, sv.size()));
    }
    return res;
}
