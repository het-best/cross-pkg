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

#include "download.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>

#ifdef WITH_LIBCURL
#include <curl/curl.h>
#endif

#include "search.hpp"
#include "../cmd.hpp"
#include "../messages.hpp"
#include "../split.hpp"


#ifdef WITH_LIBCURL
size_t curl_write_call(char *ptr, size_t size, size_t nmemb, void *stream)
{
    return fwrite(ptr, size, nmemb, (FILE *)stream);
}

struct curl_prog_str
{
    curl_off_t lastruntime;
    CURL *curl;
};
 
int curl_progress(void *p, curl_off_t down_total, curl_off_t down_now, curl_off_t up_total, curl_off_t up_now)
{
    // Setup
    const curl_prog_str* progress = static_cast<curl_prog_str*>(p);

    const uint progress_width = 40;
    const float fraction = (down_total > 0) ? (down_now / static_cast<float>(down_total)) : 0.0f;
    const uint filled = static_cast<uint>(progress_width * fraction);


    // Getting download speed
    curl_off_t speed = 0;
    curl_easy_getinfo(progress->curl, CURLINFO_SPEED_DOWNLOAD_T, &speed);

    std::string speed_str = std::to_string(speed) + "B/s ";

    if (speed >= 1024 * 1024 * 1024)
        speed_str = round_up_str(speed / 1024 / 1024 / 1024.0f, 2) + "GB/s ";
    else if (speed >= 1024 * 1024)
        speed_str = round_up_str(speed / 1024 / 1024.0f, 2) + "MB/s ";
    else if (speed >= 1024)
        speed_str = round_up_str(speed / 1024.0f, 2) + "KB/s ";


    // Getting estimated time
    const uint est_time = (down_total - down_now) / static_cast<float>(speed);
    std::string est_time_str = "--:--";

    if (est_time <= 3600)
    {
        std::string est_time_s = std::to_string(est_time % 60);
        
        est_time_str = std::to_string(est_time / 60) + ":";
        
        if (est_time_s.length() == 1)
            est_time_str += "0";
        
        est_time_str += est_time_s; 
    }
    else
        est_time_str = ">1:00:00";


    // Progress bar
    std::ostringstream bar;
    bar << '\r' << WHITE_COL << '[' << GREEN_COL;

    for (int i = 0; i < progress_width; i++)
        bar << (i == filled ? (">" + WHITE_COL) : (i < filled ? "=" : "-"));

    bar << WHITE_COL << "] " << std::setw(3) << (fraction == 1 ? GREEN_COL : "") << static_cast<int>(fraction * 100) << "% " << CYAN_COL << speed_str << ORANGE_COL << est_time_str << "\033[K";


    // Flushing
    std::cerr << bar.str() << std::flush;

    return 0;
}

bool url_download(const std::string& url, const std::string& path, const std::string& url_name)
{
    // Initializing
    CURL* curl = curl_easy_init();
    if (!curl)
    {
        print_msg(MSG_PKG_CURL_FAIL_SRC);
        return false;
    }


    // Opening file to output into
    FILE* file = fopen((path + url_name).c_str() , "wb");
    if (!file)
    {
        print_msg(MSG_PKG_FILE_FAIL_SRC);
        curl_easy_cleanup(curl);
        return false;
    }


    // Downloading params
    curl_prog_str progress(0, curl); 

    curl_slist* headers = NULL;
    headers = curl_slist_append(headers, "Accept: */*");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    curl_easy_setopt(curl, CURLOPT_USERAGENT, "curl/8.0.1");
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_write_call);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_FAILONERROR, 1L);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 15L);
    curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, curl_progress);
    curl_easy_setopt(curl, CURLOPT_XFERINFODATA, &progress);
    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);


    // Downloading
    const CURLcode result = curl_easy_perform(curl);
    curl_progress(&progress, 1, 1, 0, 0);
    std::cerr << "\n";
    
    if (result != CURLE_OK)
    {
        print_msg(MSG_PKG_CURL_DOWN_FAIL, curl_easy_strerror(result));

        long response_code;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
        print_msg(MSG_PKG_HTTPS_DOWN_FAIL, std::to_string(response_code));

    }

    fclose(file);
    curl_easy_cleanup(curl);


    return result == CURLE_OK;
}
#else
bool url_download(const std::string& url, const std::string& path, const std::string& url_name)
{
    if (DOWN_CMD == "curl")
        return exec_cmd("curl -L " + url + " -o " + path + url_name);
    else if (DOWN_CMD == "wget")
        return exec_cmd("wget " + url + " -P " + path);

    return false;
}
#endif


