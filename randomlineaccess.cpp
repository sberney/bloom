/*******************************************************************************
 * asdf
*******************************************************************************/

#include "randomlineaccess.h"

// Counts lines in any open ifstream [passed by reference] by resetting any
// bad bits (eofbit, badbit, failbit) and reading through the file line by
// line. ifstream shall still be usable after countLines finishes with it.
int RandomLineAccessInterface::countLines(std::ifstream* file_name)
{
        std::string line;           // std::getline() populates this string
        int loc_line_count = 0;     // increments by one after each getline

        // A valid ifstream could have been used before. This returns the stream
        // to operating condition by resetting eofbit (false) and goodbit (true)

        file_name->clear();

        // Counts and returns lines in file.

        file_name->seekg(0);
        while(std::getline((*file_name), line))
                loc_line_count++;

        file_name->clear();         // Resets stream's eofbit to false
        return loc_line_count;
}

// Open file; and create mapping between line number and binary position in file.
// Creates an array containing the binary position of each line in
// DICTIONARY_FILE. If the file can't be opened, the constructor throws an error
// and terminates the program. The dictionary will remain open until the class
// destructor is called. Also allows subsequent use of .getLineCount() to all
// other methods and functions, but .getLineCount() CANNOT be used in this
// constructor.
DenseLineCache::DenseLineCache(const char* DICTIONARY_FILE) : dictionary_file(DICTIONARY_FILE)
{
        // Tests whether the file was opened or not. Either terminates (if the
        // file can't be opened) or builds an index to the file's contents.

        if(!dictionary_file)
        {
                throw std::ios_base::failure(
                                std::string("Could not open file ") +
                                DICTIONARY_FILE
                                );
        }

        line_count = countLines(&dictionary_file);

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

// Closes the dictionary and frees up the space formerly allocated to
// DenseLineCache's index onto the dictionary.
DenseLineCache::~DenseLineCache()
{
        dictionary_file.close();
        delete[] binary_position_of_line;
        return;
}

// An accessor for DenseLineCache's private variable line_count. line_count
// is set by the constructor.
int DenseLineCache::getLineCount() const
{
        return line_count;
}

// Returns the contents of line number line_number in the file indexed by
// DenseLineCache. Seeks to location as stated in index. Resets eofbit only
// as necessary, which is fine unless another method is added to DenseLineCache
// which expects goodbit pre-set.
std::string DenseLineCache::getline(int line_number)
{
        std::string line;       // std::getline() populates this string

        if(dictionary_file.eof())       // Ignore/Reset eof marker
                dictionary_file.clear();

        // Seek to and retrieve line:

        dictionary_file.seekg(binary_position_of_line[line_number]);
        std::getline(dictionary_file, line);

        return line;
}

// True if `value` is in DenseLineCache's dictionary file. This implementation
// tests every line in the dictionary (what an awful thing to do!) A better way
// is to reimplement DenseLineCache with a sparse index and associated values
// kept in memory. Then a binary search (if sorted) would be really fast!
bool DenseLineCache::query(std::string value)
{
        int current_line = 0;
        while(current_line < getLineCount())
        {
                if(getline(current_line) == value)
                        return true;
                ++current_line;
        }
        return false;
}

