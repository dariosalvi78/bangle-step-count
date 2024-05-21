/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * ----------------------------------------------------------------------------
 * Dummy Step Counter
 * ----------------------------------------------------------------------------
 */

/// Initialise step counting
void dummy_stepcount_init();

/* Registers a new data point for step counting. Data is expected
 * as 12.5Hz, 8192=1g, and accMagSquared = x*x + y*y + z*z
 *
 * Returns the number of steps counted for this accel interval
 */
int dummy_stepcount_new(int accMagSquared);
