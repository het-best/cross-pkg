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

#include "install.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <ranges>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include "build.hpp"
#include "search.hpp"
#include "../cmd.hpp"
#include "../messages.hpp"



bool c_install(const std::vector<std::string> &targets, const std::vector<std::pair<char, std::string>> &flags, bool hide_flags_msg,
	std::pair<std::string, std::string> number)
{
	// Checking flags
	bool autoyes = false;
	bool verbose = false;
	std::string install_path = "";

	for (const auto& [flag, arg] : flags)
	{
		switch (flag)
		{
			case 'y':
				autoyes = true;
				std::cout << FLAG_PREFIX << "Autoyes is enabled" << WHITE_COL << "\n";
				break;
			case 'i':
				if (arg.empty())
					print_msg(MSG_NO_FLAG_ARG, std::string(1, flag));
				else
				{
					if (!std::filesystem::exists(arg))
					{
						if (!hide_flags_msg)
							std::cout << ERR_PREFIX << "Cannot install into " << BLUE_COL << arg << WHITE_COL << ", directory does not exists\n";
						continue;
					}

					if (!std::filesystem::is_directory(arg))
					{
						if (!hide_flags_msg)
							std::cout << ERR_PREFIX << "Cannot install into " << BLUE_COL << arg << WHITE_COL << ", not a directory\n";
						continue;
					}

					if (!hide_flags_msg)
						std::cout << FLAG_PREFIX << "Packages will be installed to: " << BLUE_COL << arg << WHITE_COL << "\n";
				}
				break;
			case 'v':
				verbose = true;
				if (!hide_flags_msg)
					std::cout << FLAG_PREFIX << "Verbose output is enabled" << WHITE_COL << "\n";
				break;
			default:
				print_msg(MSG_UNK_FLAG, std::string(1, flag));
		}
	}


	for (uint i = 0; i < targets.size(); i++)
	{
		const std::string& target_name = targets[i];
		const std::string& target_cache = std::string(CACHE_PATH) + target_name + "/";
		const std::string& target_ins_path = install_path + INSTALL_PATH + target_name + "/";


		// Checking cache
		if (verbose)
			print_msg(MSGV_PKG_CHECK_CACHE, target_name);

		if (!std::filesystem::exists(target_cache) || !std::filesystem::exists(target_cache + target_name + ".crs"))
		{
			print_msg(MSG_PKG_NO_CACHE, target_name);
			continue;
		}

		if (number.first.empty())
			print_msg(MSG_PKG_INSTALLING, target_name, std::to_string(i + 1) + "/" + std::to_string(targets.size()));
		else
			print_msg(MSG_PKG_INSTALLING, target_name, number.first + "/" + number.second);


		if (std::filesystem::is_empty(target_cache + "install"))
		{
			print_msg(MSG_PKG_NO_TO_INST);
			continue;
		}
	

		std::optional<pkg_info> info = get_pkg_info(get_target_path(target_name));
		if (!info.has_value())
			continue;

		
		// Reading installed manifests
		std::unordered_map<std::string, std::string> packages_index;

		for (const std::filesystem::path& inst_entry : std::filesystem::directory_iterator(INSTALL_PATH))
		{
			const std::string package_name = inst_entry.filename().string();
			if (package_name == target_name)
				continue;

			
			// Checking index
			if (packages_index.find(package_name) == packages_index.end())
			{
				// Reading manifest file
				std::ifstream man_file(inst_entry / "manifest");
				if (!man_file)
        			continue;
				
				std::string line;
				while (getline(man_file, line))
				{
					packages_index.insert({line, package_name});
				}
				man_file.close();
			}
		}


		// Checking for alternatives
		std::unordered_set<std::string> excluded_files;

		for (const std::filesystem::path& entry : std::filesystem::recursive_directory_iterator(target_cache + "install"))
		{
			if (is_directory(entry))
				continue;

			const std::string full_path = entry.string();
			const std::string relative_path = std::filesystem::path(install_path + std::string(full_path.begin() + (target_cache + "install").length(), full_path.end())).remove_filename().string();
			

			// Checking if file is owned by somebody
			auto package_index = packages_index.find(relative_path + entry.filename().string());
			if (package_index == packages_index.end())
				continue;


			// Checking alternatives
			std::filesystem::create_directories(ALT_PATH + relative_path);
			const std::string alt_path = ALT_PATH + relative_path + entry.filename().string();
			
			std::ifstream if_alt_file(alt_path);

			std::string line;
			getline(if_alt_file, line);

			if_alt_file.close();

			if (line == target_name)
				continue;


			// Asking user if he wants to replace file
			std::ofstream alt_file(alt_path);
			if (autoyes)
			{
				alt_file << target_name << "\n";
				alt_file.close();
				continue;
			}

			print_msg(MSG_PKG_ALT_REPLACE, relative_path + entry.filename().string(), package_index->second);
			print_msg(MSG_PKG_ALT_REPLACE_QW);

			std::string input;
			std::getline(std::cin, input);

			if (input == "n" || input == "N")
			{
				excluded_files.insert(relative_path + entry.filename().string());
				alt_file << package_index->second << "\n";
				break;
			}

			if (input == "yy" || input == "Yy" || input == "yY" || input == "YY")
				autoyes = true;
			
			alt_file << target_name << "\n";
			alt_file.close();
		}
		packages_index.clear();


		// Creating manifest
		print_msg(MSG_PKG_MANIFEST, target_name);
		
		if (std::filesystem::exists(target_cache + "manifest"))
			std::filesystem::remove(target_cache + "manifest");

		std::ofstream file(target_cache + "manifest");

		for (const std::filesystem::path entry : std::filesystem::recursive_directory_iterator(target_cache + "install"))
		{
			const std::string full_path = entry.string();
			const std::string relative_path(full_path.begin() + std::string(target_cache + "install").length(), full_path.end());
			
			if (full_path.find("DONT_STRIP") != std::string::npos)
				continue;

			file << relative_path << "\n";

			if (verbose)
				print_msg(MSGV_PKG_INST_FILE_FOUND, install_path + static_cast<std::string>(relative_path));
		}
		file.close();


		// Removing old package files
		if (verbose)
			print_msg(MSGV_PKG_REM_FILES, target_name);

		std::ifstream man_file(target_cache + "manifest");
		std::ifstream inst_man_file(target_ins_path + "manifest");
		
		if (man_file.is_open() && inst_man_file.is_open())
		{
			std::string line;

			std::stringstream buffer;
			buffer << man_file.rdbuf();

			const std::string buffer_str = buffer.str();

			while(getline(inst_man_file, line))
			{
				if (buffer_str.find(line) == std::string::npos)
					exec_cmd(SU_CMD + " rm" + (verbose ? " -v " : " " ) + install_path + line);
			}
		}
		man_file.close();
		inst_man_file.close();


		// Checking for already existing config files in /etc
		if (std::filesystem::exists(target_cache + "install/etc"))
		{
			if (std::filesystem::exists(target_cache + "tmp"))
				std::filesystem::remove_all(target_cache + "tmp");
			std::filesystem::create_directory(target_cache + "tmp");

			
			// Checking files
			const std::string install_etc = target_cache + "install/etc";

			for (const std::filesystem::path entry : std::filesystem::recursive_directory_iterator(install_etc))
			{
				const std::string full_path = entry.string();
				std::string relative_path = std::filesystem::path(install_path + std::string(full_path.begin() + install_etc.length(), full_path.end())).remove_filename().string();
				if (std::filesystem::is_directory(entry))
					continue;


				// Checking if file already exists
				const std::string target_tmp = target_cache + "tmp" + relative_path;

				if (!std::filesystem::exists(install_path + "/etc" + relative_path + entry.filename().string()))
					continue;


				// Moving existing file
				if (std::filesystem::exists(target_cache + "install/CROSS_ETC_OVERWRITE"))
				{
					print_msg(MSG_PKG_ETC_REPLACE, "/etc" + relative_path + entry.filename().string());
					print_msg(MSG_PKG_ALT_REPLACE_QW);
		
					std::string input;
					std::getline(std::cin, input);
					
					if (input == "n" || input == "N")
						goto move_etc_to_tmp;
	
					if (input == "yy" || input == "Yy" || input == "yY" || input == "YY")
						autoyes = true;


					// Determening if file is not already owned by target package
					const std::string overwrite_path = target_ins_path + "overwrite/etc" + relative_path;

					std::stringstream buffer;
					if (std::filesystem::exists(target_ins_path + "manifest"))
					{
						std::ifstream man_file(target_ins_path + "manifest");
						buffer << man_file.rdbuf();
						man_file.close();
					}

					if (!std::filesystem::exists(overwrite_path + entry.filename().string()) || buffer.str().find("/etc" + relative_path + entry.filename().string()) == std::string::npos)
					{
						exec_cmd(SU_CMD + " mkdir -p " + overwrite_path);
						exec_cmd(SU_CMD + " cp " + install_path + "/etc" + relative_path + entry.filename().string() + " " + overwrite_path + "/");
					}

					continue;
				}
move_etc_to_tmp:
				
				std::filesystem::create_directories(target_tmp);
				std::filesystem::rename(full_path, target_tmp + entry.filename().string());
			}
		}


		// Installing to INSTALL_PATH
		if (verbose)
			print_msg(MSGV_PKG_INST_FILES, target_name);

		if (!std::filesystem::exists(target_ins_path))
			exec_cmd(SU_CMD + " mkdir " + target_ins_path);
		else
			exec_cmd(SU_CMD + " rm " + target_ins_path + "config.crs " + target_ins_path + "manifest");

		exec_cmd(SU_CMD + " cp " + target_cache + target_name + ".crs " + target_ins_path + "config.crs");
		exec_cmd(SU_CMD + " cp " + target_cache  + "manifest " + target_ins_path);
		exec_cmd(SU_CMD + " chmod -R 755 " + target_ins_path);


		// Copying files
		const std::string cp_cmd  = SU_CMD + " cp -r" + ((verbose) ? "v" : "") + " --preserve=mode,timestamps --remove-destination ";

		for (const std::filesystem::path entry : std::filesystem::recursive_directory_iterator(target_cache + "install"))
		{
			const std::string full_path = entry.string();
			std::string relative_path = std::filesystem::path(install_path + std::string(full_path.begin() + std::string(target_cache + "install").length(), full_path.end())).remove_filename().string();
			relative_path.erase(relative_path.length() - 1);

			if (!is_symlink(std::filesystem::path(relative_path)) || excluded_files.contains(relative_path))
				continue;

			while (is_symlink(std::filesystem::path(relative_path)))
			{
				const std::string old_path = relative_path;

				relative_path = std::string(relative_path.begin(), relative_path.begin() + relative_path.find_last_of('/'));
				relative_path += "/" + read_symlink(std::filesystem::path(old_path)).string();
			}

			exec_cmd(cp_cmd + full_path + " " + relative_path + "/");
		}

		exec_cmd(cp_cmd + target_cache + "install/* " + install_path + "/ 2>/dev/null");
		if (std::filesystem::exists("/DONT_STRIP") || std::filesystem::exists("/CROSS_ETC_OVERWRITE"))
			exec_cmd(SU_CMD + " rm -rf /DONT_STRIP /CROSS_ETC_OVERWRITE 2>/dev/null");

		if (std::filesystem::exists(target_cache + "tmp"))
		{
			std::filesystem::copy(target_cache + "tmp", target_cache + "/install/etc", std::filesystem::copy_options::overwrite_existing | std::filesystem::copy_options::recursive);
			std::filesystem::remove_all(target_cache + "tmp");
		}


		if (number.first.empty())
			print_msg(MSG_PKG_INSTALLED, target_name, std::to_string(i + 1) + "/" + std::to_string(targets.size()));
		else
			print_msg(MSG_PKG_INSTALLED, target_name, number.first + "/" + number.second);


		// Compiling glib schemas
		if (std::filesystem::exists(target_cache + "install/usr/share/glib-2.0/schemas") || std::filesystem::exists(target_cache + "install/share/glib-2.0/schemas"))
		{
			print_msg(MSG_PKG_GLIB_COMPILE, target_name);
			exec_cmd(SU_CMD + " glib-compile-schemas /usr/share/glib-2.0/schemas/");
		}


		// After install script
		run_build("after-install", target_cache, info->aft_install, false, autoyes);
	}


	return true;
}
