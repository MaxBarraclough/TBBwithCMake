// Loosely based on https://repo.anl-external.org/repos/BlueTBB/tbb30_104oss/src/rml/perfor/tbb_simple.cpp

// #include "stdafx.h"


// int _tmain(int argc, _TCHAR* argv[])
// {
//	return 0;
// }


#include "tbb/task_group.h"
#include <tbb/tbb_thread.h> // for sleep functionality

#include <iostream>
#include <stdio.h> //// MOVE TO iostreams TODO
#include <stdlib.h>

// #if ( _WIN32 || _WIN64 )
// #else
// #endif


using namespace tbb;


#if 0 // just spin up and run four simple tasks, using lambdas
const tbb::tick_count::interval_t oneSecond(1.0);    // double holds the number of seconds

int main(int argc, char *argv[]) {
    puts("Starting...");

    task_group g;

    g.run( [&]{ this_tbb_thread::sleep(oneSecond); puts("[Task 1] started"); } );
    g.run( [&]{ this_tbb_thread::sleep(oneSecond); puts("[Task 2] started"); } );
    g.run( [&]{ this_tbb_thread::sleep(oneSecond); puts("[Task 3] started"); } );

    g.run_and_wait( [&]{ this_tbb_thread::sleep(oneSecond); puts("A message from the main thread"); } );

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

// We use TBB's sleep functionality, to avoid C++14 dependency
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
