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

#include "light_sensor.h"
#include "event_loop.h"

//#include <ubuntu/application/sensors/light.h>
typedef void UASLightEvent;
typedef void UASensorsLight;

namespace repowerd
{

class UbuntuLightSensor : public LightSensor
{
public:
    UbuntuLightSensor();

    HandlerRegistration register_light_handler(LightHandler const& handler) override;

    void enable_light_events() override;
    void disable_light_events() override;

private:
    static void static_sensor_reading_callback(UASLightEvent* event, void* context);
    void handle_light_event(double light_value);

    UASensorsLight* const sensor;
    EventLoop event_loop;
    LightHandler handler;
    bool enabled;

    void *ua_sensors_light_new();

    void ua_sensors_light_set_reading_cb(void *const pVoid, void (*callback)(void *, void *), UbuntuLightSensor *pSensor);

    void ua_sensors_light_enable(void *const pVoid);

    void ua_sensors_light_disable(void *const pVoid);

    static void uas_light_event_get_light(void *pVoid, float *pDouble);
};

}
