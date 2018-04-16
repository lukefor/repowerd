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

#include "dbus_bus.h"
#include "dbus_client.h"
#include "src/adapters/x11_lock.h"

#include "wait_condition.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <chrono>
#include <tests/common/fake_log.h>
#include <tests/common/fake_shared.h>

namespace rt = repowerd::test;
using namespace std::chrono_literals;

namespace
{

struct X11LockDBusClient : rt::DBusClient
{
    X11LockDBusClient(std::string const& dbus_address)
        : rt::DBusClient{
            dbus_address,
            "org.thinkglobally.Gemian.Lock",
            "/org/thinkglobally/Gemian/Lock"}
    {
        connection.request_name("org.thinkglobally.Gemian.Lock");
    }

    void emit_lock_active()
    {
        emit_signal("org.thinkglobally.Gemian.Lock", "Active", nullptr);
    }

    void emit_lock_inactive()
    {
        emit_signal("org.thinkglobally.Gemian.Lock", "Inactive", nullptr);
    }
};

struct AX11Lock : testing::Test
{
    AX11Lock()
    {
        registrations.push_back(
            x11_lock.register_lock_handler(
                [this] (repowerd::LockState state)
                {
                    mock_handlers.lock(state);
                }));
        x11_lock.start_processing();
    }

    struct MockHandlers
    {
        MOCK_METHOD1(lock, void(repowerd::LockState));
    };
    testing::NiceMock<MockHandlers> mock_handlers;

    rt::DBusBus bus;
    rt::FakeLog fake_log;
    repowerd::X11Lock x11_lock{rt::fake_shared(fake_log), bus.address()};
    X11LockDBusClient client{bus.address()};
    std::vector<repowerd::HandlerRegistration> registrations;

    std::chrono::seconds const default_timeout{3};
};

}

TEST_F(AX11Lock, calls_handler_for_lock_active_signal)
{
    rt::WaitCondition request_processed;

    EXPECT_CALL(mock_handlers, lock(repowerd::LockState::active))
        .WillOnce(WakeUp(&request_processed));

    client.emit_lock_active();

    request_processed.wait_for(default_timeout);
    EXPECT_TRUE(request_processed.woken());
}

TEST_F(AX11Lock, calls_handler_for_lock_inactive_signal)
{
    rt::WaitCondition request_processed;

    EXPECT_CALL(mock_handlers, lock(repowerd::LockState::inactive))
        .WillOnce(WakeUp(&request_processed));

    client.emit_lock_inactive();

    request_processed.wait_for(default_timeout);
    EXPECT_TRUE(request_processed.woken());
}

TEST_F(AX11Lock, does_not_calls_unregistered_handlers)
{
    using namespace testing;

    registrations.clear();

    EXPECT_CALL(mock_handlers, lock(_)).Times(0);

    client.emit_lock_active();
    client.emit_lock_inactive();

    // Give some time for dbus signals to be delivered
    std::this_thread::sleep_for(100ms);
}
