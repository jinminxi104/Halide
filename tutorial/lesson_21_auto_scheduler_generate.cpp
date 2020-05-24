// Halide tutorial lesson 21: Auto-Scheduler

// So far we have written Halide schedules by hand, but it is also possible to
// ask Halide to suggest a reasonable schedule. We call this auto-scheduling.
// This lesson demonstrates how to use the auto-scheduler to generate a
// copy-pasteable CPU schedule that can be subsequently improved upon.

// On linux or os x, you can compile and run it like so:

// g++ lesson_21_auto_scheduler_generate.cpp ../tools/GenGen.cpp -g -std=c++11 -fno-rtti -I ../include -L ../bin -lHalide -lpthread -ldl -o lesson_21_generate
// export LD_LIBRARY_PATH=../bin   # For linux
// export DYLD_LIBRARY_PATH=../bin # For OS X
// ./lesson_21_generate -o . -g auto_schedule_gen -f auto_schedule_false -e static_library,h,schedule target=host auto_schedule=false
// ./lesson_21_generate -o . -g auto_schedule_gen -f auto_schedule_true -e static_library,h,schedule target=host auto_schedule=true machine_params=32,16777216,40
// g++ lesson_21_auto_scheduler_run.cpp -std=c++11 -I ../include -I ../tools auto_schedule_false.a auto_schedule_true.a -ldl -lpthread -o lesson_21_run
// ./lesson_21_run

// If you have the entire Halide source tree, you can also build it by
// running:
//    make tutorial_lesson_21_auto_scheduler_run
// in a shell with the current directory at the top of the halide
// source tree.

#include "Halide.h"
#include <stdio.h>

using namespace Halide;

// We will define a generator to auto-schedule.
class AutoScheduled : public Halide::Generator<AutoScheduled> {
public:
    Input<Buffer<int8_t>> input1{"input1", 2};

    Output<Buffer<int8_t>> output{"output", 2};

    void generate() {
        // For our algorithm, we'll use Harris corner detection.
        Func intm("intm");
        Func intm2("intm2");
        Func in = BoundaryConditions::repeat_edge(input1);
        intm(x, y) = in(y,x)+in(x-1, y) + in(x+1, y);
        intm2(x, y) = intm(x,y) + intm(x,y+1) + intm(x, y-1);
        output(x, y) = intm2(x, y-1)+intm2(x, y)+intm2(x, y+1);
    }

    void schedule() {
        if (auto_schedule) {
            input1.set_estimates({{0, 1024}, {0, 1024}});

            output.set_estimates({{0, 1024}, {0, 1024}});

        } else {
        }
    }
private:
    Var x{"x"}, y{"y"}, c{"c"};
};

// As in lesson 15, we register our generator and then compile this
// file along with tools/GenGen.cpp.
HALIDE_REGISTER_GENERATOR(AutoScheduled, auto_schedule_gen)

// After compiling this file, see how to use it in
// lesson_21_auto_scheduler_run.cpp
