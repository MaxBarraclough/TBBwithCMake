// Loosely based on https://repo.anl-external.org/repos/BlueTBB/tbb30_104oss/src/rml/perfor/tbb_simple.cpp

// #include "stdafx.h"


// int _tmain(int argc, _TCHAR* argv[])
// {
//	return 0;
// }


#include "tbb/task_group.h"

#include <iostream>

#include <tbb/mutex.h>
tbb::mutex my_mutex;

#if _WIN32||_WIN64
#include <Windows.h> /* Sleep */
#else
#error Only tested on Windows!
#include <unistd.h>  /* usleep */
#endif


using namespace tbb;

#if 0 // totally inappropriate and broken use of subclassing of tbb::task, just for giggles
// see https://www.threadingbuildingblocks.org/docs/help/reference/task_scheduler/task_cls.htm
#include "tbb/task.h"

class TrivialTask : public tbb::task {
public:

    TrivialTask() {	}

    task* parent() const { // the "parent" member-function is confusingly referred to as "the successor attribute"
        return NULL;
    }

    task* execute() override {
        puts("Hello from my task!");
        return NULL;
    }

    ~TrivialTask() {
        puts("Destroying my task");
    }
};


// allocate_continuation
// #include <stdio.h>
// #include <stdlib.h>
int main(int argc, char *argv[]) {
    // use TBB's custom allocator, along the lines shown in
    // https://www.threadingbuildingblocks.org/docs/help/tbb_userguide/Simple_Example_Fibonacci_Numbers.htm#tutorial_Simple_Example_Fibonacci_Numbers

    // see also https://www.threadingbuildingblocks.org/docs/help/reference/task_scheduler/task_allocation.htm

    {
        // looking at task_allocation.htm this should use:
        // "cancellation group is the current innermost cancellation group. "
        ////// WHICH WE DON'T HAVE!?!?! shouldn't this cause an assert failure?
        task& t = *new(task::allocate_root()) TrivialTask; // following http://fulla.fnal.gov/intel/tbb/html/a00249.html

        auto count1 = t.ref_count();

        t.execute(); // No concurrency! Hence 'totally inappropriate'
                     // t.decrement_ref_count(); causes underflow!
                     //if ( NULL != t.parent ) {
                     //}


                     // "Because there is no placement delete expression[...]" https://en.wikipedia.org/w/index.php?title=Placement_syntax&oldid=674756226#Custom_allocators

                     // not using standard 'new' so can't use standard 'delete'
                     //delete (task::allocate_root()) &t; // kaboom! VS2010 picks up a double-free problem. Looks like we're missing something important....????

        auto count2 = t.ref_count();

        t.decrement_ref_count(); // same as   t.set_ref_count(0);


    }

    return EXIT_SUCCESS;
}
#endif

#if 0
int main(int argc, char *argv[]) {
    puts("Starting...");

    task_group g;

    g.run([&] {Sleep(2000); puts("Task 1 is away"); });
    g.run([&] {Sleep(2000); puts("Task 2 is away"); });
    g.run([&] {Sleep(2000); puts("Task 3 is away"); });

    g.run_and_wait([&] {Sleep(2000); puts("A message from the main thread"); });



    //////g.wait();
    // getchar();
    return EXIT_SUCCESS;
}
#endif



// demonstrate the const issue with run_and_wait
#if 1
int main(int argc, char *argv[]) {
    puts("Starting main thread...");

    class MyCallable {
    public:

        int dummy; // no-one likes an empty class

        void operator()() {
            puts("Task is running");
        }
    };

    MyCallable f;

    {
        task_group g;

        // g.run( f ); g.wait();
        // g.run_and_wait( f ); // this doesn't work! VS2010 compiler complains of type trouble. seems to cast from const& to &

        g.run_and_wait([&]() {f(); });
        // C++ lambdas seem to survive the const shenanigans, but MyCallable doesn't: build error
    }

    puts("Press Enter to exit");
    getchar();
    return EXIT_SUCCESS;
}
#endif





#if 0 // fun with fake future

// #include "tbb/atomic.h"

