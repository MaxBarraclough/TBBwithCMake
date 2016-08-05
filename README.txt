About
=====

A simple example program using Intel Threading Building Blocks.

CMake is used for the metabuild/package-finding task.
A third-party CMake script is used to find TBB, as there is no 'official' CMake module for this.
The module was taken from
https://github.com/justusc/FindTBB/blob/c8471320e7f11ea/FindTBB.cmake


Building and Running
====================

Windows
-------

For CMake to find TBB, you'll have to set the CMake variable TBB_ROOT_DIR to give the path of the root directory of your TBB build. This may look something like: `C:/Users/mb/Downloads/tbb45/tbb44_20160526oss`

Note that ***you also need to ensure the relevant TBB DLL can be found at runtime***.
The easiest way to do this is to simply copy tbb_debug.dll from, for example `tbbroot\bin\intel64\vc14`
into the main project folder (note: *not* the 'bin' folder).


Linux
-----

Not tested, but it should (hopefully) be much the same as on Windows.
