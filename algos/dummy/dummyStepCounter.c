/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * ----------------------------------------------------------------------------
 * Dummy Step Counter
 * ----------------------------------------------------------------------------
 */
#include <stdint.h>

long dummy_samples_counter = 0;
const float steps_per_sec = 1.5;

/// Initialise step counting
void dummy_stepcount_init()
{
    dummy_samples_counter = 0;
}

/* Registers a new data point for step counting. Data is expected
 * as 12.5Hz, 8192=1g, and accMagSquared = x*x + y*y + z*z
 *
 * Returns the number of steps counted for this accel interval
 */
int dummy_stepcount_new(int accMagSquared)
{
    // keeps track of how many samples have been taken -> we can derive time passed
    dummy_samples_counter++;

    if ((dummy_samples_counter % (int)(steps_per_sec * 12.5)) == 0)
        return 1;
    else
        return 0;
}
