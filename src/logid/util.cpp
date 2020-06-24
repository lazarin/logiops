/*
 * Copyright 2019-2020 PixlOne
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <cstdio>
#include <string>
#include <cstring>
#include <cstdarg>
#include <cmath>
#include <algorithm>
#include <stdexcept>

#include "util.h"

using namespace logid;

/*
Direction logid::getDirection(int x, int y)
{
    if(x == 0 && y == 0) return Direction::None;

    double angle;

    if(x == 0 && y > 0) angle = 90; // Y+
    else if(x == 0 && y < 0) angle = 270; // Y-
    else if(x > 0 && y == 0) angle = 0; // X+
    else if(x < 0 && y == 0) angle = 180; // X+
    else
    {
        angle = fabs(atan((double)y/(double)x) * 180.0 / M_PI);

        if(x < 0 && y > 0) angle = 180.0 - angle; //Q2
        else if(x < 0 && y < 0) angle += 180; // Q3
        else if(x > 0 && y < 0) angle = 360.0 - angle; // Q4
    }

    if(315 < angle || angle <= 45) return Direction::Right;
    else if(45 < angle && angle <= 135) return Direction::Down;
    else if(135 < angle && angle <= 225) return Direction::Left;
    else if(225 < angle && angle <= 315) return Direction::Up;

    return Direction::None;
}

Direction logid::stringToDirection(std::string s)
{
    const char* original_str = s.c_str();
    std::transform(s.begin(), s.end(), s.begin(), ::tolower);

    if(s == "none") return Direction::None;
    if(s == "up") return Direction::Up;
    if(s == "down") return Direction::Down;
    if(s == "left") return Direction::Left;
    if(s == "right") return Direction::Right;

    s = original_str;

    throw std::invalid_argument(s + " is an invalid direction.");
}

GestureMode logid::stringToGestureMode(std::string s)
{
    const char* original_str = s.c_str();
    std::transform(s.begin(), s.end(), s.begin(), ::tolower);

    if(s == "nopress") return GestureMode::NoPress;
    if(s == "onrelease") return GestureMode::OnRelease;
    if(s == "onfewpixels") return GestureMode::OnFewPixels;
    if(s == "axis") return GestureMode::Axis;

    s = original_str;

    log_printf(INFO, "%s is an invalid gesture mode. Defaulting to OnRelease", original_str);


    return GestureMode::OnRelease;
}

Action logid::stringToAction(std::string s)
{
    std::string original_str = s;
    std::transform(s.begin(), s.end(), s.begin(), ::tolower);

    if(s == "none") return Action::None;
    if(s == "keypress") return Action::Keypress;
    if(s == "gestures") return Action::Gestures;
    if(s == "togglesmartshift") return Action::ToggleSmartshift;
    if(s == "togglehiresscroll") return Action::ToggleHiresScroll;
    if(s == "cycledpi") return Action::CycleDPI;
    if(s == "changedpi") return Action::ChangeDPI;

    throw std::invalid_argument(original_str + " is an invalid action.");
}

 */

LogLevel logid::stringToLogLevel(std::string s)
{
    std::string original_str = s;
    std::transform(s.begin(), s.end(), s.begin(), ::tolower);

    if(s == "rawreport") return RAWREPORT;
    if(s == "debug") return DEBUG;
    if(s == "info") return INFO;
    if(s == "warn" || s == "warning") return WARN;
    if(s == "error") return ERROR;

    throw std::invalid_argument(original_str + " is an invalid log level.");
}