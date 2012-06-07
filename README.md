PerfCounter
===========


Overview
========

This library provides mechanisms for tracking the memory and time use of a
program. It is platform-independent (read: Linux and Windows) and supports
three main features:

    
* [Querying the memory use](#querying-the-memory-use-of-a-program) of a
  program. (Both peak and current, and both VM size and resident/working set
  size.)
* [Tracking the memory and time
  use](#tracking-the-memory-and-time-use-of-a-program) of a program over a
  portion of the execution.
* [Graphing the memory use](#graphing-the-memory-use-of-a-process) of a
  program over time, optionally with annotations showing program events.

For example:

![Memory usage graph](https://github.com/EvanED/perfcounter/raw/master/doc/example.png)

Caveats
-------

This library is not very robust... there are lots of assertions that can fire
in places where a more graceful failure would be better. It is not tested
very well either.

Also, function, class, and field names may be renamed aggressively in the
future. (Also file and library names. And maybe even the project.)


Building
========

On Windows, use the project files in the ++vc2010/++. Only the debug
configuration has been tested, but the libraries 'should' build under the
release configuration as well. The demo projects probably won't, but just
because `../Release` needs to be added to the library include paths.

Once they are built, find the ++monitor.lib++ and ++getmemusage.lib++
libraries (both are static) and link against one or both of them. You'll also
have to make your project know where the headers are. (You can freely move
both libraries and headers around.)

On Linux, build using ++scons++. Right now the compiler path is hard-coded to
something particular to our environment, but you can feel free to change
that. You'll need a recent GCC though, I think 4.6 or later (at least for
now). (You don't need to use such a recent version for your own project, only
the library needs to be built with it.) You'll have to set `LD_LIBRARY_PATH`
to find the libraries as the `RPATH` setup relies on a patched version of
`scons`.

After `scons` is run, you should have an `install/` directory with `lib/` and
`include/` subdirectories. These are the only files you'll need to use the
library, and you can move them around freely. (Right now only shared
libraries are built on Linux.)


Querying the memory use of a program
====================================

*The query portion works with C and C++.*

`PerfCounter` provides a function which allows you to get the memory use of
any process, as well as a shortcut for this process. The signatures are
simple:

    int get_self_memory_usage(memory_stats_t * out_stats);
    int get_process_memory_usage(int pid, memory_stats_t * out_stats);

The `memory_stats_t` structure contains fields for both the VM size and the
amount that is currently resident in physical memory, as well as the peak
values of both of those. The structure is defined as follows:

    typedef struct {
        long long vm_bytes;
        long long resident_bytes;
        long long vm_bytes_peak;
        long long resident_bytes_peak;
    } memory_stats_t;

The names should be self-evident.

These entities are defined in the header `getmemusage.h`, and implemented
in the `getmemusage` library.


Technical information
---------------------

On Linux, the VM sizes correspond to the `VmSize` and `VmPeak` fields of
the `proc/_pid_/status` contents. The resident set sizes correspond to
the `VmRSS` and `VmHWM` fields. (`HWM` stands for 'high water mark' and
corresponds to the `resident_bytes_peak` field.) You will need read
permission to that file.

On Windows, the VM sizes correspond to the `PagefileUsage` and
`PeakPagefileUsage` of the `PROCESS_MEMORY_COUNTERS` structure. The
resident set sizes correspond to the `WorkingSetSize` and
`PeakWorkingSetSize` fields. You will need the
`PROCESS_QUERY_LIMITED_INFORMATION` (or `PROCESS_QUERY_INFORMATION`) and
`PROCESS_VM_READ` rights for the queried process.


Tracking the memory and time use of a program
=============================================
*The tracking portion works with C++ only.*

The library provides a class for tracking the time and memory use of a
program over a portion (or portions) of its execution. The interface to this
class is as follows:

    class block_tracker
    {
    public:
        block_tracker();
        block_tracker(std::string const & desc);
    
        void start();
        void stop();
    
        void report() const;
        void report(std::ostream & is) const;
    
        bool is_running() const;
    };

This class is defined in the header `block_tracker.hpp`, and implemented in
the `getmemusage` library.

A `block_tracker` instance tracks the time and memory usage between calls
to `start()` and `stop()`. When `report()` is called, it writes a
report of the cumulative time taken by these regions as well as the net
effect of memory changes that occurred during these regions. It also reports
the peak memory use of the entire process at the point `report()` is
called. An example report is as follows:

    ~~~ Resource usage tracker before to between
    ~~~     Cumulative time: 0.822
    ~~~     Net increase in VM size: 524292096
    ~~~     Net increase in RS size: 524333056
    ~~~     Peak VM size: 542662656
    ~~~     Peak RS size: 525611008
    ~~~ (Note: the peak VM and RS sizes may not have been achieved during
    ~~~ the duration of this resource tracker)

The description passed to the constructor is displayed on the first line; in
the example output, it is `before to between`. The description is the empty
string if one is not provided. The report goes to standard error if a stream
isn't provided.

A `block_tracker` is automatically started when it is created and stopped
when it is destroyed (if it is still running). If, at the time it is
destroyed, no report has been made since the last time it was started, it
will also automatically report it. This means that you can declare a
`block_tracker` at the top of a function and it will time the execution of
that function. (This is the source of the name `block_tracker`.)

**Note:** the library will assert if you `start()` a timer that is
  currently running, `stop()` a timer that is currently stopped, or
  `report()` a timer that is currently stopped.


Graphing the memory use of a process
====================================

*This library should work with any language that can correctly link against
a C++ library and have constructors and destructors called at the appropriate
time. The `annotate()` function works with C or C++.*

This will allow you to easily create graphs such as the following:

![Memory usage graph](https://github.com/EvanED/perfcounter/raw/master/doc/example.png)

In this graph, the red line is the VM size, the blue line is the
resident/working set size, and the black lines are the peak versions of those
measures.

At its simplest form, all you have to do is link against the `monitor`
library and run your program. It will output a file called `blah.py` in the
startup directory, which you can run under a Python interpreter with
Matplotlib installed to create `blah.pdf`.

On Linux, it should be possible to `LD_PRELOAD` the library into an
arbitrary process, but I didn't have much luck with that with a simple
example.

As illustrated in the above example, it is possible to include markers in the
graph at program events. (I think this feature makes this library unique.) To
do this, include the header `monitor.h` to get access to the following
function:

    void annotate(const char * str);


Just call that at any point you want to mark.

Caveats (read: bugs!)
---------------------

There are two main caveats I'm aware of.

The first is that sometimes an extra timer interrupt fires and creates an
extra line of output. (This shouldn't happen on the Windows implementation.)
This will cause an error such as the following when you try to run
`blah.py`:

    $ python blah.py
      File "blah.py", line 162
        [35.5264, 1302, 1240,2318, 2251],
        ^
    IndentationError: unexpected indent

If you see such an error, open up `blah.py` and see if there's an extra
line at the end, and remove it.

The second is that there is no attempt to prevent annotations from
overlapping. If you make two calls to `annotate()` close together in
runtime, you won't be able to read them in the graph.

You can see the times events happen, and comment out enough annotations so
that they do not overlap, by looking for the calls to `plt.annotate()` in
`blah.py`.

