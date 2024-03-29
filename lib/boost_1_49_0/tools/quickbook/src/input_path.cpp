/*=============================================================================
    Copyright (c) 2009 Daniel James

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#include <boost/program_options.hpp>
#include <iostream>
#include "input_path.hpp"
#include "utils.hpp"
#include "files.hpp"

#if QUICKBOOK_WIDE_PATHS || QUICKBOOK_WIDE_STREAMS
#include <boost/scoped_ptr.hpp>
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#endif

#if QUICKBOOK_CYGWIN_PATHS
#include <boost/scoped_array.hpp>
#include <boost/program_options/errors.hpp>
#include <sys/cygwin.h>
#endif

namespace quickbook {
    extern bool ms_errors;
}

namespace quickbook {
namespace detail {

// This is used for converting paths to UTF-8 on cygin.
// Might be better not to use a windows 
#if QUICKBOOK_WIDE_PATHS || QUICKBOOK_WIDE_STREAMS
    namespace {
        std::string to_utf8(std::wstring const& x)
        {
            int buffer_count = WideCharToMultiByte(CP_UTF8, 0, x.c_str(), -1, 0, 0, 0, 0); 
        
            if (!buffer_count)
                throw conversion_error("Error converting wide string to utf-8.");
    
            boost::scoped_ptr<char> buffer(new char[buffer_count]);
    
            if (!WideCharToMultiByte(CP_UTF8, 0, x.c_str(), -1, buffer.get(), buffer_count, 0, 0))
                throw conversion_error("Error converting wide string to utf-8.");
            
            return std::string(buffer.get());
        }

        std::wstring from_utf8(std::string const& x)
        {
            int buffer_count = MultiByteToWideChar(CP_UTF8, 0, x.c_str(), -1, 0, 0); 
        
            if (!buffer_count)
                throw conversion_error("Error converting utf-8 to wide string.");
    
            boost::scoped_ptr<wchar_t> buffer(new wchar_t[buffer_count]);
    
            if (!MultiByteToWideChar(CP_UTF8, 0, x.c_str(), -1, buffer.get(), buffer_count))
                throw conversion_error("Error converting utf-8 to wide string.");
            
            return std::wstring(buffer.get());
        }
    }
#endif

#if QUICKBOOK_WIDE_PATHS
    std::string input_to_utf8(input_string const& x)
    {
        return to_utf8(x);
    }
#else
    std::string input_to_utf8(input_string const& x)
    {
        return x;
    }
#endif

#if QUICKBOOK_WIDE_PATHS
    fs::path generic_to_path(std::string const& x)
    {
        return fs::path(from_utf8(x));
    }

    std::string path_to_generic(fs::path const& x)
    {
        return to_utf8(x.generic_wstring());
    }
#else
    fs::path generic_to_path(std::string const& x)
    {
        return fs::path(x);
    }

    std::string path_to_generic(fs::path const& x)
    {
        return x.generic_string();
    }

#endif

#if QUICKBOOK_CYGWIN_PATHS
    fs::path input_to_path(input_string const& path)
    {
        cygwin_conv_path_t flags = CCP_POSIX_TO_WIN_W | CCP_RELATIVE;

        ssize_t size = cygwin_conv_path(flags, path.c_str(), NULL, 0);
        
        if (size < 0)
            throw conversion_error("Error converting cygwin path to windows.");

        boost::scoped_array<char> result(new char[size]);
        void* ptr = result.get();

        if(cygwin_conv_path(flags, path.c_str(), ptr, size))
            throw conversion_error("Error converting cygwin path to windows.");

        return fs::path(static_cast<wchar_t*>(ptr));
    }
    
    stream_string path_to_stream(fs::path const& path)
    {
        cygwin_conv_path_t flags = CCP_WIN_W_TO_POSIX | CCP_RELATIVE;

        ssize_t size = cygwin_conv_path(flags, path.native().c_str(), NULL, 0);
        
        if (size < 0)
            throw conversion_error("Error converting windows path to cygwin.");

        boost::scoped_array<char> result(new char[size]);

        if(cygwin_conv_path(flags, path.native().c_str(), result.get(), size))
            throw conversion_error("Error converting windows path to cygwin.");

        return std::string(result.get());
    }
#else
    fs::path input_to_path(input_string const& path)
    {
        return fs::path(path);
    }

#if QUICKBOOK_WIDE_PATHS && !QUICKBOOK_WIDE_STREAMS
    stream_string path_to_stream(fs::path const& path)
    {
        return path.string();
    }
#else
    stream_string path_to_stream(fs::path const& path)
    {
        return path.native();
    }
#endif

#endif // QUICKBOOK_CYGWIN_PATHS

#if QUICKBOOK_WIDE_STREAMS

    void initialise_output()
    {
        if (_isatty(_fileno(stdout))) _setmode(_fileno(stdout), _O_U16TEXT);
        if (_isatty(_fileno(stderr))) _setmode(_fileno(stderr), _O_U16TEXT);
    }

    void write_utf8(ostream& out, std::string const& x)
    {
        out << from_utf8(x);
    }

    ostream& out()
    {
        return std::wcout;
    }

    namespace
    {
        inline ostream& error_stream()
        {
            return std::wcerr;
        }
    }

#else

    void initialise_output()
    {
    }

    void write_utf8(ostream& out, std::string const& x)
    {
        out << x;
    }

    ostream& out()
    {
        return std::cout;
    }

    namespace
    {
        inline ostream& error_stream()
        {
            return std::clog;
        }
    }

#endif

    ostream& outerr()
    {
        return error_stream() << "Error: ";
    }

    ostream& outerr(fs::path const& file, int line)
    {
        if (line >= 0)
        {
            if (ms_errors)
                return error_stream() << path_to_stream(file) << "(" << line << "): error: ";
            else
                return error_stream() << path_to_stream(file) << ":" << line << ": error: ";
        }
        else
        {
            return error_stream() << path_to_stream(file) << ": error: ";
        }
    }

    ostream& outerr(file_ptr const& f, string_iterator pos)
    {
        return outerr(f->path, f->position_of(pos).line);
    }

    ostream& outwarn(fs::path const& file, int line)
    {
        if (line >= 0)
        {
            if (ms_errors)
                return error_stream() << path_to_stream(file) << "(" << line << "): warning: ";
            else
                return error_stream() << path_to_stream(file) << ":" << line << ": warning: ";
        }
        else
        {
            return error_stream() << path_to_stream(file) << ": warning: ";
        }
    }

    ostream& outwarn(file_ptr const& f, string_iterator pos)
    {
        return outwarn(f->path, f->position_of(pos).line);
    }
}}
