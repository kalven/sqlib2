#ifndef SQLIB_SQLIB_HPP
#define SQLIB_SQLIB_HPP

#define SQLIB_NOCOPY(cls)                       \
    cls(const cls&) = delete;                   \
    cls& operator=(const cls&) = delete;

#endif
