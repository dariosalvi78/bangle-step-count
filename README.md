Simple step counter test
========================

* In `data` there's some step data from http://forum.espruino.com/conversations/359542
* On `main.c` we have some step counter code which can be run on

## Running

Ensure `Espruino` is installed at the same level as `step-count`,
as this code expects to use the step counter code from Espruino
itself.

Type `make && ./main` to run and execute the file.

Total difference in steps between expected and calculated is shown.

* `expected` = what we expect
* `orig` = step count based on the most trivial step counter

Type `make tuneEspruino && ./tuneEspruino --bruteforce` to run a brute-force check for all
possible values using the offline data from `data`. You can then feed
these back into `stepcount.c`

## Graphs

With `gnuplot` installed you can run `gnuplot -p gnuplot.plot` to plot graphs of acceleration vs step count/theshold data for debugging.

To compile, run and display the most recent graphs, you can do `pkill gnuplot;make clean;make && ./main && gnuplot -p gnuplot.plot`
