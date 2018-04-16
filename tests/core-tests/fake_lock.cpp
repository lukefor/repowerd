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

#include "fake_lock.h"

namespace rt = repowerd::test;

void rt::FakeLock::start_processing()
{
    mock.start_processing();
}

repowerd::HandlerRegistration rt::FakeLock::register_lock_handler(
    LockHandler const& handler)
{
    mock.register_lock_handler(handler);
    this->handler = handler;
    return HandlerRegistration{
        [this]
        {
            mock.unregister_lock_handler();
            this->handler = [](LockState){};
        }};
}

void rt::FakeLock::active()
{
    handler(LockState::active);
}

void rt::FakeLock::inactive()
{
    handler(LockState::inactive);
}
