#include "str_utils.h"

#include <optional>
#include <charconv>

std::vector<std::string> split(const std::string& str, char d)
{
    std::vector<std::string> r;

    std::string::size_type start = 0;
    std::string::size_type stop = str.find_first_of(d);
    while (stop != std::string::npos)
    {
        r.push_back(str.substr(start, stop - start));

        start = stop + 1;
        stop = str.find_first_of(d, start);
    }

    r.push_back(str.substr(start));

    return r;
}

std::string get_first_word(const std::string& str, char d) {
    if (auto stop = str.find_first_of(d); stop != std::string::npos) {
        return str.substr(0, stop);
    }
    return str;
}

std::pair<std::string_view, std::optional<std::string_view>> split_two_strict(
        std::string_view data, std::string_view delimiter) {
    const auto pos = data.find(delimiter);
    if (pos != data.npos) {
        return {data.substr(0, pos), data.substr(pos + delimiter.size())};
    } else {
        return {data, std::nullopt};
    }
}

std::pair<std::string_view, std::string_view > split_two(
        std::string_view data, std::string_view delimiter) {
    const auto [lhs, rhs_opt] = split_two_strict(data, delimiter);
    return {lhs, rhs_opt.value_or("")};
}

std::string_view read_token(std::string_view& data, std::string_view delimiter) {
    const auto [lhs, rhs] = split_two(data, delimiter);
    data = rhs;
    return lhs;
}

int convertToInt(std::string_view str) {
    int res;
    if (auto [p, ec] = std::from_chars(str.begin(), str.end(), res); ec != std::errc()) {
        throw std::invalid_argument("bad cast to integer value ");
    }
    return res;
}

int convertToIntDef(std::string_view str, int def_value) noexcept {
    try {
        return convertToInt(str);
    } catch (const std::exception&) {
        return def_value;
    }
}
