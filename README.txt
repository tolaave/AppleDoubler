AppleDoubler tool for M.A.C.E. (and other emulators)

DESCRIPTION
===========

This utility only works on native Mac OS X systems (verified to work on at least up to macOS 15.4), as it depends on native resource forks existing in source files, that HFS+/APFS still provide.

HOW TO COMPILE
==============

How to compile On Mac OS X:

- Make sure "compile.sh" is executable (chmod a+x compile.sh)
- Run "./compile.sh" to compile the tool

HOW TO RUN
==========

After compiling, you should have "appledouble" executable in this folder. You can use it to convert files from a source folder to destination folder using the following convention:

	./appledouble src output

And any files in output folder should be directly usable by the NativeFS in M.A.C.E.

(Ps. There is alternatively a xcode project for the tool, but I used it only just to debug and test the tool. I recall that by default the Xcode build destination was buried in developer folders, and needed to be manually adjusted to local folder, so I recommend using the above "compile.sh" shell script to compile to make sure 

DISCLAIMER
==========

This is prototype tool, and may (and likely will contain) bugs, so use it only on data you have backups of, as data loss may occur. Use at your own risk!
