#include "list.hpp"

#include <filesystem>
#include <iostream>

#include "search.hpp"
#include "../defines.hpp"
#include "../messages.hpp"



void c_list()
{
    uint installed_count = 0;
    uint max_length = 0;

    for (const std::filesystem::path path : std::filesystem::directory_iterator(INSTALL_PATH))
    {
        max_length = std::max(max_length, static_cast<uint>(path.filename().string().length()));
    }

    for (const std::filesystem::path path : std::filesystem::directory_iterator(INSTALL_PATH))
    {
        std::optional<pkg_info> info = get_pkg_info(path.string() + "/config.crs", false);
        if (!info.has_value())
            continue;

        std::cout << path.filename().string() << std::string(max_length - path.filename().string().length() + 1, ' ') << CYAN_COL << info->version << WHITE_COL << "\n";
        installed_count++;
    }

    if (installed_count == 0)
    {
        print_msg(MSG_NO_INSTALLED);
        return;
    }

    print_msg(MSG_TOTAL_INSTALLED, std::to_string(installed_count));
}
