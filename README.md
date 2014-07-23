bloom
=====

Bloom Filter Implementation
Copyright 2014 Samuel Berney (sam@samuelberney.com)

 This project demonstrates an implementation of a Bloom Filter. For more
 information about Bloom Filters, read the comment block above the BloomFilter
 class below. The problem statement (create a Bloom Filter that can store
 a large dictionary) was found at codekata.com/kata/kata05-bloom-filters/.

* ORGANIZATION OF PROJECT AND DOCUMENTATION
 You will find abstract function and class documentation in this file. More
 implementation specific comments are located with the function definitions.
 This header contains additional information in the categories below.

* PROGRAM OUTPUT
 Program output should look as follows:
 lenfact (m/n) = 6
 hashcount (k) = 2
 Valid Entries: 100 / 100 tested positive.
 Invalid Entries: 8 / 100 tested positive.
 5 chr random words: 9 / 100 tested positive.

 The first two entries describe settings used on the Bloom Filter. lenfact
 is how many times longer the bit array is longer than the training
 dictionary length. hashcount is the number of hash functions used.
 (These are called "m/n" and "k" respectively on a very useful site
 I recommend visiting: pages.cs.wisc.edu/~cao/papers/summary-cache/node8.html)

 The second three entries describe the results of tests performed on the
 Bloom Filter. The Bloom Filter should recognize 100% of the entries
 it was trained on (the first test). It should recognize a few invalid
 entries and a few random entries. False positives should reduce with
 higher lenfact and hashcount.

* COMPILATION NOTES (SEE ALSO: KNOWN BUGS AND COMPILER IDS)
 ** Compiling using VS2010 works fine (even with line 164, see below.
 ** Compiling from command line (VS2010 sp1) required using /EHsc option and commenting
 out line bloom.cpp:164 (after building with VS2010 gui, I could put line 164 back in)
 ** Compiling with g++, replace #include<functional> with #include<tr1/function>.

* KNOWN BUGS
 ** tellg()/getline()
 ** Symptoms: bloom filter does not recognize Valid Entries.
 ** Remedy: convert non-native line breaks to [Windows] (native) specific line returns.
 https://kb.iu.edu/d/acux
 ** Description: Does not affect gnu c++ compiler. Cannot currently parse *nix text files
 on Windows when compiled with VS2010. tellg() moves the cursor unnecessarily and
 reports an incorrect location. When seeking to the saved locations, an offset is
 introduced and getline() produces garbage. Every solution I found on the internet
 involved opening the file in binary.
 ** Affects: RandomLineAccess::RandomLineAccess and RandomLineAccess::getline.

* ABSTRACT PROGRAM FLOW
 SETUP
 Calculate dictionary word count directly from file.
 Use word count to pick bitarray length and optimal (or sub-optimal)
 hash key count.

 INITIALIZATION
 Instantiate Bloom Filter class
 For every dictionary entry, load it into the Bloom Filter.

 TEST USAGE
 Test a random sample of trained entries for membership. Report result.
 Generate and test a set of invalid entries. Report Result.
 Generate random combinations and test for membership. Report Result.
 Done.

* ON BLOOM FILTERS AND USAGE
 This Bloom Filter requires a training dictionary. Here is the preferred
 dictionary for you to use:
 (windows formatting) samuelberney.com/wordlist.txt
 (unix formatting) codekata.com/data/wordlist.txt. But you
 can use any dictionary (like /usr/dict/words or /usr/share/dict/words) so
 long as you are aware of the following formatting rules:

 The training dictionary is required to be a text file with a single word
 per line. Any whitespace will be considered a part of a word. The longest
 word allowed depends on system hardware and can be found in the following way:
 std::string str = "anything";
 std::cout << str.max_size() << std::endl;
 It is the user's responsibility to ensure that all lines in the training
 dictionary do not exceed this limit (which is ridiculously large);
 you are likely to run out of memory if operating in this regime.
 You must use native line break endings (see KNOWN BUGS).

* FUTURE DIRECTIONS
 This project needs an enhanced user interface. It needs better data
 presentation and a way to change settings without recompiling the program.
 * test functions can return a table object, which prints afterwards
 * can make interactive or create parameters file

 RandomLineAccess::query() needs to be changed to a more efficient solution:
 current implementation is a last minute addition to check if any mutated
 entries (testInvalidEntries) are still in the training dictionary.
 * can implement binary search if check ordered at RandomLineAccess
 construction. Would integrate well with a sparse implementation of
 RandomLineAccess. Can store sparse values in memory to speed search.

* ACKNOWLEDGEMENTS FOR ALL THIRD PARTY FUNCTIONS
 Two functions and a macro from third parties were used in this demonstration:
 Hash functions djb2 and sdbm (http://www.cse.yorku.ca/~oz/hash.html), as
 well as the DISALLOW_COPY_AND_ASSIGN macro
 (http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml).

* COMPILER IDS
 This project was compiled and tested using
 g++ (GCC) 4.7.4 20130416 for GNAT GPL 2013 (20130314) on Mac OS X
 Microsoft (R) 32-bit C/C++ Optimizing Compiler Version 16.00.40219.01 for 80x86 with /EHsc
 g++ (GCC) 4.8.3 for Target: x86_64-pc-cygwin
