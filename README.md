# C-Store: A column-oriented DBMS prototype

**Note: Do _not_ use this repository as an actual DBMS. It is a mostly-unmaintained research prototype.**

C-Store was a prototype for a columnar, analytics-focused (read-focused) DBMS, developed in the mid-2000's. Read more about it on the [project's (frozen/archived) homepage](http://db.csail.mit.edu/projects/cstore/). It involed researchers such a Daniel Abadi, Samuel Madden, Miguel Ferreira and [others](db.csail.mit.edu/projects/cstore/#people) (not including the owner/maintainer of this repository!); and resulted in several academic [publications](http://db.csail.mit.edu/projects/cstore/#papers).

C-Store's development as a free-software project/repository ended in 2006, with the last release (so to speak) made in October 2006: C-Store 0.2. Development did continue as a commercial product, [Vertica](http://www.vertica.com/), currently owned by Micro Focus (after its merger with Hewlett-Packard Enterprise Solutions in 2017) - but of course, one cannot study Vertica's closed codebase. 

The final FOSS release of C-Store is available for free download on the project website, butunfortunately - it suffers from multiple issues:

* Incompatible with (newer versions of) modern compilers (even GCC, which it targeted) - they fail compiling it.
* Incompatible with the C++ standard library, especially with respect to file locations.
* Incompatible with newer versions of supporting libraries (e.g. Berkeley DB, LZO compression) and/or their location in modern Linux distributions.
* Uses manually-written Makefile's instead of generating them.
* Does not try to detect location of relevant software on the system.
* Has a lot of questionable-though-compilable code: Ambiguity, unsafe type conversion and so on.

These issues motivated the creation of this repository. The intent is to address - fix or circumvent - the above issues, with minimal changes, and without altering C-Store functionality. Another possible goal is to expand somewhat the range of systems on which CMake would build and run. There is **no intention** of further optimizing the code or introducing any new functionality.

## Contact and feedback

Note: The owner/maintainer of this repository is _not_ one of the authors or researchers involved with C-Store originally!

* Have you found a **bug** in the code? Report it via the [issues page](https://github.com/eyalroz/c-store/issues).
* Do you have **questions about C-Store as a project**, its design, or even implementation specifics? Ask one of the original authors. However, the repository owner/maintainer is [involved](https://arxiv.org/abs/1904.12217) in column store architecture research, and may be interested in discussing your idea in other contexts.
* Are you having **trouble building** C-Store on
   * ... GNU/Linux? Report it via the [issues page](https://github.com/eyalroz/c-store/issues).
   * ... any other platform? Not surprising - C-Store was only intended to build on Linux. However, you may [write the maintainer](mailto:eyalroz@technion.ac.il) to try and explain why you want to make this work on other platforms.
* Do you have a specific idea about how to **improve C-Store**'s design or implementation? <- Don't bother. The code is essentially frozen, and is maintained as a resource for researchers into column stores. 
* Have any **other questions**? [Write the maintainer](mailto:eyalroz@technion.ac.il).

