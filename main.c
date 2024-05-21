#define _GNU_SOURCE
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

#define PATH "data/"

// all test data
/*
#define FILECOUNT 10
const char *FILES[FILECOUNT] = {
 "HughB-walk-1834.csv",
 "HughB-walk-2331.csv",
 "HughB-walk-2350.csv",
 "HughB-walk-6605.csv",

 "HughB-drive-36min-0.csv",
 "HughB-drive-29min-0.csv",
 "HughB-drive-18.csv",

 "HughB-work-0.csv",
 "Hughb-work-66.csv",
 "HughB-mixed-390.csv"
};

int EXPECTED_STEPS[FILECOUNT] = {1834,2331,2350,6605,  0,0,18,  0,66,390};
int HOWMUCH[FILECOUNT] = {5,5,5,5,  5,5,5,  5,5,5}; // how much do we care about these?
*/

#define FILECOUNT 13
const char *FILES[FILECOUNT] = {
    "HughB-walk-6605.csv",
    "HughB-walk-2350.csv",
    "HughB-walk-a3070-b3046.csv",
    "HughB-walk-a10021-b10248.csv",
    "HughB-drive-36min-0.csv",
    "HughB-drive-29min-0.csv",
    "HughB-drive-a3-b136.csv",
    "HughB-work-66.csv",
    "HughB-work-66.csv",
    "HughB-mixed-390.csv",
    "HughB-general-a260-b573.csv",
    "HughB-housework-a958-b2658.csv",
    "MrPloppy-stationary-0.csv"};

int EXPECTED_STEPS[FILECOUNT] = {6605, 2350, 3070, 10021, 0, 0, 3, 66, 66, 390, 260, 958, 0};
int HOWMUCH[FILECOUNT] = {1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 10}; // how much do we care about these?
/* Some files have more steps in than others, so we (probably) want to
put less weight on those than others. For instance:

* when driving you might detect 60 steps when you should have 0
* when walking you detect 6060 steps when you did 6000

Probably we care more about the accuracy driving (or about the
logs that are done over a longer time period)
*/

#define DEBUG 0
#define STEPCOUNT_CONFIGURABLE

#include "../Espruino/libs/misc/stepcount.c"
#include "./algos/dummy/dummyStepCounter.c"

typedef struct
{
  short x, y, z;
} Vector3;

/// accelerometer data
Vector3 acc;
/// squared accelerometer magnitude
int accMagSquared, accMag;
uint32_t espruino_stepCounter = 0;
uint32_t dummy_stepCounter = 0;

bool origStepWasLow;
/// How low must acceleration magnitude squared get before we consider the next rise a step?
int origStepCounterThresholdLow = (8192 - 80) * (8192 - 80);
/// How high must acceleration magnitude squared get before we consider it a step?
int origStepCounterThresholdHigh = (8192 + 80) * (8192 + 80);
int origStepCounter = 0;

void stepCount(int newx, int newy, int newz)
{
  int dx = newx - acc.x;
  int dy = newy - acc.y;
  int dz = newz - acc.z;
  acc.x = newx;
  acc.y = newy;
  acc.z = newz;
  accMagSquared = acc.x * acc.x + acc.y * acc.y + acc.z * acc.z;

  // original step counter
  if (accMagSquared < origStepCounterThresholdLow)
    origStepWasLow = true;
  else if ((accMagSquared > origStepCounterThresholdHigh) && origStepWasLow)
  {
    origStepWasLow = false;
    origStepCounter++;
  }
  // Espruino step counter
  espruino_stepCounter += stepcount_new(accMagSquared);

  // Dummy step counter
  dummy_stepCounter += dummy_stepcount_new(accMagSquared);
  // the interface here does not return only the new steps, but the whole counter
  dummy_stepCounter = dummy_stepcount(accMagSquared);
}

