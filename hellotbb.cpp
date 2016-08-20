// #include "stdafx.h"


// int _tmain(int argc, _TCHAR* argv[])
// {
//	return 0;
// }


#include <tbb/task_group.h>
#include <tbb/tbb_thread.h> // for sleep functionality
// An alternative can be taken from
// https://repo.anl-external.org/repos/BlueTBB/tbb30_104oss/src/rml/perfor/tbb_simple.cpp
//void MilliSleep(unsigned milliseconds) {
//#if ( _WIN32 || _WIN64 )
//    Sleep(milliseconds); // accepts DWORD: 32-bit unsigned integer
//#else
//    usleep(milliseconds * 1000); // accepts mysterious useconds_t
//#endif
//}


#include <iostream>
#include <stdio.h> //// MOVE TO iostreams TODO
#include <stdlib.h>

using namespace tbb;


#if 0 // just spin up and run four simple tasks, using lambdas
const tbb::tick_count::interval_t oneSecond(1.0);    // double holds the number of seconds

int main(int argc, char *argv[]) {
  setbuf(stdout, NULL); // disable buffering on stdout before we do anything

  puts("Starting...");

  task_group g;

  g.run( [&]{ this_tbb_thread::sleep(oneSecond); puts("[Task 1] started"); } );
  g.run( [&]{ this_tbb_thread::sleep(oneSecond); puts("[Task 2] started"); } );
  g.run( [&]{ this_tbb_thread::sleep(oneSecond); puts("[Task 3] started"); } );

  g.run_and_wait( [&]{ this_tbb_thread::sleep(oneSecond); puts("A message from the main thread"); } );

  return EXIT_SUCCESS;
}
#endif






#if 0 // fun with the idea of futures

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
  setbuf(stdout, NULL); // disable buffering on stdout before we do anything

  puts("Starting...");

  class MyFuture {
    struct MutableState {
      int result;
      MutableState() : result(0) { }
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






#if 0
const tbb::tick_count::interval_t oneSecond(1.0);    // double holds the number of seconds

int main(int argc, char *argv[]) {
  setbuf(stdout, NULL); // disable buffering on stdout before we do anything

  task_group tg;

  puts("Here we go");

  int myInt = 0;

  tg.run( [&](){
    myInt = 3;
    puts("[from task] Now to sleep");
    this_tbb_thread::sleep( oneSecond );
    puts("[from task] Done sleeping");
  } );


  puts("Now to wait");
  tg.wait();
  puts("Done waiting");
  printf("Value is now %d\n",myInt);

  return EXIT_SUCCESS;
}

#endif






#if 1
struct SelfMutatingFuture {
  int myInt;
  SelfMutatingFuture() : myInt(0) { }

  void operator()() {
    this->myInt = 3;
  }
};


int main(int argc, char *argv[]) {
  setbuf(stdout, NULL); // disable buffering on stdout before we do anything

  task_group tg;
  SelfMutatingFuture s;

  puts("Here we go");

// We could use run_and_wait, but to be more illustrative, we don't

// tg.run(s); // forbidden: operator()() is not const

// lambda itself is const, doesn't matter that 's' (captured by ref) is mutated
  tg.run( [&](){
    s();
  } );

  puts("Now to wait");
  tg.wait();
  puts("Done waiting");
  printf("Value is now %d\n",s.myInt);

  return EXIT_SUCCESS;
}
#endif

