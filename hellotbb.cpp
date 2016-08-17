// Loosely based on https://repo.anl-external.org/repos/BlueTBB/tbb30_104oss/src/rml/perfor/tbb_simple.cpp

// #include "stdafx.h"


// int _tmain(int argc, _TCHAR* argv[])
// {
//	return 0;
// }


#include "tbb/task_group.h"

#include <iostream>
#include <stdio.h> //// MOVE TO iostreams TODO
#include <stdlib.h>

//#include <chrono> // for 2s to mean 2 seconds
//using namespace std::literals::chrono_literals; // as above
//#include <thread> // for std::this_thread::sleep_for(1);

#include <tbb/mutex.h>
#include <tbb/tbb_thread.h>

// tbb::mutex my_mutex;

#if _WIN32||_WIN64
//#include <Windows.h> /* Sleep */
#else
//// #error Only tested on Windows! TODO
// #include <unistd.h>  /* usleep */
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



#if 1 // fun with the idea of futures

// #include "tbb/atomic.h"

// Faking/implementing futures with TBB tasks

// Note that mutable state is held in its own struct,
// as TBB's run(1) function accepts its arg by const reference,
// forcing us to handle mutation via a pointer to a mutable value
// and not within the MyFuture class itself.


// we use TBB's sleep functionality, to avoid C++14 dependency
const tbb::tick_count::interval_t oneSecond(1.0);    // double holds the number of seconds
const tbb::tick_count::interval_t threeSeconds(3.0);
const tbb::tick_count::interval_t tenSeconds(10.0);

int main(int argc, char *argv[]) {
    puts("Starting...");

    class MyFuture {
      struct MutableState {
        MutableState() : result(0) { }
        int result;
      };

      class Executor {
        /*not const*/ MutableState * const msPtr;
      public:
        Executor(MutableState *m) : msPtr(m) { };

        void operator()() const {
          puts("[from task] Task is running. Now for the pause...");
//        this_tbb_thread::sleep( threeSeconds );
          this_tbb_thread::sleep( tenSeconds );
          puts("[from task] Task pause complete, assigning output...");
          msPtr->result = 3;
          return;
        }
      };

      task_group   tg;
      MutableState ms;
      const Executor e; // must be const in order to call run(1)

    public:
      MyFuture() : e(&ms) // just invoke default constructor of tg and ms
      { }

      void begin() {
        tg.run( this->e );
      }

      int force() /*const*/ {
        tg.wait();
        return ms.result;
      }
    };

    MyFuture f; // not const: we mutate internally

    puts("Now to run");
    f.begin();
    puts("Running. Now to do a couple of prints with a pause between each.");
    this_tbb_thread::sleep( oneSecond );
    puts("And here we are after a second");
    this_tbb_thread::sleep( oneSecond );
    puts("And here we are after another second");
    this_tbb_thread::sleep( oneSecond );
    puts("And here we are after yet another second");
    this_tbb_thread::sleep( oneSecond );
    puts("And here we are after yet another second");
    puts("And now to wait...");

    printf( "%d\n", f.force() );

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
