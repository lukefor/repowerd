/*
 * Copyright © 2016 Canonical Ltd.
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
 * Authored by: Alexandros Frantzis <alexandros.frantzis@canonical.com>
 * Modified by: Adam Boardman <adamboardman@gmail.com>
 */

#pragma once

#include "src/core/lock.h"

#include <gmock/gmock.h>

namespace repowerd
{
namespace test
{

class FakeLock : public Lock
{
public:
    void start_processing() override;

    HandlerRegistration register_lock_handler(LockHandler const& handler) override;

    void active();
    void inactive();

    struct Mock
    {
        MOCK_METHOD0(start_processing, void());
        MOCK_METHOD1(register_lock_handler, void(LockHandler const&));
        MOCK_METHOD0(unregister_lock_handler, void());
    };
    testing::NiceMock<Mock> mock;

private:
    LockHandler handler;
};

}
}
