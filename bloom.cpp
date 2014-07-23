/*******************************************************************************
 * Bloom Filter demonstration
 * Copyright 2014 Samuel Berney
 *
 * Documentation and project outline available in bloom.h header file.
*******************************************************************************/

#include "randomlineaccess.h"
#include "bloom.h"

HashFunction HashMonster::hashFunctions[HashMonster::hashFunctionCount] = {
        HashMonster::builtIn,   HashMonster::djb2, HashMonster::sdbm
};

hash HashMonster::builtIn(std::string key)
{
        std::tr1::hash<std::string> str_hash;   // On *nix and VS2010
        return (hash) str_hash(key);
}

// djb2 by Dan Bernstein
// http://www.cse.yorku.ca/~oz/hash.html
hash HashMonster::djb2(std::string key)
{
        const unsigned char* str = reinterpret_cast<const unsigned char*>(key.c_str());
        hash local_hash = 5381;
        int c;

        while (c = *str++)
                local_hash = ((local_hash << 5) + local_hash) + c; /* local_hash * 33 + c */

        return local_hash;
}

// sdbm (public domain, used in gawk)
// http://www.cse.yorku.ca/~oz/hash.html
hash HashMonster::sdbm(std::string key)
{
        const unsigned char* str = reinterpret_cast<const unsigned char*>(key.c_str());
        hash local_hash = 0;
        int c;

        while (c = *str++)
                local_hash = c + (local_hash << 6) + (local_hash << 16) - local_hash;

        return local_hash;
}

// If user specified bitarray_length or active_hashes_count are larger
// than allowed for by the nature and number of implemented hash functions,
// the constructor creates a Bloom Filter with the maximum largest
// bitarray_length and/or active_hashes_count possible and notifies the user.
BloomFilter::BloomFilter(int bitarray_length, int active_hashes_count)
                : bitarray(bitarray_length, false),
                  bitarray_length_(bitarray_length),
                  active_hashes_count_(active_hashes_count)
{
        // 
        if(bitarray_length > MAX_HASH)
        {
                std::cout << "Supplied bit array length " << bitarray_length
                          << " is longer than the maximum allowed "
                          << MAX_HASH << " bits. Using the maximum length."
                          << std::endl;
                bitarray_length_ = MAX_HASH;
        }

        if(bitarray_length_ == 0)
                throw std::invalid_argument("A Bit Array is required to have at least one bit.");

        if(active_hashes_count_ > HashMonster::hashFunctionCount)
        {
                std::cout << "Supplied hash function count "
                          << active_hashes_count_ << " is larger than "
                          << HashMonster::hashFunctionCount
                          << ", the number of hash functions implemented. "
                          << "Using all hash functions instead." << std::endl;
                active_hashes_count_ = HashMonster::hashFunctionCount;
        }

        if(active_hashes_count_ == 0)
                throw std::invalid_argument("A Bloom Filter requires at least one hash function to operate.");

        return;
}

// Iterates through hash function list to find and set bits associated with key.
void BloomFilter::load(std::string key)
{
        // for each hash, set relevant bits

        for(int i = 0; i < active_hashes_count_; ++i)
        {
                hash hash_index = HashMonster::hashFunctions[i](key) %
                                                        bitarray_length_;
                bitarray[hash_index] = true;
        }
}

// Iterates through hash function list to check if bits associated with the key
// (via the hash function) are set. If any bit is not set, query returns false.
bool BloomFilter::query(std::string value)
{
        bool is_member = true;
        for(int i = 0; i < active_hashes_count_; ++i)
        {
                hash hash_index = HashMonster::hashFunctions[i](value) %
                                                        bitarray_length_;
                if(bitarray[hash_index] == false)
                        is_member = false;
        }

        return is_member;
}

// Uses rand() to select an ascii character in the range ['A', '~').
const char randomChar()
{
        return 'A' + rand() % ('~' - 'A');
}

// Creates a char[] of the required length, populates it with characters
// selected by randomChar(), and adds a '\0' for good measure before converting
// to and returning std::string.
std::string randomWord(int length)
{
        char* new_word = new char[length + 1];
        std::string finished_word;

        for(int j = 0; j < length; ++j)
                new_word[j] = randomChar();
        new_word[length] = '\0';

        finished_word = (std::string) new_word;
        delete[] new_word;
        return finished_word;
}

