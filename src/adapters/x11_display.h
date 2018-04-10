//
// Created by adam on 10/04/18.
//

#pragma once

#include "src/core/display_power_control.h"
#include "src/core/display_information.h"
#include "src/core/handler_registration.h"
#include "src/core/log.h"

#include "dbus_connection_handle.h"
#include "dbus_event_loop.h"

#include <X11/Xlib.h>
#include <memory>
#include <atomic>

namespace repowerd
{
    class Log;

    class X11Display : public DisplayPowerControl, public DisplayInformation
    {
    public:
        X11Display(
                std::shared_ptr<Log> const& log,
                std::string const& dbus_bus_address);

        virtual ~X11Display();

        // From DisplayPowerControl
        void turn_on(DisplayPowerControlFilter filter) override;
        void turn_off(DisplayPowerControlFilter filter) override;

        // From DisplayInformation
        bool has_active_external_displays() override;

    private:
        void handle_dbus_signal(
                GDBusConnection* connection,
                gchar const* sender,
                gchar const* object_path,
                gchar const* interface_name,
                gchar const* signal_name,
                GVariant* parameters);
        void dbus_query_active_outputs();
        void dbus_PropertiesChanged(GVariantIter* properties_iter);
        void dbus_ActiveOutputs(int32_t internal, int32_t external);

        Display *dpy;
        std::shared_ptr<Log> const log;
        DBusConnectionHandle dbus_connection;
        DBusEventLoop dbus_event_loop;
        HandlerRegistration dbus_signal_handler_registration;
        std::atomic<bool> has_active_external_displays_;
    };

}
