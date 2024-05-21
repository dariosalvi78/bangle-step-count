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
#include "./algos/original/originalStepCounter.c"

uint32_t espruino_tot_steps = 0;
uint32_t original_tot_steps = 0;
uint32_t dummy_tot_steps = 0;

void feedStepCounters(int newx, int newy, int newz)
{
  int accMagSquared = newx * newx + newy * newy + newz * newz;

  // Espruino step counter
  espruino_tot_steps += stepcount_new(accMagSquared);

  // original step counter
  original_tot_steps = original_stepcount(accMagSquared);

  // Dummy step counter
  dummy_tot_steps = dummy_stepcount(accMagSquared);
}

void testStepCounters(char *filename)
{
  // init
  original_tot_steps = 0;
  espruino_tot_steps = 0;
  dummy_tot_steps = 0;

  // init step counter algos
  stepcount_init();
  original_stepcount_init();
  dummy_stepcount_init();

  // go
  char *line = NULL;
  size_t len = 0;
  int read;
  int n = 0;
  FILE *fp = fopen(filename, "r");

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
      // skip computing steps on the very first value
      continue;
    }
    int origStepCounterP = origStepCounter;
    int stepCounterP = espruino_tot_steps;
    feedStepCounters(x, y, z);
  }
  // ensure we flush filter to get final steps out
  for (int i = 0; i < 10; i++)
  {
    int origStepCounterP = origStepCounter;
    int stepCounterP = espruino_tot_steps;
    feedStepCounters(x, y, z);
  }
  fclose(fp);
  if (line)
    free(line);
}

static void testAll()
{
  int fileCnt = 0;
  int differences = 0;

  printf("File, Expected, Espruino, Original, Dummy\n");
  while (fileCnt < FILECOUNT)
  {
    char buf[256], obuf[256];
    strcpy(buf, PATH);
    strcat(buf, FILES[fileCnt]);
    strcpy(obuf, buf);
    strcat(obuf, ".out.csv");
    // if (outputFiles) printf("VVV %s\n", FILES[fileCnt]);
    testStepCounters(buf);

    printf("%s, %d, %d, %d, %d\n", FILES[fileCnt], EXPECTED_STEPS[fileCnt],
           espruino_tot_steps, original_tot_steps, dummy_tot_steps);
    fileCnt++;
  }
}

int main(int argc, char *argv[])
{
  printf("github.com/gfwilliams/step-count\n");
  printf("----------------------------------\n");

  testAll();

  return 0;
}
