/*
 Copyright (C) 2017-2021 Fredrik Öhrström (gpl-3.0-or-later)

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include"bus.h"
#include"cmdline.h"
#include"config.h"
#include"meters.h"
#include"printer.h"
#include"rtlsdr.h"
#include"serial.h"
#include"shell.h"
#include"threads.h"
#include"util.h"
#include"wmbus.h"

#include <algorithm>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>
#include <set>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

using namespace std;

shared_ptr<BusManager> createBusManager(shared_ptr<SerialCommunicationManager> serial_manager,
                                        shared_ptr<MeterManager> meter_manager)
{
    return shared_ptr<BusManager>(new BusManager(serial_manager, meter_manager));
}

BusManager::BusManager(shared_ptr<SerialCommunicationManager> serial_manager,
                       shared_ptr<MeterManager> meter_manager)
    : serial_manager_(serial_manager),
        meter_manager_(meter_manager),
        bus_devices_mutex_("bus_devices_mutex"),
        bus_send_queue_mutex_("bus_send_queue_mutex"),
        printed_warning_(true)
{
}

BusDevice* BusManager::findBus(string bus_alias)
{
    return NULL;
}

