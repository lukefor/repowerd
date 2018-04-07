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

#pragma once

#include "src/core/proximity_sensor.h"
#include "event_loop.h"

#include <memory>
#include <mutex>
#include <condition_variable>
#include <vector>

//#include <ubuntu/application/sensors/proximity.h>
typedef void UASProximityEvent;
typedef void UASensorsProximity;

namespace repowerd
{

class DeviceQuirks;
class Log;

class UbuntuProximitySensor : public ProximitySensor
{
public:
    UbuntuProximitySensor(
        std::shared_ptr<Log> const& log,
        DeviceQuirks const& device_quirks);

    HandlerRegistration register_proximity_handler(
        ProximityHandler const& handler) override;
    ProximityState proximity_state() override;

    void enable_proximity_events() override;
    void disable_proximity_events() override;

    void emit_proximity_event(ProximityState state);

private:
    enum class EnablementMode{with_handler, without_handler};

    static void static_sensor_reading_callback(UASProximityEvent* event, void* context);
    void handle_proximity_event(ProximityState state);
    void enable_proximity_events_unqueued(EnablementMode mode);
    void disable_proximity_events_unqueued(EnablementMode mode);
    ProximityState wait_for_valid_state();
    void schedule_synthetic_initial_event();
    void invalidate_synthetic_initial_event();

    bool is_enabled();
    bool should_invoke_handler();

    std::shared_ptr<Log> const log;
    UASensorsProximity* const sensor;
    EventLoop event_loop;

    std::vector<EnablementMode> enablements;

    ProximityHandler handler;
    int synthetic_event_seqno;
    std::chrono::milliseconds const synthetic_event_delay;
    ProximityState const synthetic_event_state;

    std::mutex state_mutex;
    std::condition_variable state_cv;
    bool is_state_valid;
    ProximityState state;

    void *ua_sensors_proximity_new();

    void ua_sensors_proximity_enable(void *const pVoid);

    static int uas_proximity_event_get_distance(void *pVoid);

    void ua_sensors_proximity_disable(void *const pVoid);

    void ua_sensors_proximity_set_reading_cb(void *const pVoid, void (*callback)(void *, void *),
                                             UbuntuProximitySensor *pSensor);
};

}
