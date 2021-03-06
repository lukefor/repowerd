/*
 * Copyright © 2018 Gemian.
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

#pragma once

namespace repowerd
{

    class Exec
    {
    public:
        virtual ~Exec() = default;

        virtual int exec(char const *string) = 0;

    protected:
        Exec() = default;
        Exec(Exec const&) = delete;
        Exec& operator=(Exec const&) = delete;
    };

}
