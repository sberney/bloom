/******************************************************************************
 * asdf
*******************************************************************************/

#include <fstream>      /* ifstream, getline */
#include <string>       /* string */
#include "macros.h"

#ifndef RANDOM_LINE_ACCESS_H_
#define RANDOM_LINE_ACCESS_H_

// RandomLineAccessInterface can retrieve the contents of any line in a text
// file without keeping the entire file in memory.
//      Example usage:
//          RandomLineAccess database("some file");
//          std::cout << database.getline(27013);
class RandomLineAccessInterface
{
        public:
                //virtual RandomLineAccessInterface(const char* DICTIONARY_FILE) = 0;
                virtual ~RandomLineAccessInterface() {}//= 0;
                virtual std::string getline(int line_number) = 0;  // return contents at line_number
                virtual bool query(std::string value) = 0;         // true if value is in the file
                virtual int getLineCount() const = 0;
                static int countLines(std::ifstream* file_name);
};

// This implementation of RandomLineAccessInterface keeps an integer in
// memory for every line in the file. A more memory efficient version would keep
// a fraction of the lines in memory (a sparse index).
// The query method
class DenseLineCache : public virtual RandomLineAccessInterface
{
        public:
                DenseLineCache(const char* DICTIONARY_FILE);
                virtual ~DenseLineCache();
                virtual std::string getline(int line_number);  // see class docs
                virtual bool query(std::string value);  // true if value is in the file
                virtual int getLineCount() const;   // accessor for line_count
        private:
                std::ifstream dictionary_file;
                int line_count;
                int* binary_position_of_line;  // an index onto dictionary_file
                DISALLOW_COPY_AND_ASSIGN(DenseLineCache);
};

#endif
