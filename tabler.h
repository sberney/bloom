/*******************************************************************************
 * asdf
*******************************************************************************/

#include <algorithm>    /* max_element */
#include <string>       /* std::string */
#include <vector>       /* std::vector */
#include <iostream>     /* temporarily, for cout */
#include "macros.h"

#ifndef TABLER_H_
#define TABLER_H_


template <class T> class Row;

template <class T>
class TablerInterface
{
        public:
                virtual ~TablerInterface() {}

                virtual Row<T> add_row(std::string row_key) = 0;

                virtual std::string format() = 0;    // we'll see after the rest is done
                virtual void print() = 0;

                virtual void set_row_label() = 0;
                virtual void set_column_label() = 0;
                virtual void set_column_keys(std::string* keys) = 0;
                virtual void set_uniform_width(int width = 0) = 0;      // width = 0 => auto pick width
                virtual void set_formatter(/* functionPointer */) = 0;
};


template <class T>
class Tabler : public virtual TablerInterface<T>
{
        public:
                Tabler();
                virtual ~Tabler();

                virtual Row<T> add_row(std::string row_key);

                virtual void print();
                virtual std::string format();    // we'll see after the rest is done

                virtual void set_row_label();
                virtual void set_column_label();
                virtual void set_column_keys(std::string* keys);
                virtual void set_uniform_width(int width = 0);      // width = 0 => auto pick width
                virtual void set_formatter();
        private:
                //friend Row<T>* row_list_;
                Row<T>* row_list_;

                int get_column_width(int column);
                bool uniform_width_;
                bool width_;
                std::vector<int> widest_in_column_; // as elements are added, check if it's widest against this, and update if it is

                std::string row_label_;     // m/n
                std::string column_label_;  // k
                std::string* column_keys_;  // k = 1, 2, 3, 4, 5, ...
                //DISALLOW_COPY_AND_ASSIGN();
};



template <class T>
void Tabler<T>::print()
{
        // Header formatting      m
        //                      k x 1 2 3 4 5 k
        //                        1 . . . . .
        //                        2 . . . . .
        //                        m
        return;
}

template <class T>
class Row// : protected Tabler<T>
{
        public:
                Row();
                ~Row();
                void add_data(T data);
};


/*
table = Tabler
for outerloop
        row = table.add_row()
        for innerloop
                row.add_data(data)
table.print()
*/

#endif
