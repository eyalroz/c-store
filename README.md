# C-Store: A column-oriented DBMS prototype

**Note: Do _not_ use this repository in production code. It is a (now old) research prototype.**

C-Store was a collaboration project between researches in MIT, and the universities of Yale, Brandeis, Brown and Massachusats Boston. Read more about it on the [project's (frozen/archived) homepage](http://db.csail.mit.edu/projects/cstore/) - where you can also find links to the academic publications about it and to the people involved in it.

C-Store's development as a free-software project/repository ended in 2006, with the last release (so to speak) made in October 2006: C-Store 0.2. Development did continue as a commercial product, [Vertica](http://www.vertica.com/), currently owned by Micro Focus (after its merger with Hewlett-Packard Enterprise Solutions in 2017); but the final FOSS release is available on the project website. 

Unfortunately - the final official FOSS version of C-Store has multiple issues:

* Incompatible with modern compilers (even GCC which it targeted) - they fail compiling it.
* Incompatible with the C++ standard library, especially with respect to file locations.
* Incompatible with newer versions of supporting libraries (e.g. Berkeley DB, LZO compression) and/or their location in modern Linux distributions.
* Uses manually-written Makefile's instead of generating them.
* Does not try to detect location of relevant software on the system.
* Has a lot of questionable, though compilable code: Ambiguity, unsafe type conversion and so on.

These issues motivated the creation of this repository. The intent is to address - fix or circumvent - the above issues, while not changing any of the C-Store functionality; and perhap even improve availability on non-Linux machines. There is **no intention** of further optimizing the code or introducing any new functionality.

Feel free to use the issues page to report bugs or other issues. You may also [write me](mailto:eyalroz@technion.ac.il) with questions or for further discussion.
