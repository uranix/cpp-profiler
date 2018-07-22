=== Simple manual cpp profiler ===

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
