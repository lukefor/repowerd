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

#include "backlight.h"

#include "path.h"
#include "../core/exec.h"
//#include "timer.h"

#include <memory>
#include <string>

namespace repowerd
{
class Log;
class Filesystem;

class SysfsBacklight : public Backlight
{
public:
    SysfsBacklight(
        std::shared_ptr<Log> const& log,
        std::shared_ptr<Exec> const& exec,
        //std::shared_ptr<Timer> const& timer,
        std::shared_ptr<Filesystem> const& filesystem);

    void set_brightness(double) override;
    double get_brightness() override;

private:
    int absolute_brightness_for(double relative_brightness);

    std::shared_ptr<Log> const log;
    std::shared_ptr<Exec> const exec;
    std::shared_ptr<Filesystem> const filesystem;
    Path const sysfs_backlight_dir;
    Path const sysfs_brightness_file;
    int const max_brightness;
    double last_set_brightness;
	//AlarmId aal_alarm = 0;
};

}

