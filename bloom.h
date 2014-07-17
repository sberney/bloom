#include <iostream>     /* cout, ios_base::failure */
#include <fstream>      /* ifstream, getline */
#include <string>       /* string */
#include <cstdlib>      /* rand, srand */
#include <vector>       /* vector<bool> */
#include <tr1/functional>   /* hash<std::string>. Macintosh specific? */

#ifndef BLOOM_H_
#define BLOOM_H_

// This single macro is borrowed straight from Google's style guide:
// A macro to disallow the copy constructor and operator= functions
// This should be used in the private: declarations for a class
#define DISALLOW_COPY_AND_ASSIGN(TypeName)  \
        TypeName(const TypeName&);          \
        void operator=(const TypeName&)


/****** typedefs ******/

typedef int hash;                           // Return type for hash functions.
typedef hash (*HashFunction)(std::string);  // Function pointer to functions
                                            // of form: hash fxn(string).

/****** Forward Declarations ******/

class BloomFilter;

// Counts lines ("words") in file named DICTIONARY_FILE.
int countWordsInDictionary(const char* DICTIONARY_FILE);

// Obtains sample_size random entries from DICTIONARY FILE. Tests each entry
// for membership using BloomFilter bloom. Returns string array of obtained
// dictionary entries. For each entry not recognized by the bloom filter, a
// slot at the end of the returned array will contain "bloom failure".
//
// It is the user's responsibility to delete the returned string array.
std::string* testValidEntries(const char*   DICTIONARY_FILE,
                              int           sample_size,
                              BloomFilter*  bloom);

// Generates sample_size invalid entries based on input valid_entries.
// Tests each invalid entry for membership using BloomFilter bloom.
void testInvalidEntries(std::string*    valid_entries,
                        int             sample_size,
                        BloomFilter*    bloom);

// Generates sample_size # of random five character words. Each entry
// is tested for membership using BloomFilter bloom.
void testRandomPermutations(int sample_size, BloomFilter* bloom);

// Changes a string in a somewhat random way.
std::string mutateString(std::string input);


/****** Class Contracts *****/

// Container class for a variety of hash functions. Cannot be instantiated.
// The class variable hashFunctionCount must be updated to reflect the number
// of hash functions in HashMonster. The function list hashFunctions likewise
// must be updated whenever a new hash function is added.
//      Example usage:
//          std::cout << HashMonster::hash1("hello world");
class HashMonster
{
        public:
                static const int hashFunctionCount = 3;
                static HashFunction hashFunctions[hashFunctionCount];

                static hash builtIn(std::string);
                static hash hash1(std::string);
                static hash hash2(std::string);
        protected:
                HashMonster();  // Disallows instantiation
        private:
                DISALLOW_COPY_AND_ASSIGN(HashMonster);
};

// Bloom Filters test set membership without storing the set. A membership
// query is not guaranteed to be correct if the Bloom Filter returns a positive
// membership result. However, the result is guaranteed to be correct if the
// membership result is negative. Instead of storing a set, Bloom Filters are
// trained on an input set. Each input has a number of hashes applied to it.
// Each hash maps onto a single bit in a bitarray. During training, the bits
// corresponding to each hash of each input are changed to 1. During a query,
// if a bit corresponding to any hash is not set, the input was not in the
// training set.
//      Example usage:
//          bloomFilter BloomFilter(10,3);
//          bloomFilter.load("hello");
//          std::cout << bloomFilter.query("hello");
class BloomFilter
{
        public:
                BloomFilter(int bitarray_length, int active_hashes_count)
                        : bitArray(bitarray_length, false),
                          active_hashes_count(active_hashes_count) {}
                bool load(std::string);
                bool query(std::string);
        private:
                std::vector<bool> bitArray;
                int active_hashes_count;
                DISALLOW_COPY_AND_ASSIGN(BloomFilter);
};

// RandomLineAccess can retrieve the contents of any line in a text file without
// keeping the entire file in memory. This implementation keeps an integer in
// memory for every line in the file. A more memory efficient version would keep
// a fraction of the lines in memory (a sparse index).
//      Example usage:
//          RandomLineAccess database("some file");
//          std::cout << database.getline(27013);
class RandomLineAccess 
{
        public:
                RandomLineAccess(const char* DICTIONARY_FILE);
                ~RandomLineAccess();
                static int countLines(std::ifstream* file_name);
                int getLineCount() const;   // accessor for line_count
                std::string getline(int line_number);
        private:
                std::ifstream dictionary_file;
                int line_count;
                int* binary_position_of_line;
                DISALLOW_COPY_AND_ASSIGN(RandomLineAccess);
};

#endif
