/*
 * Copyright © 2018 Gemian
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 3,
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authored by: Adam Boardman <adamboardman@gmail.com>
 */

#include "fake_exec.h"

#include <cstdarg>
#include <cstdlib>

namespace rt = repowerd::test;

namespace
{

    bool get_log_to_console()
    {
        auto const test_log_cstr = getenv("REPOWERD_TEST_LOG");
        auto const test_log = test_log_cstr ? std::string{test_log_cstr} : "";

        return test_log == "console";
    }

}

rt::FakeExec::FakeExec()
        : log_to_console{get_log_to_console()}
{
}

int rt::FakeExec::exec(char const* string)
{
    std::lock_guard<std::mutex> lock{contents_mutex};
    contents.push_back(string);

    if (log_to_console)
        printf("%s", string);

    return 0;
}

bool rt::FakeExec::contains_line(std::vector<std::string> const& words)
{
    std::lock_guard<std::mutex> lock{contents_mutex};

    for (auto const& line : contents)
    {
        bool found = true;

        for (auto const& word : words)
        {
            if (line.find(word) == std::string::npos)
            {
                found = false;
                break;
            }
        }

        if (found)
            return true;
    }

    return false;
}