/*
 * Copyright © 2016 Canonical Ltd.
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
 * Authored by: Alexandros Frantzis <alexandros.frantzis@canonical.com>
 */

#include "acceptance_test.h"

#include <gtest/gtest.h>

namespace rt = repowerd::test;
using namespace std::chrono_literals;

namespace
{

struct ALock : rt::AcceptanceTest
{
};

}

TEST_F(ALock, lock_inactive_display_turns_on)
{
    expect_display_turns_on();

    lock_inactive();
}

TEST_F(ALock, lock_active_display_turns_off)
{
    turn_on_display();

    expect_display_turns_off();

    lock_active();
}

TEST_F(ALock, lock_active_lid_closed_user_activity_display_stays_off)
{
    close_lid();
    lock_active();

    perform_user_activity_changing_power_state();
    perform_user_activity_extending_power_state();

    expect_no_display_power_change();
    expect_no_display_brightness_change();
}


TEST_F(ALock, lock_active_is_logged)
{
    lock_active();

    EXPECT_TRUE(log_contains_line({"handle_lock_active()"}));
}

TEST_F(ALock, lock_inactive_is_logged)
{
    lock_inactive();

    EXPECT_TRUE(log_contains_line({"handle_lock_inactive()"}));
}
