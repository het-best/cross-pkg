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

#include "own.hpp"

#include <iostream>

#include "search.hpp"
#include "../defines.hpp"
#include "../messages.hpp"



bool c_own(const std::string& target)
{
	// Checking if file exists
	if (!std::filesystem::exists(target))
	{
		print_msg(MSG_NO_FILE, target);
		return false;
	}

	const std::string absolute_path = std::filesystem::canonical(target).string();

	std::string owned_by = "None";


	// Checling if explicitely owned by someone
	if (!std::filesystem::exists(ALT_PATH + absolute_path))
	{
		std::ifstream alt_file(ALT_PATH + absolute_path);
		getline(alt_file, owned_by);
		alt_file.close();
	}


	// Checking manifest
	if (owned_by == "None")
	{
		for (const std::filesystem::path entry : std::filesystem::directory_iterator(INSTALL_PATH))
		{
			// Checking
			std::ifstream man_file(entry / "manifest");
			std::string line;

			while (getline(man_file, line) && owned_by == "None")
			{
				if (line == absolute_path)
					owned_by = entry.filename().string();
			}
			man_file.close();
		}
	}

	print_msg(MSG_LIST_OWN, owned_by);

	return true;
}