void testStepCount(char *filename, char *outfile)
{
  // init
  origStepCounter = 0;
  origStepWasLow = 0;
  espruino_stepCounter = 0;
  dummy_stepCounter = 0;

  // init step counter algos
  stepcount_init();

  dummy_stepcount_init();

  // go
  char *line = NULL;
  size_t len = 0;
  int read;
  int n = 0;
  FILE *fp = fopen(filename, "r");
  FILE *fop = 0;
  if (outfile)
  {
    fop = fopen(outfile, "w");
    fprintf(fop, "n,x,y,z,scaled,filtered,origSteps,steps,thresh\n");
  }
  int x, y, z;
  bool first = true;
  while ((read = getline(&line, &len, fp)) != -1)
  {
    long time = strtol(strtok(line, ","), NULL, 10);
    x = (int)(strtol(strtok(NULL, ","), NULL, 10));
    y = (int)(strtol(strtok(NULL, ","), NULL, 10));
    z = (int)(strtol(strtok(NULL, ","), NULL, 10));
    if (first)
    {
      first = false;
      continue;
    }
    int origStepCounterP = origStepCounter;
    int stepCounterP = espruino_stepCounter;
    stepCount(x, y, z);

    if (fop)
    {
      int M = 6000;
      int a = (origStepCounter - origStepCounterP) * 500 + M;   // old - high
      int b = -(espruino_stepCounter - stepCounterP) * 500 - M; // new - low
      fprintf(fop, "%d,%d,%d,%d,%d,%d,%d,%d,%d\n", n++, x, y, z, accScaled << 6, accFiltered, a, b, stepCounterThreshold);
    }
  }
  // ensure we flush filter to get final steps out
  for (int i = 0; i < ACCELFILTER_TAP_NUM; i++)
  {
    int origStepCounterP = origStepCounter;
    int stepCounterP = espruino_stepCounter;
    stepCount(x, y, z);
    if (fop)
    {
      int M = 6000;
      int a = (origStepCounter - origStepCounterP) * 500 + M;   // old - high
      int b = -(espruino_stepCounter - stepCounterP) * 500 - M; // new - low
      fprintf(fop, "%d,%d,%d,%d,%d,%d,%d,%d,%d\n", n++, x, y, z, accScaled << 6, accFiltered, a, b, stepCounterThreshold);
    }
  }
  if (fop)
    fclose(fop);
  fclose(fp);
  if (line)
    free(line);
}

static int testAll(bool outputFiles)
{
  int fileCnt = 0;
  int differences = 0;
  // show the config and output format
  if (outputFiles)
  {
    printf("X_STEPS = %d, RAW_THRESHOLD = %d\n", X_STEPS, RAW_THRESHOLD);
    printf("File, Expected, Espruino, Original, Dummy\n");
  }
  while (fileCnt < FILECOUNT)
  {
    char buf[256], obuf[256];
    strcpy(buf, PATH);
    strcat(buf, FILES[fileCnt]);
    strcpy(obuf, buf);
    strcat(obuf, ".out.csv");
    // if (outputFiles) printf("VVV %s\n", FILES[fileCnt]);
    testStepCount(buf, outputFiles ? obuf : NULL);

    // work out accuracy %
    float pc;
    if (EXPECTED_STEPS[fileCnt] != 0)
      pc = (100 * (float)espruino_stepCounter / (float)EXPECTED_STEPS[fileCnt]);
    else
      pc = 0.00;

    if (outputFiles)
      printf("%s, %d, %d, %d, %d\n", FILES[fileCnt], EXPECTED_STEPS[fileCnt],
             espruino_stepCounter, 0, dummy_stepCounter);
    int d = espruino_stepCounter - EXPECTED_STEPS[fileCnt];
    differences += d * d * HOWMUCH[fileCnt];
    fileCnt++;
  }

  return differences;
}

int main(int argc, char *argv[])
{
  printf("github.com/gfwilliams/step-count\n");
  printf("----------------------------------\n");

  bool bruteForce = false;
  // printf("argc %d\n",argc);
  if (argc > 1)
  {
    if (strcmp(argv[1], "--bruteforce") == 0)
    { // match
      bruteForce = true;
    }
    else
    {
      printf("Unknown argument!\n\n");
      printf("USAGE:\n");
      printf(" ./main\n");
      printf("   Run single test on all available step data\n");
      printf(" ./main --bruteforce\n");
      printf("   Brute-force all available arguments on all available step data\n");
      return 1;
    }
  }

  int d = testAll(true);
  printf("TOTAL DIFFERENCE %d\n", int_sqrt32(d));
  // =======================
  // comment this out to brute-force over the data to find the best coefficients
  if (!bruteForce)
    return 0;
  // =======================
  int bestDiff = 0xFFFFFFF;
  int best_stepCounterThreshold = 0;

  for (stepCounterThreshold = STEPCOUNTERTHRESHOLD_MIN; stepCounterThreshold <= STEPCOUNTERTHRESHOLD_MAX; stepCounterThreshold += STEPCOUNTERTHRESHOLD_STEP)
  {
    printf("testing %d \n", stepCounterThreshold);
    int d = testAll(false);
    if (d < bestDiff)
    {
      printf("           BEST %d\n", d);
      bestDiff = d;
      best_stepCounterThreshold = stepCounterThreshold;
    }
  }

  printf("best difference %d\n", int_sqrt32(d));
  printf("stepCounterThreshold %d\n", best_stepCounterThreshold);

  return 0;
}
