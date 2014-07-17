#include "bloom.h"

HashFunction HashMonster::hashFunctions[HashMonster::hashFunctionCount] = {
        HashMonster::builtIn,   HashMonster::hash1, HashMonster::hash2
};

hash HashMonster::builtIn(std::string key)
{
        std::tr1::hash<std::string> str_hash;
        return str_hash(key);
}

hash HashMonster::hash1(std::string key)
{
        return 10;
}

hash HashMonster::hash2(std::string key)
{
        return 11;
}

// Store key value in Bloom Filter.
bool BloomFilter::load(std::string key)
{
        return false;
}

// Test value's membership using bloom filter
bool BloomFilter::query(std::string value)
{
        return false;
}


// Open file; and create mapping between line number and binary position in file.
RandomLineAccess::RandomLineAccess(const char* DICTIONARY_FILE) : dictionary_file(DICTIONARY_FILE)
{
        // Test whether the file was opened or not. If the file can't be opened,
        // this program has to terminate. Otherwise, we extract its contents

        if(dictionary_file)
        {
                line_count = countLines(&dictionary_file);
        }
        else
        {
                throw std::ios_base::failure(
                                std::string("Could not open file ") +
                                DICTIONARY_FILE
                                );
        }

        // binary-position-of-line is a mapping from Line Number to where that
        // line begins in the dictionary file. It allows usage:
        //      seekg ( binary_position_of_line [ line number ] )

        binary_position_of_line = new int[line_count];

        // The rest of this function populates the binary-position-of-line array.

        std::string line;       // std::getline() requires a string to populate
        int line_number = 1;    // this increments by one each time thru loop

        dictionary_file.seekg(0);
        do
        {
                binary_position_of_line[line_number - 1] = dictionary_file.tellg();
                line_number++;

        } while(std::getline(dictionary_file, line));

        return;
}

RandomLineAccess::~RandomLineAccess()
{
        dictionary_file.close();
        delete[] binary_position_of_line;
        return;
}

// Count lines in any open ifstream (passed by reference).
int RandomLineAccess::countLines(std::ifstream* file_name)
{
        std::string line;           // std::getline() populates this string
        int loc_line_count = 0;     // increments by one after each getline

        // A valid ifstream could have been used before. This returns the stream
        // to operating condition by resetting eofbit (false) and goodbit (true)

        file_name->clear();

        // Count and return lines in file.

        file_name->seekg(0);
        while(std::getline((*file_name), line))
        {
                loc_line_count++;
        }

        file_name->clear();         // Reset stream's eofbit to false
        return loc_line_count;
}

// Return the number of lines cached by RandomLineAccess.
// DO NOT USE in RandomLineAccess's constructor (line_count not yet set).
int RandomLineAccess::getLineCount() const
{
        return line_count;
}

// Retrieve line contents located at line_number in dictionary_file.
std::string RandomLineAccess::getline(int line_number)
{
        std::string line;       // std::getline() populates this string

        if(dictionary_file.eof())       // Ignore/Reset eof marker
                dictionary_file.clear();

        // Seek to and retrieve line:

        dictionary_file.seekg(binary_position_of_line[line_number]);
        std::getline(dictionary_file, line);

        return line;
}


// Open dictionary, obtain word count
int countWordsInDictionary(const char* DICTIONARY_FILE)
{
        std::ifstream dictionary(DICTIONARY_FILE);
        int word_count = RandomLineAccess::countLines(&dictionary);

        dictionary.close();
        return word_count;
}

