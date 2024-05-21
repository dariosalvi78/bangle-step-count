/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * ----------------------------------------------------------------------------
 * Dummy Step Counter
 * ----------------------------------------------------------------------------
 */

#ifndef ORIGINAL_STEP_COUNTER
#define ORIGINAL_STEP_COUNTER

// Initialise step counting
void original_stepcount_init();

/** Sets new thresholds for the algorithm
 *
 */
void original_stepcount_setThresholds(int threLow, int threHigh);

/* Registers a new data point for step counting. Data is expected
 * as 12.5Hz, 8192=1g, and accMagSquared = x*x + y*y + z*z
 *
 * Returns the number of steps counted for this accel interval
 */
int original_stepcount(int accMagSquared);

#endif