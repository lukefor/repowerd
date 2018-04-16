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

#include "x11_lock.h"
#include "event_loop_handler_registration.h"

namespace
{
    auto const null_handler = [](repowerd::LockState){};
    char const* const x11_lock_bus_name = "org.thinkglobally.Gemian.Lock";
    char const* const x11_lock_object_path = "/org/thinkglobally/Gemian/Lock";
    char const* const x11_lock_interface_name = "org.thinkglobally.Gemian.Lock";
    char const* const log_tag = "X11Lock";

}

repowerd::X11Lock::X11Lock(
        std::shared_ptr<Log> const& log,
        std::string const& dbus_bus_address)
        : log{log},
          dbus_connection{dbus_bus_address},
          dbus_event_loop{"Lock"},
          lock_handler{null_handler}
{
}

void repowerd::X11Lock::start_processing()
{
    dbus_signal_handler_registration = dbus_event_loop.register_signal_handler(
        dbus_connection,
        x11_lock_bus_name,
        x11_lock_interface_name,
        nullptr,
        x11_lock_object_path,
        [this] (
            GDBusConnection* connection,
            gchar const* sender,
            gchar const* object_path,
            gchar const* interface_name,
            gchar const* signal_name,
            GVariant* parameters)
        {
            handle_dbus_signal(
                connection, sender, object_path, interface_name,
                signal_name, parameters);
        });
}

repowerd::HandlerRegistration repowerd::X11Lock::register_lock_handler(
    LockHandler const& handler)
{
    return EventLoopHandlerRegistration{
        dbus_event_loop,
        [this, &handler] { this->lock_handler = handler; },
        [this] { this->lock_handler = null_handler; }};
}

void repowerd::X11Lock::handle_dbus_signal(
    GDBusConnection* /*connection*/,
    gchar const* /*sender*/,
    gchar const* /*object_path*/,
    gchar const* /*interface_name*/,
    gchar const* signal_name_cstr,
    GVariant* /*parameters*/)
{
    std::string const signal_name{signal_name_cstr ? signal_name_cstr : ""};

    if (signal_name == "Active")
        lock_handler(repowerd::LockState::active);
    else if (signal_name == "Inactive")
        lock_handler(repowerd::LockState::inactive);
}
