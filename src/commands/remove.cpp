/*
	cross-pkg, source based package manager
	Copyright (C) 2026 Het Best

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.
 */

#include "remove.hpp"

#include <filesystem>
#include <string>
#include <vector>
#include <cstdlib>
#include <algorithm>
#include <fstream>
#include <iostream>

#include "search.hpp"
#include "build.hpp"
#include "../cmd.hpp"
#include "../defines.hpp"
#include "../messages.hpp"



bool c_remove(const std::vector<std::string> &targets, const std::vector<std::pair<char, std::string>> &flags)
{
	// Checking flags
	bool autoyes = false;
	bool force = false;
	bool verbose = false;

	for (const auto& [flag, arg] : flags)
	{
		switch (flag)
		{
			case 'y':
				autoyes = true;
				std::cout << FLAG_PREFIX << "Autoyes is enabled" << WHITE_COL << "\n";
				break;
			case 'f':
				force = true;
				std::cout << FLAG_PREFIX << "Force removing is enabled" << WHITE_COL << "\n";
				break;
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
		const std::string& rm_cmd = SU_CMD + " rm -f" + std::string((verbose) ? "v" : "");


		if (!std::filesystem::exists(target_path))
		{
			print_msg(MSG_PKG_NOT_INSTALL, target_name);
			continue;
		}
		
		std::optional<pkg_info> info = get_pkg_info(target_path + "/config.crs");
		if (!info.has_value())
			continue;


		// Checking dependents
		if (!force)
		{
			print_msg(MSG_PKG_CHECK_DEL, target_name);

			if (std::string depends_str = get_pkg_dependents(info.value()); depends_str != "")
			{
				depends_str.erase(depends_str.length() - 2);
				print_msg(MSG_PKG_HAS_DEPENDS, target_name, depends_str);
				return false;
			}
		}


		// Running before remove script
		if (!run_build("before-remove", target_path, info->bef_remove, false, autoyes))
		{
			print_msg(MSG_PKG_NOT_REMOVED, target_name);
			return false;
		}


		print_msg(MSG_PKG_REMOVING, target_name);


		// Manifest
		std::ifstream file(INSTALL_PATH + target_name + "/manifest");
		if (!file.is_open())
			return false;

		std::string line;
		while(getline(file, line))
		{
			if (!exec_cmd(rm_cmd + " '" + line + "'"))
			{
				print_msg(MSG_PKG_NOT_REMOVED, target_name);
				return false;
			}
		}
		file.close();

		if (!exec_cmd(rm_cmd + "r " + target_path))
		{
			print_msg(MSG_PKG_NOT_REMOVED, target_name);
			return false;
		}

		print_msg(MSG_PKG_REMOVED, target_name);


		// Running after remove script
		run_build("after-remove", target_path, info->aft_remove, false, autoyes);
	}


	return true;
}
