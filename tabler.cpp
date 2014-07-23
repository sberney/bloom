/*******************************************************************************
 * asdf
*******************************************************************************/

#include "tabler.h"

template <class T>
int Tabler<T>::get_column_width(int column)
{
        if (widest_in_column_.size() == 0)
                return 0;

        if (uniform_width_ == true)
        {
                if (width_ == 0)
                        width_ = *std::max_element(widest_in_column_.begin(), widest_in_column_.end()); // get max width

                return width_;
        }

        return widest_in_column_[column];
}


int main()
{
        std::cout << "Running!\n";
        return 0;
}
