/*******************************************************************************
 * asdf
*******************************************************************************/

#include <algorithm>    /* max_element */

#ifndef TABLER_H_
#define TABLER_H_


class Row;

template <class T>
class Tabler
{
        public:
                Tabler();
                ~Tabler();

                Row<T> add_row(std::string row_key);

                void print();
                //void format();    // we'll see after the rest is done

                void set_row_label();
                void set column_label();
                void set_column_keys(std::string* keys);
                void set_uniform_width(int width = 0);      // width = 0 => auto pick width
                //set_formatter
        private:
                friend Row* row_list_;

                int get_column_width(int column);
                bool uniform_width_;
                bool width_;
                std::vector<int> widest_in_column_; // as elements are added, check if it's widest against this, and update if it is

                std::string row_label_;     // m/n
                std::string column_label_;  // k
                std::string* column_keys_;  // k = 1, 2, 3, 4, 5, ...
                DISALLOW_COPY_AND_ASSIGN();
};



template <class T>
void Tabler<T>::print()
{
        // Header formatting      m
        //                      k x 1 2 3 4 5 k
        //                        1 . . . . .
        //                        2 . . . . .
        //                        m

}

template <class T>
class Row// : protected Tabler<T>
{
        public:
                Row();
                ~Row();
                void add_data(T data);
        private:
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