// Open dictionary, obtain sample_size random entries, test membership;
// return string array of random entries. any failures shall read "bloom failure":
// REMEMBER TO DELETE the dynamically allocated returned string array.
std::string* testValidEntries(const char*   DICTIONARY_FILE,
                              int           sample_size,
                              BloomFilter*  bloom)
{
        // Open dictionary, prepare to obtain sample_size # of random entries:

        RandomLineAccess dictionary(DICTIONARY_FILE);

        int successes = 0;      // incremented each time the bloom
                                // filter recognizes the dictionary entry 
        int failures = 0;       // incremented when bloom doesn't recognize entry
                                // (should never ever happen)

        std::string* valid_entries = new std::string[sample_size];
                                // will record each sampled entry

        // obtain sample_size # of random entries:

        for(int i = 0; i < sample_size; i++)
        {
                int randint = rand() % dictionary.getLineCount();
                std::cout << dictionary.getline(randint) << std::endl;

                // test membership

                if(bloom->query(dictionary.getline(randint)))
                {
                        // record success in successes counter & valid_entries

                        valid_entries[successes++] = dictionary.getline(randint);

                } 
                else
                        failures++;
        }

        // Just in case the bloom filter malfunctions, record and notify cerr

        if(failures > 0)
        {
                for(int i = successes; i < sample_size; i++)
                        valid_entries[i] = "bloom failure";

                std::cerr << "Test testValidEntries failed " << failures
                          << " times. This indicates a problem with the bloom filter."
                          << std::endl;
        }

        // Announce successes to the User and return successes

        std::cout << "Valid Entries: " << successes << " / " << sample_size
                  << " tested positive." << std::endl;
        return valid_entries;
}

// Changes a string in a somewhat random way.
std::string mutateString(std::string input)
{
        std::string mutation = input + "not mutated yet";    // FIXME

        return mutation;
}

// morph valid_entries into invalid entries, and test them against bloom filter.
void testInvalidEntries(std::string*    valid_entries,
                        int             sample_size,
                        BloomFilter*    bloom)
{
        int successes = 0;      // incremented each time the bloom
                                // filter recognizes the dictionary entry 

        for(int i = 0; i < sample_size; i++)
        {
                // mutate sample and test membership

                valid_entries[i] = mutateString(valid_entries[i]);

                if(bloom->query(valid_entries[i]))
                        successes++;
        }

        std::cout << "Invalid Entries: " << successes << " / " << sample_size
                  << " tested positive." << std::endl;
        return;
}

// generate sample_size # of random five character words, & test their membership.
void testRandomPermutations(int sample_size, BloomFilter* bloom)
{
        int successes = 0;      // incremented each time the bloom
                                // filter recognizes the dictionary entry 

        for(int i = 0; i < sample_size; i++)
        {
                // generate random five character word & test its membership

                std::string test_word = "fixme";    // FIXME

                if(bloom->query(test_word))
                        successes++;
        }

        std::cout << "5 letter random words: " << successes << " / "
                  << sample_size << " tested positive." << std::endl;
        return;
}

// Load dictionary into Bloom Filter, demonstrate filter's effectiveness.
int main()
{
        // constants

        const char DICTIONARY_FILE[] = "wordlist.txt";

        /******* Setup *******/

        srand(1984);    // random seed

        // Find number of entries in input dictionary, "key_count".

        int key_count = countWordsInDictionary(DICTIONARY_FILE);

        // Choose parameters which change the efficacy of the bloom filter:
        // The bitarray length and hash function count.

        int bitarray_length = int(2 * key_count);
        int hash_function_count = 1;

        // Create bloom filter

        BloomFilter bloom_filter(bitarray_length, hash_function_count);


        /******* Initialization *******/

        // For every dictionary entry, load it into the Bloom Filter

        std::ifstream dictionary(DICTIONARY_FILE);
        std::string line;
        while(std::getline(dictionary, line))
        {
                bloom_filter.load(line);
        }
        dictionary.close();


        /******* Test Usage *******/

        // Test a random sample of valid entries for membership. Report result.

        const int sample_size = 10;
        std::string* valid_entries = testValidEntries(DICTIONARY_FILE,
                        sample_size, &bloom_filter);

        // Test a sample of invalid entries for membership. Report result.

        testInvalidEntries(valid_entries, sample_size, &bloom_filter);

        // Generate random combinations, and report false positive rate

        testRandomPermutations(sample_size, &bloom_filter);

        delete[] valid_entries;
        return 0;
}