// futures in TBB
int main(int argc, char *argv[]) {
    puts("Starting...");

    class MyFuture {
    public:
        int result;

        //		atomic<int> executionHasBegun;
        //		atomic<bool> executionIsComplete;

        MyFuture() :
            result(-1)
            //			,executionHasBegun(),
            //		    ,executionIsComplete() /*Uninitialised at this point. This DOES NOT assign false.*/
        {
            //			this->executionHasBegun = false;
            //			this->executionIsComplete = false; // needlessly atomic assignment (can't opt-out of atomicity here, can we?)
        }

        void operator()() {
            // atomics consistency rules are specified at https://software.intel.com/en-us/node/506092

            // this->executionHasBegun = true; //  unsafe. 'release' semantics ==> "at least this late but possibly later"
            // this->executionHasBegun = 1;    //  unsafe. 'release' semantics ==> "at least this late but possibly later"

            // (using atomic<bool> rather than atomic<bool>) this doesn't work! only <release> is permitted!
            //this->executionHasBegun.store<acquire>(true); // force ues of acquire semantics: "at least this early, but possibly earlier"

            //			this->executionHasBegun.fetch_and_increment<acquire>(); // safe. fully sequentially consistent, huzzah

            puts("Task is running");

            result = 3;

            // this time the default of "at least this late" semantics is appropriate, so we leave the default semantics
            // this->executionIsComplete = true; // guaranteed to happen only after outInt has been assigned, and to propagate as expected
            return;
        }


        int getResult(task_group& tg) {
            //			if ( !this->executionHasBegun ) {
            //				tg.run(*this); // possibly a really bad idea..... futures are meant to be started manually!
            //			}

            //			if ( !this->executionIsComplete ) {
            tg.wait();
            //			}
            return this->result;
        }

    };


    int myOutInt = -1;
    MyFuture f;

    {
        task_group g;


        g.run(f); //	g.wait();
        g.run_and_wait(f); // this doesn't work! VS2010 compiler complains of type trouble. seems to cast from const& to &
                           // what's going on here? curiously the arg is *not* the same type as that of the "run" member-function !!!
                           //g.run_and_wait( [&](){f();} ); // yes, this craziness *is* necessary!

        printf("%d\n", f.getResult(g));

        //g.wait(); // Do nothing. Idempotency of waiting.
        //g.wait();
        //g.wait();
    }


    //g.run( [&]{Sleep(2000);puts("Task 1 is away");} );
    //g.run( [&]{Sleep(2000);puts("Task 2 is away");} );
    //g.run( [&]{Sleep(2000);puts("Task 3 is away");} );

    //g.run_and_wait( [&]{Sleep(2000); puts("A message from the main thread");} );



    //////g.wait();
    // getchar();
    return EXIT_SUCCESS;
}
#endif








#if 0 // the fib example from the web
int fib(int n) {
    int ret;
    if (n < 2) {
        ret = n;
    }
    else {
        int x, y;
        task_group g;

        x = y = 9;

        // g.run( [&]{my_mutex.lock();/* x=fib(n-1); */ puts("Now:\n"); getchar(); my_mutex.unlock();} ); // spawn a task

        g.run([&] {
            tbb::mutex::scoped_lock lock(my_mutex);
            /* x=fib(n-1); */ puts("Now:\n"); getchar();
        }); // spawn a task

            // g.run( [&]{my_mutex.lock();/* y=fib(n-2); */ puts("Now:\n"); getchar(); my_mutex.unlock();} ); // spawn another task
        g.run([&] {
            tbb::mutex::scoped_lock lock(my_mutex);/* y=fib(n-2); */ puts("Now:\n"); getchar();
        }); // spawn another task

        g.wait();                // wait for both tasks to complete

        ret = x + y;
    }

    return ret;
} // - See more at: https://www.threadingbuildingblocks.org/tutorial-intel-tbb-task-based-programming#sthash.EzgRXzaB.dpuf

int main(int argc, char *argv[]) {
    std::cout << fib(33);

    // getchar();
    return EXIT_SUCCESS;
}

#endif