// Ensures that a mutation (insertion, deletion, in-place mutation) occurs
// to the input string. Uses probabilities in conjunction with rand() to
// randomize the way in which the input string is mutated. If the probabilistic
// randomization fails, a random string is added on the end of the word until
// the mutated string is different from the input string. mutateString() always
// makes sure not to modulo by zero and to use int cast along with floor or ceil
// to reliably and controllably convert floating point numbers into integers.
std::string mutateString(std::string input)
{
        const float char_mutation_rate = 0.3;
        const float p_shorten_word = 0.9;
        const float max_deletion_rate = 0.7;
        const float p_extend_word = 1.0;
        const float max_extension_rate = 0.5;

        // mutates existing characters

        std::string mutated = input;
        for(int i = 0; i < mutated.length(); ++i)
        {
                if(rand() % 100 < char_mutation_rate)
                        mutated[i] = randomChar();
        }

        // deletes characters

        if(rand() % 100 < 100 * p_shorten_word &&
                                        mutated.length() > 0)
        {
                int max_deletions = (int) std::floor(mutated.length() *
                                                     max_deletion_rate);
                if(max_deletions > 0)
                {
                        int num_deletions = rand() % max_deletions;
                        for(int i = 0; i < num_deletions; ++i)
                        {
                                // There must be characters in mutated to
                                // delete if we're here. max_deletions already
                                // takes mutated.length() into account.

                                int delpos = rand() % mutated.length();
                                mutated.erase(delpos, 1);   // erase one letter
                        }
                }
        }

        // inserts new characters

        if(rand() % 100 < 100 * p_extend_word)
        {

                int max_insertions = (int) std::ceil(input.length() *
                                                     max_extension_rate);
                if(max_insertions == 0)
                        max_insertions = 10;

                int num_insertions = rand() % max_insertions;
                for(int i = 0; i < num_insertions; ++i)
                {
                        if(mutated.length() == 0)
                                mutated.insert(0, randomWord(1));
                        else
                                mutated.insert(rand() % mutated.length(),
                                               randomWord(1));
                }
        }

        // makes sure the string has changed

        //while(input == mutated /* and not in dictionary
        //                          (else 5 match training input) */)
        //        mutated += randomWord(10);    // for SLOW functionality
        mutated += randomWord(10);

        return mutated;
}

// Queries random lines in DICTIONARY_FILE. Each line is tested against the
// Bloom Filter and added to the valid_entries array (which must be created
// and deleted outside of testValidEntries and will be modified by
// testValidEntries). This requires that the dictionary file contain
// an entry. If for some reason the Bloom Filter does not recognize a training
// key, the user is notified and an entry at the end of valid_entries is set to
// "bloom failure".
void testValidEntries(RandomLineAccessInterface*  dictionary,
                      int                sample_size,
                      BloomFilter*       bloom,
                      std::string*       valid_entries)
{
        int successes = 0;      // incremented each time the bloom
                                // filter recognizes the dictionary entry 
        int failures = 0;       // incremented when bloom doesn't recognize entry
                                // (should never ever happen)

        // obtain sample_size # of random entries:

        for(int i = 0; i < sample_size; ++i)
        {
                if(dictionary->getLineCount() == 0)
                        throw std::invalid_argument("No Valid Dictionary Entries to Test.");
                int randint = rand() % dictionary->getLineCount();

                // test membership

                if(bloom->query(dictionary->getline(randint)))
                {
                        // record success in successes counter & valid_entries

                        valid_entries[successes++] = dictionary->getline(randint);

                }
                else
                        failures++;
        }

        // Just in case the bloom filter malfunctions, record and notify cerr

        if(failures > 0)
        {
                for(int i = successes; i < sample_size; ++i)
                        valid_entries[i] = "bloom failure";

                std::cerr << "Test testValidEntries failed " << failures
                          << " times. This indicates a problem with the bloom filter."
                          << std::endl;
        }

        // Announce successes to the User and return successes

        std::cout << "Valid Entries:\t\t" << successes << " / " << sample_size
                  << " tested positive." << std::endl;
        return;
}

// Creates a new string based for each string in valid_entries based off
// that string. testInvalidEntries uses mutateString() to ensure that each
// new string is almost certainly not in the dictionary. The function tests
// each new string against the Bloom Filter.
void testInvalidEntries(RandomLineAccessInterface*   dictionary,
                        std::string*        valid_entries,
                        int                 sample_size,
                        BloomFilter*        bloom)
{
        int successes = 0;        // Incremented each time the bloom
                                  // filter recognizes the dictionary entry.
        int false_positives = 0;  // Checked against training dictionary

        for(int i = 0; i < sample_size; ++i)
        {
                // mutate sample and test membership

                valid_entries[i] = mutateString(valid_entries[i]);

                if(bloom->query(valid_entries[i]))
                {
                        successes++;
                        //if(!dictionary->query(valid_entries[i]))
                        //        false_positives++;    // SLOW
                }
        }

        std::cout << "Invalid Entries:\t" << successes << " / " << sample_size
                  << " tested positive." << std::endl;
                  //<< " tested positive. (False Positives: " << false_positives
                  //<< ")" << std::endl;    // SLOW
        return;
}

// Uses randomWord() to generate sample_size # of five character words. Each
// word is tested for membership in the Bloom Filter.
void testRandomPermutations(RandomLineAccessInterface*   dictionary,
                            int                 sample_size,
                            BloomFilter*        bloom)
{
        int successes = 0;        // Incremented each time the bloom
                                  // filter recognizes the dictionary entry.
        int false_positives = 0;  // Checked against training dictionary.

        std::string random_word;
        for(int i = 0; i < sample_size; ++i)
        {
                random_word = randomWord(5);
                if(bloom->query(random_word))
                {
                        successes++;
                        //if(!dictionary->query(random_word))   // SLOW
                        //        false_positives++;
                }
        }

        std::cout << "5 chr random words:\t" << successes << " / "
                  << sample_size << " tested positive." << std::endl;
                  //<< sample_size << " tested positive. (False Positives: "
                  //<< false_positives << ")" << std::endl; // SLOW
        return;
}

