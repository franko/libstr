# Libstr

A minimalist C and C++ libraries with no dependencies.

This library is a mostly trivial implementation of a string library for C and
C++ based on C99 and C++14 standards.

It is only meant for simple project with no sophisticated needs that for some reason do not want or can use std::string.

The advantages it may offer are:
  - can be used both from C and C++ code
  - does not depends on any external library (use only the C runtime library)
  - avoid iostream and std::string for those who do not want to use them
  - does not throw exceptions (it will abort if cannot allocate memory)

## Notes for C++ programmers

This library may seem primitive to some C++ programmers because it does use old
C-style printf based formatting. This is meant to be like that for compatibility
with C and to explicitly avoid iostreams.

Most C++ programmers does not need this library and can use std::string and iostreams.
