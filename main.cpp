#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

#include "flags.hpp"
#include "utils.hpp"
#include "Url.hpp"

const std::string VERSION {"1.0.1"};

int main(int argc, char **argv) {
    // Parse flags if provided
    std::vector<Option> options;
    if (argc > 1)
    {
        options = parse_flags(argc, argv);
    }

    std::vector<std::string> urls {};
    std::string url {};

    bool url_file_provided, regex_mode {false};
    for (const Option &option: options)
    {
        if (option.flag.short_name == "-h")
        {
            print_usage();
            return 0;
        }

        if (option.flag.short_name == "-V")
        {
            print_version(VERSION);
            return 0;
        }

        if (option.flag.short_name == "-u")
        {
            url_file_provided = true;
            load_urls_from_file(urls, option.value);
        }

        regex_mode = option.flag.short_name == "-r";
    }

    if (!url_file_provided)
    {
        while (getline(std::cin, url))
        {
            urls.push_back(url);
        }
    }

    std::unordered_map<std::string, bool> deduped_url_keys;
    std::vector<Url> deduped_urls {};
    for (const std::string &u: urls)
    {
        Url parsed_url(u, regex_mode);

        // Decode URLs before assessing
        bool was_encoded {false};
        if (Url::is_encoded(parsed_url.get_url_string()))
        {
            parsed_url.set_url_string(parsed_url.decode());
            was_encoded = true;
        }

        std::string url_key {parsed_url.get_url_key()};
        if (deduped_url_keys.find(url_key) != deduped_url_keys.end())
        {
            continue;
        }

        deduped_url_keys.insert(std::make_pair(url_key, true));

        // Re-encode back to original value after dupe check
        if (was_encoded)
        {
            parsed_url.set_url_string(parsed_url.encode());
        }

        deduped_urls.push_back(parsed_url);
    }

    for (const Url &u: deduped_urls)
    {
        std::cout << u.get_url_string() << std::endl;
    }

    return 0;
}