// Ensures enough entries are present in the training dictionary (and that
// the training dictionary exists at all). Returns number of entries.
int countKeysAndVerifyDictionaryBigEnough(const char* DICTIONARY_FILE,
                                          const int sample_size)
{
        // Makes sure training dictionary is present

        std::ifstream dictionary(DICTIONARY_FILE);
        if (!dictionary)
        {
                std::cout << "Training dictionary not detected! "
                             "You can download one from\n"
                             "\t(windows)\tsamuelberney.com/wordlist.txt\n"
                             "\t(*nix)\t\tcodekata.com/data/wordlist.txt\n\n"
                             "Ensure the file is located in the "
                             "same directory as this program,\nand "
                             "that it is named wordlist.txt (or "
                             "that you've changed\n`const char DICTIONARY_FILE[]` "
                             "to the appropriate setting in main()).\n"
                             "Ensure that the file has native line endings.\n\n"
                             "You can create your own training dictionary "
                             "in a text file with one 'word' per line.\n\n"
                             "\tone\n\ttwo\n\tthree\n\n is sufficient.\n\n";
                exit(-1);
        }

        int key_count = RandomLineAccessInterface::countLines(&dictionary);
        dictionary.close();

        // Notify user if too few words in training dictionary

        if(key_count == 0)
        {
                std::cout << "Training dictionary must contain at least one word.\n";
                exit(-1);
        }
        else if(key_count < sample_size)
                std::cout << "There are fewer training entries than "
                             "random samples to test.\n(Adjust with "
                             "`const int sample_size`.) Entries will "
                             "be tested more than once.\n\n";

        return key_count;
}

// Opens a training dictionary and loads each entry into the Bloom Filter.
void train(const char* DICTIONARY_FILE, BloomFilter* bloom)
{
        std::ifstream dictionary(DICTIONARY_FILE);
        std::string line;
        while(std::getline(dictionary, line))
        {
                bloom->load(line);
        }
        dictionary.close();
}

// Tests a random sample of valid entries, a generated sample of
// (almost certainly) invalid entries, and random strings for
// membership using the bloom filter.
void test(const char* DICTIONARY_FILE, BloomFilter* bloom, int sample_size)
{
        DenseLineCache dictionary(DICTIONARY_FILE);
        std::string* valid_entries = new std::string[sample_size];
                                           // Will contain each sampled entry.

        testValidEntries(&dictionary,
                         sample_size,      // # of words to test.
                         bloom,
                         valid_entries);   // To populate w/ valid entries.
        testInvalidEntries(&dictionary,
                           valid_entries,  // Strings to modify.
                           sample_size,    // Length of valid_entries.
                           bloom);
        testRandomPermutations(&dictionary, sample_size, bloom);

        delete[] valid_entries;
}

// Creates and trains a Bloom Filter and computes its effectiveness using
// a number of tests; repeatedly for different flavors of Bloom Filter,
// by iteratively changing the number of hash functions (hashcount) used as
// well as the length of the bitarray relative to the size of the training
// dictionary (lenfact).
//
// According to http://pages.cs.wisc.edu/~cao/papers/summary-cache/node8.html,
// hashcount < 3 is required for lenfact == 2. Further constraints on
// hashcount as a function of lenfact exist, however only three hash functions
// are currently implemented. Thus we iterate lenfact from 3 on upwards. This
// is simply a convenient thing to do; other values could've been selected.
//
// Seeds the random number generator with the system time.
int main()
{
        // Demonstration Parameters

        const int random_seed = std::time(NULL);
        const char DICTIONARY_FILE[] = "wordlist.txt";  // The location of the
                                                        // training dictionary.
        const int sample_size = 100;            // # of words to test using
                                                // the Bloom Filter.

        int key_count = countKeysAndVerifyDictionaryBigEnough(DICTIONARY_FILE,
                                                              sample_size);

        // Tries varied settings of lenfact:
        // Bit array length shall be lenfact multiples of the dictionary length.

        for(int lenfact = 3; lenfact < 8; ++lenfact)
        {
                int bitarray_length = int(lenfact * key_count);

                // Tries varied settings of hashcount:
                // Bloom Filter shall use hashcount # of hash functions.

                for(int hashcount = 1;
                    hashcount <= HashMonster::hashFunctionCount;
                    ++hashcount)
                {
                        // tells user what settings we're using

                        std::cout << "lenfact (m/n) = " << lenfact << std::endl
                                  << "hashcount (k) = " << hashcount << std::endl;

                        // Creates, trains, and tests Bloom Filter; outputs
                        // test results to stdout.

                        BloomFilter bloom_filter(bitarray_length, hashcount);
                        train(DICTIONARY_FILE, &bloom_filter);

                        srand(random_seed);
                        test(DICTIONARY_FILE, &bloom_filter, sample_size);

                        std::cout << std::endl;
                }
        }

        return 0;
}
