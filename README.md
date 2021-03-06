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

For CMake to find TBB, you'll have to set the CMake variable `TBB_ROOT_DIR` to give the path of the root directory of your TBB build. This may look something like: `C:/Users/mb/Downloads/tbb44/tbb44_20160526oss`

Note that ***you also need to ensure the relevant TBB DLL can be found at runtime***.
The easiest way to do this is to simply copy `tbb_debug.dll` and `tbb.dll` from, for example `C:/Users/mb/Downloads/tbb44/tbb44_20160526oss/bin/intel64/vc14`
into the main project folder (note: *not* the `bin` folder).

You may find it useful to adjust `createLinkToTbbDll.bat` to work on your system.

If you forget to handle the DLLs in this way, you will see an error-message output if you attempt to run your .exe, along the lines of either

> error while loading shared libraries: tbb.dll: cannot open shared object file: No such file or directory

or (yes, oddly)

> error while loading shared libraries: api-ms-win-crt-heap-l1-1-0.dll: cannot open shared object file: No such file or directory


Linux
-----

CMake may need hints to find your install of Intel TBB. Adjust `configureCMakeLinux.sh` to work on your system.

Unlike on Windows, there's no need to worry about the system finding the shared object library after the build has completed.

