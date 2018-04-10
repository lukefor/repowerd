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

#include "x11_display.h"
#include "scoped_g_error.h"

#include <gio/gio.h>
#include <X11/extensions/dpms.h>

namespace
{
    char const* const log_tag = "X11Display";

    std::string filter_to_str(repowerd::DisplayPowerControlFilter filter)
    {
        std::string filter_str;

        if (filter == repowerd::DisplayPowerControlFilter::all)
            filter_str = "all";
        else if (filter == repowerd::DisplayPowerControlFilter::internal)
            filter_str = "internal";
        else if (filter == repowerd::DisplayPowerControlFilter::external)
            filter_str = "external";
        else
            filter_str = "(unknown)";

        return filter_str;
    }

}

repowerd::X11Display::X11Display(
        std::shared_ptr<Log> const& log,
        std::string const& dbus_bus_address)
        : log{log},
          dbus_connection{dbus_bus_address},
          dbus_event_loop{"Display"},
          has_active_external_displays_{false}
{
    dpy = XOpenDisplay(getenv("DISPLAY"));
    log->log(log_tag, "X11Display %p", (void*)dpy);

//    dbus_signal_handler_registration = dbus_event_loop.register_signal_handler(
//            dbus_connection,
//            unity_display_bus_name,
//            "org.freedesktop.DBus.Properties",
//            "PropertiesChanged",
//            unity_display_object_path,
//            [this] (
//                    GDBusConnection* connection,
//                    gchar const* sender,
//                    gchar const* object_path,
//                    gchar const* interface_name,
//                    gchar const* signal_name,
//                    GVariant* parameters)
//            {
//                handle_dbus_signal(
//                        connection, sender, object_path, interface_name,
//                        signal_name, parameters);
//            });

    dbus_event_loop.enqueue([this] { dbus_query_active_outputs(); }).get();
}

void repowerd::X11Display::turn_on(DisplayPowerControlFilter filter)
{
    auto const filter_str = filter_to_str(filter);

    log->log(log_tag, "turn_on(%s)", filter_str.c_str());

    Status status = DPMSForceLevel(dpy, DPMSModeOn);

    log->log(log_tag, "turned_on(%s) - %d", filter_str.c_str(), status);
}

void repowerd::X11Display::turn_off(DisplayPowerControlFilter filter)
{
    auto const filter_str = filter_to_str(filter);

    log->log(log_tag, "turn_off(%s)", filter_str.c_str());

    Status status = DPMSForceLevel(dpy, DPMSModeOff);

    log->log(log_tag, "turned_off(%s) - %d", filter_str.c_str(), status);
}

bool repowerd::X11Display::has_active_external_displays()
{
    return has_active_external_displays_;
}

void repowerd::X11Display::handle_dbus_signal(
        GDBusConnection* /*connection*/,
        gchar const* /*sender*/,
        gchar const* /*object_path*/,
        gchar const* /*interface_name*/,
        gchar const* signal_name_cstr,
        GVariant* parameters)
{
    std::string const signal_name{signal_name_cstr ? signal_name_cstr : ""};

    if (signal_name == "PropertiesChanged")
    {
        char const* properties_interface_cstr{""};
        GVariantIter* properties_iter{nullptr};
        g_variant_get(parameters, "(&sa{sv}as)",
                      &properties_interface_cstr, &properties_iter, nullptr);

        std::string const properties_interface{properties_interface_cstr};

        log->log(log_tag, "handle_dbus_signal PropertiesChanged (%s)", properties_interface.c_str());

//        if (properties_interface == unity_display_interface_name)
//            dbus_PropertiesChanged(properties_iter);

        g_variant_iter_free(properties_iter);
    }
}

void repowerd::X11Display::dbus_PropertiesChanged(
        GVariantIter* properties_iter)
{
    char const* key_cstr{""};
    GVariant* value{nullptr};

    while (g_variant_iter_next(properties_iter, "{&sv}", &key_cstr, &value))
    {
        auto const key_str = std::string{key_cstr ? key_cstr : ""};

        if (key_str == "ActiveOutputs")
        {
            gint32 internal{0};
            gint32 external{0};

            g_variant_get(value, "(ii)", &internal, &external);

            dbus_ActiveOutputs(internal, external);
        }

        g_variant_unref(value);
    }
}

void repowerd::X11Display::dbus_ActiveOutputs(
        int32_t internal, int32_t external)
{
    log->log(log_tag, "dbus_ActiveOutputs(internal=%d, external=%d)", internal, external);

    has_active_external_displays_ = (external > 0);
}

void repowerd::X11Display::dbus_query_active_outputs()
{
    log->log(log_tag, "dbus_query_active_outputs()");

//    int constexpr timeout_default = -1;
//    auto constexpr null_cancellable = nullptr;
//    ScopedGError error;

    auto const result = false;
//            g_dbus_connection_call_sync(
//            dbus_connection,
//            unity_display_bus_name,
//            unity_display_object_path,
//            "org.freedesktop.DBus.Properties",
//            "Get",
//            g_variant_new("(ss)", unity_display_interface_name, "ActiveOutputs"),
//            G_VARIANT_TYPE("(v)"),
//            G_DBUS_CALL_FLAGS_NONE,
//            timeout_default,
//            null_cancellable,
//            error);

    if (!result)
    {
        log->log(log_tag, "dbus_get_active_outputs() failed to get ActiveOutputs: %s",
                 "");
//                 error.message_str().c_str());
        return;
    }

//    GVariant* property_variant{nullptr};
//    g_variant_get(result, "(v)", &property_variant);
//
//    gint32 internal{0};
//    gint32 external{0};
//
//    g_variant_get(property_variant, "(ii)", &internal, &external);
//
//    dbus_ActiveOutputs(internal, external);
//
//    g_variant_unref(property_variant);
//    g_variant_unref(result);
}

repowerd::X11Display::~X11Display() {
    XCloseDisplay(dpy);
}
