# Simple manual cpp profiler

Procs:
* Single header
* Profile only what you need
* Profiling arbitrary scope
* Early profiling scope ending

Cons:
* Overhead ~300ns per call
* You need to explicitely mark what you want to profile
* May break in multithreaded environments
* Combines calls to the same function from different contexts, e.g. if `foo() -> baz(), bar() -> baz()` then `baz()` will be shown only after `foo()`
* Complex recursion (e.g. `rec1` calls `rec2` and `rec2` calls `rec1`) may lead to incorrect total time for both `rec1` and `rec2`.

## Demo output
```
[bar] Sleeping for 99978us twice

function                                                    line	calls	   tottime	avgtime

int main().................................................. 57       1            1.34s      1.34s 
 foo for loop............................................... 60       1          905.03ms   905.03ms
  double foo(int)........................................... 10       10         905.02ms    90.50ms
   nop...................................................... 17       1000000    277.58ms   277.58ns
 bar wait................................................... 66       1          200.21ms   200.21ms
  double bar(int)........................................... 23       1          100.13ms   100.13ms
 fibb....................................................... 71       1          208.41ms   208.41ms
  int fibb(int)............................................. 31       177        207.51ms     1.17ms
 rec1 + rec2................................................ 76       1           30.17ms    30.17ms
  int rec1(int)............................................. 41       89          29.87ms   335.61us
   int rec2(int)............................................ 49       88          28.18ms   320.28us
```
