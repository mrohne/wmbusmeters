/*
 Copyright (C) 2020 Fredrik Öhrström (gpl-3.0-or-later)

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

#include "threads.h"

#include <unistd.h>
#include <sys/resource.h>
#include <stdio.h>

using namespace std;

Semaphore::Semaphore(const char *name)
    : name_(name)
{
}

Semaphore::~Semaphore()
{
}

bool Semaphore::wait()
{
    trace("[WAITING] %s\n", name_);

    pthread_mutex_lock(&mutex_);
    struct timespec wait_until;
    clock_gettime(CLOCK_REALTIME, &wait_until);
    wait_until.tv_sec += 5;

    int rc = 0;
    for (;;)
    {
        rc = pthread_cond_timedwait(&condition_, &mutex_, &wait_until);
        if (!rc) break;
        if (rc == EINTR) continue;
        if (rc == ETIMEDOUT) break;
        error("(thread) pthread cond timedwait ERROR %d\n", rc);
    }

    pthread_mutex_unlock(&mutex_);

    trace("[WAITED] %s %s\n", name_, (rc==ETIMEDOUT)?"TIMEOUT":"OK");

    // Return true if proper wait.
    // Return false if timeout!!!!
    return rc != ETIMEDOUT;
}

void Semaphore::notify()
{
    trace("[NOTIFY] %s\n", name_);
    int rc = pthread_cond_signal(&condition_);
    if (rc)
    {
        error("(thread) pthread cond signal ERROR\n");
    }
}

size_t getPeakRSS()
{
  return 0;
}

size_t getCurrentRSS()
{
  return 0;
}
