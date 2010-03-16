#ifndef SQLIB_TRACING_HPP
#define SQLIB_TRACING_HPP

#ifdef SQLIB_TRACING

#include <iostream>
#include <vector>

namespace sqlib
{
    namespace detail
    {
        inline std::ostream*& trace_stream()
        {
            static std::ostream* ts = 0;
            return ts;
        }
    }

    // extern std::ostream * trace_out;

    inline void enable_sql_trace(std::ostream & os)
    {
        detail::trace_stream() = &os;
    }

    inline void disable_sql_trace()
    {
        detail::trace_stream() = 0;
    }

    // For the blob type
    inline std::ostream & operator<<(std::ostream& os, const std::vector<char>& blob)
    {
        return os << "#<sqlib " << blob.size() << " byte blob>";
    }
}

#define SQLIB_TRACE(x) do { if(::sqlib::detail::trace_stream()) { (*::sqlib::detail::trace_stream()) << x << std::endl; } } while(0);
#define SQLIB_TRACE_ENABLE(x) ::sqlib::enable_sql_trace(x);

#else

#define SQLIB_TRACE(x) ;
#define SQLIB_TRACE_ENABLE(x) ;

#endif

#endif
