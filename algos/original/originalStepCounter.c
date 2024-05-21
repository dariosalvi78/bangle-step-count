/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 *  Copyright (C) 2021 Gordon Williams <gw@pur3.co.uk>
 *
 * ----------------------------------------------------------------------------
 * Simple (original) Step Counter
 * ----------------------------------------------------------------------------
 */
#include <stdbool.h>

bool origStepWasLow;

/// How low must acceleration magnitude squared get before we consider the next rise a step?
int origStepCounterThresholdLow = (8192 - 80) * (8192 - 80);
/// How high must acceleration magnitude squared get before we consider it a step?
int origStepCounterThresholdHigh = (8192 + 80) * (8192 + 80);
int origStepCounter = 0;

/// Initialise step counting
void original_stepcount_init()
{
    origStepCounter = 0;
    origStepWasLow = 0;
}

/* Registers a new data point for step counting. Data is expected
 * as 12.5Hz, 8192=1g, and accMagSquared = x*x + y*y + z*z
 *
 * Returns the number of steps counted for this accel interval
 */
int original_stepcount(int accMagSquared)
{
    if (accMagSquared < origStepCounterThresholdLow)
        origStepWasLow = true;
    else if ((accMagSquared > origStepCounterThresholdHigh) && origStepWasLow)
    {
        origStepWasLow = false;
        origStepCounter++;
    }

    return origStepCounter;
}