bool c_download(const std::vector<std::string> &targets, const std::vector<std::pair<char, std::string>> &flags, const bool hide_flags_msg)
{
    // Checking for arguments
    bool force_download = false;
    bool preserve_src = false;
    bool verbose = false;

    for (const auto& [flag, arg] : flags)
    {
        switch (flag)
        {
            case 'f':
                force_download = true;
                if (!hide_flags_msg)
                    std::cout << FLAG_PREFIX << "Force source download is enabled" << WHITE_COL << "\n";
                break;
            case 'u':
                preserve_src = true;
                if (!hide_flags_msg)
                    std::cout << FLAG_PREFIX << "Source preservation is enabled" << WHITE_COL << "\n";
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
        const std::string& target_path = get_target_path(target_name);
        const std::string& target_cache = CACHE_PATH + target_name + "/";

        if (target_path.empty())
            return false;


        // Checking cache
        if (verbose)
            print_msg(MSGV_PKG_CHECK_CACHE, target_name);

        if (!std::filesystem::exists(target_cache))
            std::filesystem::create_directory(target_cache);
        if (std::filesystem::exists(target_cache + "install"))
            std::filesystem::remove_all(target_cache + "install");
        std::filesystem::create_directory(target_cache + "install");

        if (verbose)
            exec_cmd("cp -rva " + target_path + "* " + target_cache);
        else
            exec_cmd("cp -ra " + target_path + "* " + target_cache);


        // Reading sources
        print_msg(MSG_PKG_READ_SRC, target_name);

        std::optional<pkg_info> info = get_pkg_info(target_path);
        if (!info.has_value())
            return false;


        // Downloading sources
        if (verbose)
            print_msg(MSGV_PKG_START_DOWN, target_name);

        if (preserve_src && std::filesystem::exists(target_cache + "source"))
            continue;
        if (!preserve_src && std::filesystem::exists(target_cache + "source"))
            std::filesystem::remove_all(target_cache + "source");
        if (!std::filesystem::exists(target_cache + "source"))
            std::filesystem::create_directory(target_cache + "source");


        for (const auto& [prefix, source, output] : info->sources)
        {
            std::string source_name = split(source, "/").back();

            if (prefix == SRC_URL)
            {
                // Checking
				const bool is_archive = source_name.ends_with(".gz") || source_name.ends_with(".bz") || source_name.ends_with(".tar") || source_name.ends_with(".xz");

                if (std::filesystem::exists(target_cache + source_name) && !force_download &&
					!(is_archive && !exec_cmd("tar -atf " + target_cache + source_name + " > /dev/null 2>&1")))
                {
					print_msg(MSG_PKG_ALR_DOWN, target_name, source);
                    continue;
                }
				
                print_msg(MSG_PKG_DOWN_SRC, target_name, source);


                // Downloading
                if (url_download(source, target_cache, source_name))
                    continue;


                // Checking mirrors
                const std::vector<std::string> splitted_source = split(source, "/");
                const std::string domain = splitted_source[2];
                const std::string no_domain_url(source.begin() + (splitted_source[0] + "//" + splitted_source[2]).length(), source.end());
                bool good_mirror = false;

                if (std::filesystem::exists(MIRRORS_PATH) && std::filesystem::exists(MIRRORS_PATH + domain))
                {
                    std::ifstream file(MIRRORS_PATH + domain);
                    std::string line;

                    while (getline(file, line) && !good_mirror)
                    {
                        const std::string url = splitted_source[0] + "//" + line + no_domain_url;
                        print_msg(MSG_PKG_DOWN_MIRROR, target_name, url);

                        if (url_download(url, target_cache, source_name))
                            good_mirror = true;
                    }
                }

                if (!good_mirror)
                {
                    print_msg(MSG_PKG_DOWN_FAIL, source);
                    return false;
                }
            }
            else if (prefix == SRC_GIT) 
            {
                std::string commit = "git-source";
                std::string git_source = source;

                if (source.find("@") != std::string::npos)
                {
                    const std::vector<std::string> splitted_source = split(source, "@");

                    if (splitted_source.size() > 2)
                        print_msg(MSG_PKG_GIT_COMMIT, target_name, source);

                    commit = splitted_source.back();
                    git_source = splitted_source.front();
                }


                // Checking
                if (std::filesystem::exists(target_cache + commit))
                {
                    if (!force_download)
                    {
                        print_msg(MSG_PKG_ALR_DOWN, target_name, source);
                        continue;
                    }

                    std::filesystem::remove_all(target_cache + commit);
                }


                print_msg(MSG_PKG_DOWN_SRC, target_name, source);

                if (!exec_cmd("git clone --recurse-submodules " + git_source + " " + target_cache + commit + "/" + output))
                {
                    print_msg(MSG_PKG_DOWN_FAIL, git_source);
                    return false;
                }

                if (commit != "git-source")
                    exec_cmd("(cd " + target_cache + commit + "/ " + output + " && git checkout " + commit + " " + target_cache + commit + "/" + output + ")");
            }
        }


        // Extracting sources
        print_msg(MSG_PKG_EXTR_SRC);

        for (const auto& [prefix, source, output] : info->sources)
        {
            const std::string source_name = split(source, "/").back();


            if (prefix == SRC_URL)
            {
                if (!exec_cmd("tar -atf " + target_cache + source_name + " > /dev/null 2>&1"))
                {
                    exec_cmd("cp " + target_cache + source_name + " " + target_cache + "source/" + output);
                    continue;
                }

                if (!std::filesystem::exists(target_cache + "source/" + output))
                    std::filesystem::create_directory(target_cache + "source/" + output);

                if (verbose)
                    exec_cmd("tar -xvf" + target_cache + source_name + " -C" + target_cache + "source/" + output + " --strip-components=1");
                else
                    exec_cmd("tar -xf" + target_cache + source_name + " -C" + target_cache + "source/" + output + " --strip-components=1");
            }
            else if (prefix == SRC_GIT)
            {
                std::string commit = "git-source";

                if (source.find("@") != std::string::npos)
                    commit = split(source, "@").back();
                
                exec_cmd("cp -a " + target_cache + commit + "/. " + target_cache + "source");
            }
        }
    }


    return true;
}
