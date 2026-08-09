#include "remove.hpp"

#include <filesystem>
#include <string>
#include <vector>
#include <cstdlib>
#include <algorithm>
#include <fstream>
#include <iostream>

#include "search.hpp"
#include "../cmd.hpp"
#include "../defines.hpp"
#include "../messages.hpp"



void c_remove(const std::vector<std::string> &targets, const std::vector<std::pair<char, std::string>> &flags)
{
    // Checking flags
    bool verbose = false;

    for (const auto& [flag, arg] : flags)
    {
        switch (flag)
        {
            case 'v':
                verbose = true;
                std::cout << FLAG_PREFIX << "Verbose output is enabled" << WHITE_COL << "\n";
                break;
            default:
                print_msg(MSG_UNK_FLAG, std::string(1, flag));
        }
    }


    for (uint i = 0; i < targets.size(); i++)
    {
        const std::string& target_name = targets[i];
        const std::string& target_path = INSTALL_PATH + target_name;


        if (!std::filesystem::exists(target_path))
        {
            print_msg(MSG_PKG_NOT_INSTALL, target_name);
            continue;
        }


        // Checking dependents
        print_msg(MSG_PKG_CHECK_DEL, target_name);
        std::string depends_str = "";

        for (const std::filesystem::path path : std::filesystem::directory_iterator(INSTALL_PATH))
        {
            std::optional<pkg_info> pkg_info = get_pkg_info(path.string() + "/config.crs", false);
            if (!pkg_info.has_value())
                continue;

            if (std::ranges::find(pkg_info->depends, target_name) != pkg_info->depends.end())
                depends_str += path.filename().string() + ", ";
        }

        if (depends_str != "")
        {
            depends_str.erase(depends_str.length() - 2);
            print_msg(MSG_PKG_HAS_DEPENDS, target_name, depends_str);
            return;
        }


        print_msg(MSG_PKG_REMOVING, target_name);


        // Manifest
        std::ifstream file(INSTALL_PATH + target_name + "/manifest");
        if (!file.is_open())
        {
            return;
        }

        std::string line;
        while(getline(file, line))
        {
            if (verbose)
                exec_cmd(SU_CMD + " rm -fv " + line);
            else
                exec_cmd(SU_CMD + " rm -f " + line);
        }
        file.close();


        if (verbose)
            exec_cmd(SU_CMD + " rm -rfv " + target_path);
        else
            exec_cmd(SU_CMD + " rm -rf " + target_path);

        if (!std::filesystem::exists(target_path))
            print_msg(MSG_PKG_REMOVED, target_name);
        else
        {
            print_msg(MSG_PKG_NOT_REMOVED, target_name);
            return;
        }
    }
}
