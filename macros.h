#ifndef MACROS_H_
#define MACROS_H_

// This single macro is borrowed straight from Google's style guide:
// A macro to disallow the copy constructor and operator= functions
// This should be used in the private: declarations for a class
#define DISALLOW_COPY_AND_ASSIGN(TypeName)  \
        TypeName(const TypeName&);          \
        void operator=(const TypeName&)

#endif
