#include "version.hpp"

#include <iostream>

#include "update.hpp"
#include "../messages.hpp"
#include "../split.hpp"



void c_version(const std::vector<std::pair<char, std::string>> &flags)
{
    // Checking for arguments
    bool fetch_version = true;

    for (const auto& [flag, arg] : flags)
    {
        if (flag == 'f')
        {
            fetch_version = false;
            std::cout << FLAG_PREFIX << "Not fetching cross-pkg version is enabled" << WHITE_COL << "\n";
        }
        else
            print_msg(MSG_UNK_FLAG, std::to_string(flag));
    }


    print_msg(MSG_CROSS_VER);


    // Checking for updates
    if (!fetch_version)
        return;

    const auto [url, version] = get_version("https://github.com/het-best/cross-pkg/", CROSS_VERSION);

    if (version.empty())
        return;

    if (cross_stov(version) > cross_stov(CROSS_VERSION))
        print_msg(MSG_CROSS_UPD, version);
}
