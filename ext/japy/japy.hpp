/* 

JAPY is a new JSON parser in C++.

This header file is the only thing you need, just drop it into you includes 
and you are ready to go. 

The MIT License (MIT)

Copyright (c) 2014 Artur Brugeman, brugeman.artur@gmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

#ifndef JAPY_JSON_PARSER_HPP
#define JAPY_JSON_PARSER_HPP

#include <cassert>
#include <cstring>

#include <limits>
#include <stack>
#include <stdexcept>
#include <string>
#include <vector>

/* 
   Japy's public interface consists of these:
   - path expressions that are plain strings (see README)
   - japy::parse method that you should use to parse a complete json document.
     - returns node_set_t
   - japy::node_set_t that represents a collection of nodes selected by a path.
     - use as a container to iterate over matching nodes
     - use >> to extract nodes one by one
   - japy::node_t that represents a single node
     - use >> to convert node value into c++ type
     - use [] and a sub-path to perform sub-queries (returns node_set_t)
   - japy::parser_t that you should use to parse streams of json data.
     - supply it with a 'scope-path' that is used to buffer matched nodes
     - use 'put()' to feed a block of data
     - use parser as a container to iterate over nodes that match the
       scope-path within current block
     - goto -2;

   Japy tries to minimize copying of input data, which means that data blocks
   you feed into it must live as long as they are fully parsed.

   See examples.cpp on how to use japy.

   Japy's internals are organized this way:
   - japy::detail::parser_t is a callback-based parser (SAX-style parser
     if you are XML guy), it is supplied with a visitor that receives all the
     'events' (start/end of input/object/array/name/value). Parser has internal
     buffer to store names and values that are at the borders of data blocks.
   - japy::detail::selector_t is a single selector of a path
   - japy::detail::path_t is a collection of selectors
   - japy::detail::matcher_t is a visitor who receives events of
     detail::parser_t and matches the json against the path_t, and
     calls a receiver when something is matched
   - japy::node_set_t and japy::parser_t both encapsulate detail::parser_t,
     detail::path_t, detail::matcher_t and an internal receiver for matched
     data. japy::parser_t differs from japy::node_set_t in that it must
     buffer the matched nodes if they are fed in parts. japy::node_set_t
     does not need a buffer as it only parses complete (sub-)trees of json.

 */

namespace japy
{

// a very simple pointer+size POD
// allows to address substrings within constant strings
struct string_t
{
    const char * data;
    size_t size;

    string_t () : data (0), size (0) {}

    string_t (const char * data, const size_t size)
        : data (data), size (size)
    {}

    string_t (const char * data)
        : data (data), size (strlen (data))
    {}

    string_t (const char & ch)
        : data (&ch), size (1)
    {}

    template<class Buffer>
    string_t (const Buffer & b)
        : data (b.data ())
        , size (b.size ())
    {}

    void clear ()
    {
        data = 0;
        size = 0;
    }

    bool empty () const
    {
        return !size || !data || !*data;
    }

    operator std::string () const
    {
        return empty () ? std::string () : std::string (data, size);
    }

};

// json 'node' types
enum node_type_t
{
    node_type_any,
    node_type_array,
    node_type_object,
    node_type_value,
    node_type_string,
    node_type_integral,
    node_type_float,
    node_type_boolean,
    node_type_null
};

// exceptions 

class bad_name_error_t : public std::logic_error
{
public:
    bad_name_error_t () : std::logic_error ("Bad member name") {}
};

class bad_type_error_t : public std::logic_error
{
public:
    bad_type_error_t () : std::logic_error ("Bad type") {}
};

class bad_conversion_error_t : public std::logic_error
{
public:
    bad_conversion_error_t () : std::logic_error ("Conversion failure") {}
};

class bad_selector_error_t : public std::logic_error
{
public:
    bad_selector_error_t () : std::logic_error ("Bad selector") {}
};

class bad_input_error_t : public std::runtime_error
{
public:
    bad_input_error_t () : std::runtime_error ("Bad json input") {}
};

class bad_state_error_t : public std::runtime_error
{
public:
    bad_state_error_t () : std::runtime_error ("Bad parser state") {}
};

class trailing_garbage_error_t : public std::runtime_error
{
public:
    trailing_garbage_error_t ()
        : std::runtime_error ("Trailing garbage at json") {}
};

class required_not_found_error_t : public std::runtime_error
{
public:
    required_not_found_error_t ()
        : std::runtime_error ("Required selector was not matched")
    {}
};

// string comparisons

inline bool 
operator== (const string_t & lhs, const string_t & rhs)
{
    if (lhs.size != rhs.size) return false;
    else if (!lhs.size) return true;
    else return !memcmp (lhs.data, rhs.data, lhs.size);
}

inline bool 
operator== (const string_t & lhs, const char * rhs)
{
    assert (rhs);
    return !strncmp (lhs.data, rhs, lhs.size);
}

inline bool 
operator== (const char * lhs, const string_t & rhs)
{
    assert (lhs);
    return !strncmp (lhs, rhs.data, rhs.size);
}

inline bool 
operator== (const string_t & lhs, const std::string & rhs)
{
    return !strncmp (lhs.data, rhs.c_str (), lhs.size);
}

inline bool 
operator== (const std::string & lhs, const string_t & rhs)
{
    return !strncmp (lhs.c_str (), rhs.data, rhs.size);
}

inline bool 
operator!= (const string_t & lhs, const string_t & rhs)
{
    return !operator== (lhs, rhs);
}

inline bool 
operator!= (const string_t & lhs, const char * rhs)
{
    return !operator== (lhs, rhs);
}

inline bool 
operator!= (const char * lhs, const string_t & rhs)
{
    return !operator== (lhs, rhs);
}

inline bool 
operator!= (const string_t & lhs, const std::string & rhs)
{
    return !operator== (lhs, rhs);
}

inline bool 
operator!= (const std::string & lhs, const string_t & rhs)
{
    return !operator== (lhs, rhs);
}

// implementation DETAILS
// search for 'PUBLIC' keyword below to skip details

namespace detail
{

struct selector_t
{
    bool recursive; // /
    bool inverse;   // !
    bool optional;  // ?
    node_type_t node_type; // *@$# or T: or type:
    string_t name;

    static string_t parse_identifier (const char *& s);
    static string_t parse_member_name (const char *& s);

    selector_t ();

    bool set_abbr_type (const char *& s);
    void set_type (const string_t & type);

    void clear ();
    const char * parse (const char * s);
};

struct path_t
{
    std::string path;
    typedef std::vector<selector_t> selectors_t;
    selectors_t selectors;

    path_t (const char * path);

    void clear ();
    selector_t & add ();
    selector_t & selector (const size_t index);
    void parse_selectors ();
    void parse (const char * str);

};

/* Receiver must implement:
      void receive_input_start (const char * start);
      void receive_input_end (const char * end);
      void receive_match_start (const char * start, node_type_t node_type);
      void receive_match_end (const char * end);
    */
template<class Receiver>
struct matcher_t
{
    // receiver to be notified about matches
    Receiver & receiver;

    // path to be matched
    const path_t & path;

    // path matching state
    size_t cursor; // current selector
    size_t depth;  // current depth of the tree
    size_t cursor_depth; // depth the cursor is pointing to
    bool name_match; // has the last member name matched

    // per-selector state
    struct selector_state_t
    {
        size_t depth;
        bool has_matched;
        bool name_match;

        selector_state_t ()
            : depth (0)
            , has_matched (false)
            , name_match (false)
        {}

        void clear ()
        {
            has_matched = false;
            name_match = false;
        }
    };
    typedef std::vector<selector_state_t> selector_states_t;
    selector_states_t selector_states;

    // debug
    void print ();

    // path size
    size_t size () const;

    // helpers to access current path parts
    const selector_t & selector () const;
    selector_state_t & selector_state ();

    // helpers for object and array processing
    void container_start (const char * start,
                          const node_type_t required_node_type);
    bool container_end (const char * end);

public:

    matcher_t (Receiver & receiver, const path_t & path);

    void visit_input_start (const char * start);
    void visit_object_start (const char * start);
    bool visit_object_end (const char * end);
    void visit_array_start (const char * start);
    bool visit_array_end (const char * end);
    void visit_member_name (const string_t & name);
    bool visit_value_ex (const string_t & value, const bool quoted);
    bool visit_quoted_value (const string_t & value);
    bool visit_value (const string_t & value);
    void visit_input_end (const char * end);
};

// will be helpful when optimizing to avoid heap usage
template<size_t static_buffer_size>
class buffer_t
{
    char static_buffer_[static_buffer_size];
    typedef std::vector<char> heap_buffer_t;
    heap_buffer_t heap_buffer_;

    bool is_heap () const { return !heap_buffer_.empty (); }

public:
    buffer_t () {}

    bool buffered_data (const char * ptr) const;
    char * data (const size_t min_size);
};

/* Visitor must implement:
      void visit_input_start (const char * start);
      void visit_object_start (const char * start);
      bool visit_object_end (const char * end);
      void visit_array_start (const char * start);
      bool visit_array_end (const char * end);
      void visit_member_name (const japy::string_t & name);
      bool visit_value (const japy::string_t & value);
      bool visit_quoted_value (const japy::string_t & value);
      void visit_input_end (const char * end);
      
    */
template<class Visitor>
class parser_t
{
    Visitor & visitor_;
    bool allow_many_roots_;

    const char * input_; // input start
    const char * end_;   // input end
    bool escape_; // flags that next char is escaped
    bool end_of_step_;

    string_t current_value_;
    static const size_t value_min_buffer_size = 64;
    buffer_t<value_min_buffer_size> value_buffer_;

    static bool stack_object () { return true; }
    static bool stack_array () { return false; }
    typedef std::stack<bool, std::vector<bool> > state_stack_t;
    state_stack_t stack_;

    typedef void (parser_t::*state_handler_t) ();
    state_handler_t state_handler_;

    // parser state handlers
    void process_root ();
    void process_body ();
    void process_attr_name ();
    void process_attr_colon_expected ();
    void process_attr_value_expected ();
    void process_value_quoted ();
    void process_value ();
    void process_comma_expected ();
    void process_root_end ();

    // parser helper methods
    bool is_object () const;
    void array_end ();
    void object_end ();
    void object_start ();
    void array_start ();
    void parse_quoted (const bool member);
    void skip_spaces ();

    // called on end of block to buffer member name or value
    // that is on the border
    void copy_to_buffers ();

public:

    parser_t (Visitor & visitor)
        : visitor_ (visitor)
        , allow_many_roots_ (false)
        , input_ (0)
        , end_ (0)
        , escape_ (false)
        , end_of_step_ (false)
        , state_handler_ (&parser_t::process_root)
    {}

    void allow_many_roots (const bool a) { allow_many_roots_ = a; }
    bool allow_many_roots () const { return allow_many_roots_; }

    // feed more data to parse, data must live until !more().
    // preconditions: !more()
    void put (const string_t & s);

    // parse until end of block or until visitor flags to stop
    // returns the result of more() after parsing.
    bool step ();

    // more data to be parsed in current block?
    bool more () const;
};

// supplementary classes to specialize operator>>

// raw bytes of attributes without decoding
template<class Container>
struct cast_raw_t
{
    Container & cont;
    cast_raw_t (Container & c) : cont (c) {}
};

// node's body bytes without any decodings
template<class Container>
struct cast_body_t
{
    Container & cont;
    cast_body_t (Container & c) : cont (c) {}
};

// proxy to make common specialized conversion for all containers
template<class Container>
struct container_proxy_t
{
    Container & cont;
    container_proxy_t (Container & c) : cont (c) {}
};

// definitions

// helper methods

inline bool
is_space (const char c)
{
    switch (c)
    {
    case ' ':
    case '\n':
    case '\t':
    case '\r':
        return true;
    }
    return false;
}

inline bool
is_digit (const char c)
{
    return c >= '0' && c <= '9';
}

inline void
skip_spaces (const char *& s)
{
    assert (s);
    for (; is_space (*s); ++s);
}

inline bool
take (const char *& s, const char c)
{
    assert (s);
    const bool match = *s == c;
    if (match) ++s;
    return match;
}

// selector

inline selector_t::selector_t ()
    : recursive (false)
    , inverse (false)
    , optional (false)
    , node_type (node_type_any)
{}

inline void
selector_t::clear ()
{
    recursive = false;
    inverse = false;
    optional = false;
    node_type = node_type_any;
    name.clear ();
}

inline void
selector_t::set_type (const string_t & s)
{
    assert (!s.empty ());
    struct
    {
        const char * name;
        node_type_t type;
    } types[] = {
    {"array", node_type_array},
    {"object", node_type_object},
    {"string", node_type_string},
    {"integral", node_type_integral},
    {"float", node_type_float},
    {"boolean", node_type_boolean},
    {"null", node_type_null},
    {"value", node_type_value},
    {"any", node_type_any},
    {0, node_type_any} // zero-terminator
};

    // yes, this is slow, and could be made much faster
    // but it's simple
    // use abbreviated type names to get the speed!
    auto * t = &types[0];
    for (; t->name; ++t)
    {
        const size_t len = strlen (t->name);
        if (len != s.size)
            continue;

        if (strncmp (s.data, t->name, s.size))
            continue;

        node_type = t->type;
        return;
    }

    throw bad_type_error_t ();
}

inline string_t
selector_t::parse_identifier (const char *& s)
{
    // only a-zA-Z0-9 and _- are  allowed
    const char * start = s;
    for (; *s; ++s)
    {
        if ((*s >= 'a' && *s <= 'z') ||
                (*s >= 'A' && *s <= 'Z') ||
                (*s >= '0' && *s <= '9') ||
                (*s == '_') ||
                (*s == '-'))
            continue;

        break;
    }

    assert (start);
    assert (s >= start);
    const size_t len = s - start;
    return string_t (start, len);
}

inline string_t
selector_t::parse_member_name (const char *& s)
{
    assert (s);
    if (!*s)
        return string_t ();

    assert (!is_space (*s));

    const bool quoted = *s == '"';
    const char * start = s;
    if (!quoted)
        return parse_identifier (s);

    // any chars allowed until we see "
    // note the \" handling
    ++s; // skip start quote
    start = s;
    bool escape = false;
    for (; *s; ++s)
    {
        if (!escape && *s == '"')
            break;
        escape ^= *s == '\\';
    }
    if (*s != '"')
        throw bad_name_error_t ();

    assert (start);
    assert (s >= start);
    const size_t len = s - start;
    ++s; // skip end quote
    return string_t (start, len);
}

inline bool
selector_t::set_abbr_type (const char *& s)
{
    assert (s);
    const char type = *s;
    if (!type)
        return false;

    const bool must_be_type = *(s + 1) == ':';
    switch (type)
    {
    case '*': node_type = node_type_any;
        break;

    case '@': node_type = node_type_value;
        break;

    case '#': node_type = node_type_array;
        break;

    case '$': node_type = node_type_object;
        break;

    default:
        if (!must_be_type)
            return false;

        switch (type)
        {
        case 'i': node_type = node_type_integral;
            break;

        case 'f': node_type = node_type_float;
            break;

        case 's': node_type = node_type_string;
            break;

        case 'b': node_type = node_type_boolean;
            break;

        case 'n': node_type = node_type_null;
            break;

        default:
            throw bad_type_error_t ();
        }
    }

    ++s;           // skip type abbr
    take (s, ':'); // skip type suffix
    return true;
}

inline const char *
selector_t::parse (const char * s)
{
    // reset
    clear ();

    // recursive if /
    skip_spaces (s);
    recursive = take (s, '/');

    // inverse if !
    skip_spaces (s);
    inverse = take (s, '!');

    // optional if ?
    skip_spaces (s);
    optional = take (s, '?');

    // type
    skip_spaces (s);
    if (!*s)
        return s;

    // check for type abbreviation
    const bool has_abbr_type = set_abbr_type (s);
    skip_spaces (s);

    // if no short type, see what we've got
    if (!has_abbr_type && *s != '"')
    {
        // identifier may be a type name, or member name
        const string_t ident (
                    parse_identifier (s)); // throws

        // type names are immediately followed by ':'
        if (*s == ':')
        {
            set_type (ident);
            ++s; // skip suffix
        }
        else
        {
            name = ident;
        }
        skip_spaces (s);
    }

    // ok, that was type, lets get the name
    if (*s && name.empty ())
    {
        name = parse_member_name (s); // throws
        skip_spaces (s);
    }

    // anything else?
    if (*s && *s != '/')
        throw bad_selector_error_t ();

    return s;
}

// path

inline path_t::path_t (const char * path)
{
    parse (path);
}

inline void
path_t::clear ()
{
    path.clear ();
    selectors.clear ();
}

inline selector_t &
path_t::add ()
{
    selectors.push_back (selector_t ());
    return selectors.back ();
}

inline selector_t &
path_t::selector (const size_t index)
{
    assert (index < selectors.size ());
    return selectors[index];
}

inline void
path_t::parse_selectors ()
{
    const char * str = path.c_str ();
    skip_spaces (str);

    if (*str != '/')
    {
        // default case - empty root selector
        add ();
    }
    else
    {
        // skip root marker '/' and add root selector
        ++str;
    }

    while (*str)
    {
        // add and parse selector
        str = add ().parse (str); // throws
        assert (str);
        if (*str == '/')
            ++str;  // skip selector separator
        else
            assert (!*str);
    }
}

inline void
path_t::parse (const char * str)
{
    assert (str);

    clear ();

    if (!str)
        return;

    path = str;
    parse_selectors ();
}

// buffer

template<size_t static_buffer_size>
inline bool
buffer_t<static_buffer_size>::buffered_data (const char * ptr) const
{
    return is_heap ()
            ? ptr == heap_buffer_.data ()
            : ptr == static_buffer_;
}

template<size_t static_buffer_size>
inline char *
buffer_t<static_buffer_size>::data (const size_t min_size)
{
    if (is_heap ())
    {
        // resize heap for min_size
        if (heap_buffer_.size () < min_size)
        {
            heap_buffer_.resize (min_size);
        }
        return heap_buffer_.data ();
    }
    else if (min_size <= static_buffer_size)
    {
        return static_buffer_;
    }
    else
    {
        // move from static to heap buffer
        heap_buffer_.resize (min_size);
        memcpy (heap_buffer_.data (), static_buffer_, static_buffer_size);
        return heap_buffer_.data ();
    }
}


// matcher

template<class Receiver>
inline
matcher_t<Receiver>::matcher_t (
        Receiver & receiver, const path_t & path)
    : receiver (receiver)
    , path (path)
    , cursor (0)
    , depth (0)
    , cursor_depth (0)
    , selector_states (path.selectors.size ())
{}

template<class Receiver>
inline size_t
matcher_t<Receiver>::size () const
{
    return path.selectors.size ();
}

template<class Receiver>
inline void
matcher_t<Receiver>::print ()
{
#ifdef JAPY_DEBUG
    printf ("------------\n");
    printf ("depth: %lu\n", depth);
    printf ("cursor: %lu\n", cursor);
    printf ("cursor_depth: %lu\n", cursor_depth);
    for (size_t i = 0; i < selector_states.size (); ++i)
    {
        printf (" s: %lu depth: %d has_matched: %d name_match: %d\n",
                i,
                selector_states[i].depth,
                selector_states[i].has_matched,
                selector_states[i].name_match);
    }
    printf ("------------\n\n");
#endif
}

template<class Receiver>
inline const selector_t &
matcher_t<Receiver>::selector () const
{
    assert (cursor < size ());
    return path.selectors[cursor];
}

template<class Receiver>
inline typename matcher_t<Receiver>::selector_state_t &
matcher_t<Receiver>::selector_state ()
{
    assert (cursor < size ());
    return selector_states[cursor];
}

template<class Receiver>
inline void
matcher_t<Receiver>::container_start (
        const char * start,
        const node_type_t node_type)
{
    assert (cursor <= size ());
    assert (depth >= cursor_depth);

    // no match as we are deeper than cursor
    if (depth++ > cursor_depth)
        return;

    // have we matched all selectors?
    if (cursor == size ())
        return;

    const selector_t & s = selector ();
    selector_state_t & state = selector_state ();

    // type match?
    bool type_match = s.node_type == node_type_any;
    type_match |= s.node_type == node_type;

    // has the object's name matched?
    const bool name_match = s.name.empty () || state.name_match;

    // type name name?
    bool full_match = type_match && name_match;

    // inverse the match
    if (s.inverse)
        full_match = !full_match;

    // result
    if (full_match)
    {
        // mark as matched
        state.has_matched = true;

        // move forward
        ++cursor;
        ++cursor_depth;

        // all matched?
        if (cursor == size ())
        {
            receiver.receive_match_start (start, node_type);
        }
        else
        {
            // reset next selector's state
            selector_state ().clear ();
        }
    }
    else if (s.recursive)
    {
        ++cursor_depth;
        ++selector_state ().depth;
    }
}

template<class Receiver>
inline bool
matcher_t<Receiver>::container_end (const char * end)
{
    assert (end && *end);
    assert (cursor <= size ());
    assert (depth >= cursor_depth);

    const bool depth_match = depth == cursor_depth;

    --depth;
    if (depth_match)
    {
        --cursor_depth;

        // end of matched object
        const bool all_matched = cursor == size ();

        // have we matched all selectors?
        if (all_matched)
        {
            --cursor;
            receiver.receive_match_end (end + 1);
            return true;
        }
        else
        {
            selector_state_t & state = selector_state ();
            if (state.depth)
            {
                --state.depth;
            }
            else
            {
                assert (cursor);
                if (!selector ().optional && !state.has_matched)
                    throw required_not_found_error_t ();
                --cursor;
            }
        }
    }
    return false;
}

template<class Receiver>
inline void
matcher_t<Receiver>::visit_input_start (const char * start)
{
    receiver.receive_input_start (start);
}

template<class Receiver>
inline void
matcher_t<Receiver>::visit_object_start (const char * start)
{
    assert (start && *start == '{');
    container_start (start, /* node_type= */node_type_object);
#ifdef JAPY_DEBUG
    printf ("object start: '%s'\n", std::string (start).c_str ());
    print ();
#endif 
}

template<class Receiver>
inline bool
matcher_t<Receiver>::visit_object_end (const char * end)
{
    assert (end && *end == '}');
    const bool r = container_end (end);
#ifdef JAPY_DEBUG
    printf ("object end: '%s'\n", end);
    print ();
#endif
    return r;
}

template<class Receiver>
inline void
matcher_t<Receiver>::visit_array_start (const char * start)
{
    assert (start && *start == '[');
    container_start (start, /* node_type= */node_type_array);
#ifdef JAPY_DEBUG
    printf ("array start: '%s'\n", start);
    print ();
#endif
}

template<class Receiver>
inline bool
matcher_t<Receiver>::visit_array_end (const char * end)
{
    assert (end && *end == ']');
    const bool r = container_end (end);
#ifdef JAPY_DEBUG
    printf ("array end: '%s'\n", end);
    print ();
#endif
    return r;
}

template<class Receiver>
inline void
matcher_t<Receiver>::visit_member_name (const string_t & name)
{
    assert (depth >= cursor_depth);
    assert (cursor <= size ());

    if (cursor < size () && !selector ().name.empty ())
        selector_state ().name_match = selector ().name == name;

#ifdef JAPY_DEBUG
    printf ("member: '%s'\n", std::string (name).c_str ());
    print ();
#endif
}

template<class Receiver>
inline bool
matcher_t<Receiver>::visit_value_ex (
        const string_t & value,
        const bool quoted)
{
    assert (depth >= cursor_depth);
    assert (cursor <= size ());

    // are we deeper than the cursor?
    if (depth > cursor_depth)
        return false;

    // have we matched all selectors?
    if (cursor == size ())
        return false;

    const selector_t & s = selector ();

    // FIXME these are ugly!
    auto is_integral = [] (const string_t & s)
    {
        // FIXME find a better way and remove copying
        try
        {
            std::stoll (s);
            return true;
        }
        catch (...)
        {
            try
            {
                std::stoul (s);
                return true;
            }
            catch (...)
            {}
        }
        return false;
    };

    auto is_float = [] (const string_t & s)
    {
        // FIXME find a better way and remove copying
        try
        {
            std::stold (s);
            return true;
        }
        catch (...)
        {}
        return false;
    };

    auto is_null = [] (const string_t & s)
    {
        return s == "null";
    };

    auto is_boolean = [] (const string_t & s)
    {
        return s == "true" || s == "false";
    };

    auto guess_node_type = [&] (const string_t &)
    {
        if (is_null (value))
            return node_type_null;
        else if (is_boolean (value))
            return node_type_boolean;
        else if (is_integral (value))
            return node_type_integral;
        else if (is_float (value))
            return node_type_float;
        else
            return node_type_string;
    };

    // so what's the type?
    node_type_t value_node_type = node_type_value;
    if (quoted || value.empty ())
        value_node_type = node_type_string;
    else
        value_node_type = guess_node_type (value);
    assert (value_node_type != node_type_value);

    // type matches?
    bool type_match = s.node_type == node_type_value;
    type_match |= s.node_type == node_type_any;
    type_match |= s.node_type == value_node_type;

    // name matches?
    const bool name_match = s.name.empty () || selector_state ().name_match;

    // full match?
    bool full_match = name_match && type_match;
    if (s.inverse)
        full_match = !full_match;

    // result
    if (full_match)
    {
        // mark as matched
        selector_state ().has_matched = true;

        if (cursor == (size () - 1))
        {
            receiver.receive_match_start (value.data, value_node_type);
            receiver.receive_match_end (value.data + value.size);

            return true;
        }
        // else FIXME makes no sense for a child selector on value
    }

#ifdef JAPY_DEBUG
    printf ("value [%d]: '%s'\n", quoted, std::string (value).c_str ());
    print ();
#endif

    return false;
}

template<class Receiver>
inline bool
matcher_t<Receiver>::visit_quoted_value (const string_t & value)
{
    return visit_value_ex (value, /* quoted= */true);
}

template<class Receiver>
inline bool
matcher_t<Receiver>::visit_value (const string_t & value)
{
    return visit_value_ex (value, /* quoted= */false);
}

template<class Receiver>
inline void
matcher_t<Receiver>::visit_input_end (const char * end)
{
    receiver.receive_input_end (end);
}

// parser

template<class Visitor>
inline void
parser_t<Visitor>::skip_spaces ()
{
    assert (input_ <= end_);
    for (; input_ != end_ && is_space (*input_); ++input_);
}

template<class Visitor>
inline bool
parser_t<Visitor>::is_object () const
{
    assert (!stack_.empty ());
    return stack_.top () == stack_object ();
}

template<class Visitor>
inline void
parser_t<Visitor>::array_end ()
{
    assert (more () && *input_ == ']');

    if (stack_.empty () || is_object ())
        throw bad_input_error_t ();

    end_of_step_ = visitor_.visit_array_end (input_);

    state_handler_ = &parser_t::process_comma_expected;
    stack_.pop ();
    if (stack_.empty ())
    {
        state_handler_ = allow_many_roots_
                ? &parser_t::process_root
                : &parser_t::process_root_end;
    }

    ++input_;
    skip_spaces ();
}

template<class Visitor>
inline void
parser_t<Visitor>::object_end ()
{
    assert (more () && *input_ == '}');

    if (stack_.empty () || !is_object ())
        throw bad_input_error_t ();

    end_of_step_ = visitor_.visit_object_end (input_);

    state_handler_ = &parser_t::process_comma_expected;
    stack_.pop ();
    if (stack_.empty ())
    {
        state_handler_ = allow_many_roots_
                ? &parser_t::process_root
                : &parser_t::process_root_end;
    }

    ++input_;
    skip_spaces ();
}

template<class Visitor>
inline void
parser_t<Visitor>::object_start ()
{
    assert (more () && *input_ == '{');

    state_handler_ = &parser_t::process_body;
    stack_.push (stack_object ());
    visitor_.visit_object_start (input_);

    ++input_;
    skip_spaces ();
}

template<class Visitor>
inline void
parser_t<Visitor>::array_start ()
{
    assert (more () && *input_ == '[');

    state_handler_ = &parser_t::process_body;
    stack_.push (stack_array ());
    visitor_.visit_array_start (input_);

    ++input_;
    skip_spaces ();
}

template<class Visitor>
inline void
parser_t<Visitor>::process_root ()
{
    assert (more ());

    skip_spaces ();

    if (!more ())
        return;

    switch (*input_)
    {
    case '{':
        object_start ();
        break;

    case '[':
        array_start ();
        break;
    }
}

template<class Visitor>
inline void
parser_t<Visitor>::process_body ()
{
    assert (more ());

    skip_spaces ();

    if (!more ())
        return;

    switch (*input_)
    {
    case '{':
        if (stack_.empty () || is_object ())
            throw bad_input_error_t ();

        object_start ();
        break;

    case '[':
        if (stack_.empty () || is_object ())
            throw bad_input_error_t ();

        array_start ();
        break;

    case '}':
        object_end ();
        break;

    case ']':
        array_end ();
        break;

    case '"':
        if (stack_.empty ())
            throw bad_input_error_t ();

        if (is_object ())
            state_handler_ = &parser_t::process_attr_name;
        else
            state_handler_ = &parser_t::process_value_quoted;

        ++input_;
        break;

    default:
        assert (!is_space (*input_));

        if (stack_.empty () || is_object ())
            throw bad_input_error_t ();

        state_handler_ = &parser_t::process_value;
        // we don't eat the input char, process_value will use it
        break;
    }
}

template<class Visitor>
inline void
parser_t<Visitor>::parse_quoted (const bool member)
{
    assert (more ());

    if (current_value_.empty ())
    {
        current_value_.data = input_;
        assert (!current_value_.size);
    }

    const char * from = input_;
    bool quoted_end = false;
    for (; more () && !(quoted_end = *input_ == '"' && !escape_);
         ++input_)
        escape_ ^= *input_ == '\\';

    const size_t dist = input_ - from;
    if (dist && value_buffer_.buffered_data (current_value_.data))
    {
        char * buffered = value_buffer_.data (current_value_.size + dist);
        memcpy (buffered + current_value_.size, from, dist);
        current_value_.data = buffered;
    }
    current_value_.size += dist;

    if (quoted_end)
    {
        if (member)
        {
            state_handler_ = &parser_t::process_attr_colon_expected;
            visitor_.visit_member_name (current_value_);
        }
        else
        {
            state_handler_ = &parser_t::process_comma_expected;
            end_of_step_ = visitor_.visit_quoted_value (current_value_);
        }

        current_value_.clear ();

        ++input_; // skip quote
        skip_spaces ();
    }
}

template<class Visitor>
inline void
parser_t<Visitor>::process_attr_name ()
{
    parse_quoted (/* member= */true);
}

template<class Visitor>
inline void
parser_t<Visitor>::process_attr_colon_expected ()
{
    assert (more ());

    skip_spaces ();

    if (!more ())
        return;

    switch (*input_)
    {
    case ':':
        state_handler_ = &parser_t::process_attr_value_expected;
        ++input_;
        break;

    default:
        assert (!is_space (*input_));
        throw bad_input_error_t ();
    }

    skip_spaces ();
}

template<class Visitor>
inline void
parser_t<Visitor>::process_attr_value_expected ()
{
    assert (more ());

    skip_spaces ();

    if (!more ())
        return;

    switch (*input_)
    {
    case '{':
        object_start ();
        break;

    case '[':
        array_start ();
        break;

    case '"':
        state_handler_ = &parser_t::process_value_quoted;
        ++input_;
        break;

    default:
        assert (!is_space (*input_));

        state_handler_ = &parser_t::process_value;
        // we don't eat input char, process_value will use it

        break;
    }
}

template<class Visitor>
inline void
parser_t<Visitor>::process_value_quoted ()
{
    parse_quoted (/* member= */false);
}

template<class Visitor>
inline void
parser_t<Visitor>::process_value ()
{
    assert (more ());

    switch (*input_)
    {
    case '}':
        if (current_value_.empty ())
            throw bad_input_error_t ();

        end_of_step_ = visitor_.visit_value (current_value_);
        current_value_.clear ();
        state_handler_ = &parser_t::process_body;
        // don't eat the }, process_body will use it
        break;

    case ']':
        if (current_value_.empty ())
            throw bad_input_error_t ();

        end_of_step_ = visitor_.visit_value (current_value_);
        current_value_.clear ();
        state_handler_ = &parser_t::process_body;
        // don't eat the ], process_body will use it
        break;

    case ',':
        if (current_value_.empty ())
            throw bad_input_error_t ();

        state_handler_ = &parser_t::process_body;
        end_of_step_ = visitor_.visit_value (current_value_);
        current_value_.clear ();

        ++input_;
        skip_spaces ();
        break;

    default:
        if (current_value_.empty ())
        {
            assert (!is_space (*input_));
            current_value_.data = input_;
            current_value_.size = 1;
            ++input_;
        }

        auto is_non_quoted_value_char = [] (const char c)
        {
            switch (c)
            {
            case 't': // true
            case 'r':
            case 'u':
            case 'e':
            case 'f': // false
            case 'a':
            case 'l':
            case 's':
            case 'n': // null
            case '-': // number (-0-9.0-9eE+)
            case '+':
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
            case '.':
            case 'E':
                return true;
            }
            return false;
        };

        const char * from = input_;
        for (; more () && is_non_quoted_value_char (*input_); ++input_);

        const size_t dist = input_ - from;
        if (dist && value_buffer_.buffered_data (current_value_.data))
        {
            char * buffer =
                    value_buffer_.data (current_value_.size + dist);
            memcpy (buffer + current_value_.size, from, dist);
            current_value_.data = buffer;
        }
        current_value_.size += dist;

        if (more ())
        {
            state_handler_ = &parser_t::process_comma_expected;
            end_of_step_ = visitor_.visit_value (current_value_);
            current_value_.clear ();

            skip_spaces ();
        }
    }
}

template<class Visitor>
inline void
parser_t<Visitor>::process_comma_expected ()
{
    assert (more ());

    skip_spaces ();

    if (!more ())
        return;

    switch (*input_)
    {
    case ',':
        state_handler_ = &parser_t::process_body;
        ++input_;
        skip_spaces ();
        break;

    case '}':
        object_end ();
        break;

    case ']':
        array_end ();
        break;

    default:
        assert (!is_space (*input_));
        throw bad_input_error_t ();
    }
}

template<class Visitor>
inline void
parser_t<Visitor>::process_root_end ()
{
    assert (more ());

    skip_spaces ();

    if (more ())
        throw trailing_garbage_error_t ();
}

template<class Visitor>
inline void
parser_t<Visitor>::copy_to_buffers ()
{
    // nothing to copy
    if (!current_value_.data)
        return;

    // already in buffer
    if (value_buffer_.buffered_data (current_value_.data))
        return;

    // copy
    char * buffer = value_buffer_.data (current_value_.size);
    memcpy (buffer, current_value_.data, current_value_.size);
    current_value_.data = buffer;
}

template<class Visitor>
inline bool
parser_t<Visitor>::step ()
{
    assert (more ());

    end_of_step_ = false;
    while (more () && !end_of_step_)
    {
        assert (state_handler_);
        (this->*state_handler_) ();
    }

    if (!more ())
    {
        assert (input_ == end_);
        copy_to_buffers ();
        visitor_.visit_input_end (input_);
    }

    return more ();
}

template<class Visitor>
inline bool
parser_t<Visitor>::more () const
{
    return input_ != end_;
}

template<class Visitor>
inline void
parser_t<Visitor>::put (const string_t & s)
{
    if (more ())
        throw bad_state_error_t ();

    input_ = s.data;
    end_ = s.data + s.size;
    visitor_.visit_input_start (input_);
}

template<class Receiver>
inline matcher_t<Receiver>
make_matcher (Receiver & receiver, const path_t & path)
{
    return matcher_t<Receiver> (receiver, path);
}

template<class Visitor>
inline parser_t<Visitor>
make_parser (Visitor & visitor)
{
    return parser_t<Visitor> (visitor);
}

template<class Container>
inline detail::container_proxy_t<Container>
container_proxy (Container & c)
{
    return detail::container_proxy_t<Container> (c);
}

// FIXME using std::string is not efficient
// as we have our string_t as input and are forced to copy here
template<typename Numeric, typename Convertor>
inline Numeric
convert_numeric (const std::string & s, Convertor conv)
{
    typedef std::numeric_limits<Numeric> numeric;
    // auto bcs Numeric may be narrower what conv returns
    auto value = conv (s);
    if (value > numeric::max () || value < numeric::min ())
        throw std::out_of_range ("Numeric value is out of range");

    return static_cast<Numeric> (value);
}

}; // namespace detail


// PUBLIC interface 

// node

class node_set_t;

class node_t
{
    node_type_t type_;
    string_t body_;

public:
    node_t (const node_type_t type, const string_t & body)
        : type_ (type), body_ (body)
    {}

    node_type_t type () const { return type_; }
    const string_t & body () const { return body_; }

    node_set_t operator[] (const char * path);

};

// conversions work as follows (and your custom ones should also):
// - throw if (node_type == node_type_object || node_type == node_type_array)
// - if node body is empty - t is not written
// - node body is interpreted as Target 
// - if body may not represent target type - t is not written, no exceptions
// - if assignment to t fails (out of range, etc) - exception is thrown
template<typename Target>
const node_t & operator>> (const node_t & node, Target & t);

// node iterator

template<class Cont>
class node_input_iterator_t : 
        public std::iterator<std::input_iterator_tag, node_t>
{
    Cont * stream_;
public:
    node_input_iterator_t () : stream_ (0) {}

    node_input_iterator_t (Cont & stream)
        : stream_ (&stream)
    {
        if (!stream_->peek ())
            operator++ (); // make it dereferenceable immediately
    }

    node_input_iterator_t<Cont> & operator++ ()
    {
        assert (stream_);
        if (!stream_->next ())
            stream_ = 0;
        return *this;
    }

    node_input_iterator_t<Cont> operator++ (int)
    {
        node_input_iterator_t<Cont> tmp (*this);
        operator++ ();
        return tmp;
    }

    bool operator== (const node_input_iterator_t<Cont> & other) const
    {
        return stream_ == other.stream_;
    }

    bool operator!= (const node_input_iterator_t<Cont> & other) const
    {
        return !(*this == other);
    }

    node_t operator* () const
    {
        assert (stream_);
        return stream_->value ();
    }
};

template<class Cont>
inline node_input_iterator_t<Cont> 
begin (Cont & input)
{
    return node_input_iterator_t<Cont> (input);
}

template<class Cont>
inline node_input_iterator_t<Cont> 
end (const Cont & input)
{
    (void)input;
    return node_input_iterator_t<Cont> ();
}

// node_set

class node_set_t
{
    mutable struct receiver_t
    {
        string_t body;
        node_type_t node_type;

        receiver_t () : node_type (node_type_any) {}
        void clear ()
        {
            body.clear ();
            node_type = node_type_any;
        }

        void receive_input_start (const char * start);
        void receive_input_end (const char * end);
        void receive_match_start (const char * start, node_type_t node_type);
        void receive_match_end (const char * end);

    } receiver_;

    typedef detail::path_t path_t;
    path_t path_;
    typedef detail::matcher_t<receiver_t> matcher_t;
    matcher_t matcher_;
    typedef detail::parser_t<matcher_t> parser_t;
    mutable parser_t parser_;

public:

    node_set_t (const char * path, const string_t & body);

    bool next () const;
    bool peek () const;
    node_t value () const;

    node_input_iterator_t<node_set_t> begin ()
    {
        return japy::begin (*this);
    }

    node_input_iterator_t<node_set_t> end ()
    {
        return japy::end (*this);
    }
};

// conversion works as follows:
// - if !next () - t is not written
// - otherwise, value () >> t is called
template<typename Target>
const node_set_t & operator>> (const node_set_t & node_set, Target & t);

class parser_t
{
    struct receiver_t
    {
        static const size_t min_buffer_size = 1024;
        detail::buffer_t<min_buffer_size> buffer;

        const char * last_start;
        node_type_t node_type;
        string_t body;

        receiver_t () : last_start (0), node_type (node_type_any) {}
        void clear ()
        {
            node_type = node_type_any;
            body.clear ();
        }

        void receive_input_start (const char * start);
        void receive_input_end (const char * end);
        void receive_match_start (const char * start, node_type_t node_type);
        void receive_match_end (const char * end);
    } receiver_;

    typedef detail::path_t path_t;
    path_t path_;
    typedef detail::matcher_t<receiver_t> matcher_t;
    matcher_t matcher_;
    detail::parser_t<matcher_t> parser_;

public:

    parser_t (const char * path);

    void allow_many_roots (const bool a) { parser_.allow_many_roots (a); }
    bool allow_many_roots () const { return parser_.allow_many_roots (); }

    void put (const string_t & str);

    // FIXME implement this
    // void skip ();

    bool next ();
    bool peek () const;
    node_t value () const;

    node_input_iterator_t<parser_t> begin ()
    {
        return japy::begin (*this);
    }

    node_input_iterator_t<parser_t> end ()
    {
        return japy::end (*this);
    }

};

// conversion works as follows:
// - if !next () - t is not written
// - otherwise, 'value () >> t' is called
template<typename Target>
parser_t & operator>> (parser_t & parser, Target & t);

// utf16 -> utf8 codepoint conversion
// made public to be usable in your custom convertors

class utf16_t
{
    static const size_t codepoint_len = 4;
    static const size_t escape_len = 2;
    static const size_t escaped_codepoint_len = codepoint_len + escape_len;

    const char * str_;
    const char * const end_;

    utf16_t (const char * str, const char * const end)
        : str_ (str), end_ (end)
    {
        assert (str_ < end_);
    }

    bool is_escaped () const
    {
        assert (tail_len () >= escape_len);
        return *str_ == '\\' && *(str_ + 1) == 'u';
    }

    size_t tail_len () const
    {
        assert (str_ <= end_);
        return end_ - str_;
    }

    bool is_codepoint () const;
    long take_codepoint ();

    template<class Container>
    const char * to_utf8 (Container & cont);

public: 

    template<class Container>
    static const char *
    to_utf8 (Container & c, const char * s, const char * const end)
    {
        return utf16_t (s, end).to_utf8 (c);
    }

    template<class Container>
    static string_t
    to_utf8 (Container & c, const string_t & s)
    {
        if (s.empty ()) return s;
        const char * const end = s.data + s.size;
        const char * str = utf16_t (s.data, end).to_utf8 (c);
        assert (end >= str);
        return string_t (str, end - str);
    }

};

// definitions

// node

inline node_set_t
node_t::operator[] (const char * path)
{
    return node_set_t (path, body_);
}

// node_set

inline 
node_set_t::node_set_t (const char * path, const string_t & body)
    : path_ (path)
    , matcher_ (receiver_, path_)
    , parser_ (matcher_)
{
    parser_.put (body);
}

inline void 
node_set_t::receiver_t::receive_input_start (const char * start)
{
    assert (!body.data);
    (void)start;
}

inline void 
node_set_t::receiver_t::receive_input_end (const char * end)
{
    // noop
    (void)end;
}

inline void 
node_set_t::receiver_t::receive_match_start (
        const char * start,
        const node_type_t node_type)
{
    assert (!body.data);
    assert (start && *start);
    assert (node_type != node_type_any);
    assert (node_type != node_type_value);
    body.data = start;
    this->node_type = node_type;
}

inline void 
node_set_t::receiver_t::receive_match_end (const char * end)
{
    assert (body.data);
    assert (node_type != node_type_any);
    assert (node_type != node_type_value);
    assert (end);
    assert (end >= body.data);
    body.size = end - body.data;
}

inline bool 
node_set_t::next () const
{
    receiver_.clear ();
    if (parser_.more ())
        parser_.step ();
    return peek ();
}

inline bool 
node_set_t::peek () const
{
    return receiver_.node_type != node_type_any;
}

inline node_t 
node_set_t::value () const
{
    assert (peek ());
    return node_t (receiver_.node_type, receiver_.body);
}

// parser

inline 
parser_t::parser_t (const char * path)
    : path_ (path)
    , matcher_ (receiver_, path_)
    , parser_ (matcher_)
{}

inline void 
parser_t::receiver_t::receive_input_start (const char * start)
{
    assert (start && *start);
    last_start = start;
}

inline void 
parser_t::receiver_t::receive_input_end (const char * end)
{
    assert (end);

    if (!body.data)
        return;

    const char * from = 0;
    if (buffer.buffered_data (body.data))
    {
        from = last_start;
    }
    else
    {
        // means that match has just finished, but was not yet
        // read by the client
        if (body.size)
            return;

        from = body.data;
    }

    assert (end >= from);
    const size_t len = end - from;
    char * buffered = buffer.data (body.size + len);
    memcpy (buffered + body.size, from, len);
    body.data = buffered;
    body.size += len;
}

inline void 
parser_t::receiver_t::receive_match_start (
        const char * start,
        const node_type_t node_type)
{
    assert (start && *start);
    assert (node_type != node_type_any);
    assert (node_type != node_type_value);

    assert (!body.data);
    assert (!body.size);
    assert (this->node_type == node_type_any);

    body.data = start;
    this->node_type = node_type;
}

inline void 
parser_t::receiver_t::receive_match_end (const char * end)
{
    assert (end);

    assert (body.data);
    assert (node_type != node_type_any);

    if (buffer.buffered_data (body.data))
    {
        // copy tail to buffer
        assert (last_start);
        assert (end >= last_start);
        const size_t len = end - last_start;
        char * buffered = buffer.data (body.size + len);
        memcpy (buffered + body.size, last_start, len);
        body.data = buffered;
        body.size += len;
    }
    else
    {
        assert (!body.size);
        assert (end >= body.data);
        body.size = end - body.data;
    }
}

inline void 
parser_t::put (const string_t & str)
{
    assert (!parser_.more ()); // FIXME throw
    parser_.put (str);
}

inline bool 
parser_t::next ()
{
    receiver_.clear ();
    if (parser_.more ())
        parser_.step ();

    return peek ();
}

inline bool 
parser_t::peek () const
{
    return receiver_.node_type != node_type_any;
}

inline node_t 
parser_t::value () const
{
    assert (peek ());
    return node_t (receiver_.node_type,
                   receiver_.body);
}

// utf16_t
inline long 
utf16_t::take_codepoint ()
{
    if (tail_len () < codepoint_len)
        throw bad_input_error_t ();

    char buffer[codepoint_len + 1] = {};
    memcpy (buffer, str_, codepoint_len);

    char * eptr = 0;
    const long codepoint = strtol (buffer, &eptr, /* base= */16);
    if ((eptr && *eptr) || codepoint < 0)
        throw bad_input_error_t ();

    str_ += codepoint_len;
    assert (codepoint <= 0xFFFF);
    return codepoint;
}

inline bool 
utf16_t::is_codepoint () const
{
    if (tail_len () < escaped_codepoint_len)
        return false;

    if (!is_escaped ())
        return false;

    const char * s = str_ + escape_len;
    for (size_t i = 0; s != end_ && i < codepoint_len; ++s, ++i)
    {
        if ((*s >= '0' && *s <= '9')
                || (*s >= 'a' && *s <= 'f')
                || (*s >= 'A' && *s <= 'F'))
            continue;

        return false;
    }

    return true;
}

template<class Container>
inline const char * 
utf16_t::to_utf8 (Container & cont)
{
    if (!is_codepoint ())
        return str_;

    str_ += escape_len;
    long codepoint = take_codepoint (); // throws
    const char * result = str_;

    const bool is_lead = codepoint >= 0xD800 && codepoint <= 0xDBFF;
    if (is_lead && is_codepoint ())
    {
        str_ += escape_len;
        const long tail_codepoint = take_codepoint (); // throws
        const bool is_tail =
                tail_codepoint >= 0xDC00 && codepoint <= 0xDFFF;
        if (is_tail)
        {
            const long lead_10_bits = codepoint - 0xD800;
            const long tail_10_bits = tail_codepoint - 0xDC00;
            codepoint = 0x10000 +
                    ((lead_10_bits << 10) & 0xFFC00) +
                    ((tail_10_bits) & 0x3FF);
            result = str_;
        }
    }

    if (codepoint < 0x80)
    {
        cont.push_back (static_cast<char> (codepoint));
    }
    else if (codepoint < 0x0800)
    {
        cont.push_back (0xC0 + ((codepoint >> 6) & 0x1F));
        cont.push_back (0x80 + (codepoint & 0x3F));
    }
    else if (codepoint < 0x10000)
    {
        cont.push_back (0xE0 + ((codepoint >> 12) & 0x0F));
        cont.push_back (0x80 + ((codepoint >> 6) & 0x3F));
        cont.push_back (0x80 + (codepoint & 0x3F));
    }
    else if (codepoint < 0x10FFFF)
    {
        cont.push_back (0xF0 + ((codepoint >> 18) & 0x07));
        cont.push_back (0x80 + ((codepoint >> 12) & 0x3F));
        cont.push_back (0x80 + ((codepoint >> 6) & 0x3F));
        cont.push_back (0x80 + (codepoint & 0x3F));
    }
    else
        throw bad_input_error_t ();

    return result;
}

// conversions

// works for numeric types only
template<typename Target>
inline const node_t & 
operator>> (const node_t & node, Target & t)
{
    typedef std::numeric_limits<Target> numeric;
    static_assert (numeric::is_specialized,
                   "Only built-in numeric types are supported");

    if (node.type () == node_type_array || node.type () == node_type_object)
        throw bad_conversion_error_t ();

    if (node.body ().empty ())
        return node;

    try
    {
        // lambdas are here bcs stold have overloads
        // and are ambiguos for deduction of template parameter
        if (!numeric::is_integer)
            t = detail::convert_numeric<Target> (
                        node.body (),
                        [] (const std::string & s) { return std::stold (s);});
        else if (numeric::is_signed)
            t = detail::convert_numeric<Target> (
                        node.body (),
                        [] (const std::string & s) { return std::stoll (s);});
        else
            t = detail::convert_numeric<Target> (
                        node.body (),
                        [] (const std::string & s) { return std::stoull (s);});
    }
    catch (std::invalid_argument)
    {} // ignore conversion failures

    return node;
}

// string and vector specializations

template<class Container>
inline const node_t & 
operator>> (const node_t & node, detail::container_proxy_t<Container> dest)
{
    if (node.body ().empty ())
        return node;

    // should be a value
    if (node.type () == node_type_array || node.type () == node_type_object)
        throw bad_conversion_error_t ();

    // non-strings may not contain encoded chars
    if (node.type () != node_type_string)
    {
        dest.cont.assign (node.body ().data,
                          node.body ().data + node.body ().size);
        return node;
    }

    // decode escaped sequences

    bool escaped = false;
    const char * const end = node.body ().data + node.body ().size;
    for (const char * c = node.body ().data;
         c != end;)
    {
        if (escaped)
        {
            const char e = *c++;
            switch (e)
            {
            case 'b': dest.cont.push_back ('\b'); break;
            case 'f': dest.cont.push_back ('\f'); break;
            case 'n': dest.cont.push_back ('\n'); break;
            case 'r': dest.cont.push_back ('\r'); break;
            case 't': dest.cont.push_back ('\t'); break;

            default:
                // default is to simply append escaped char
                dest.cont.push_back (e);
            }
            escaped = false;
            continue;
        }

        const char * next = utf16_t::to_utf8 (dest.cont, c, end); // throws
        assert (next >= c && next <= end);
        if (next > c)
        {
            c = next;
        }
        else if (*c != '\\')
        {
            dest.cont.push_back (*c);
            ++c;
        }
        else
        {
            assert (*c == '\\');
            escaped = true;
            ++c;
        }
    }

    return node;
}

inline const node_t & 
operator>> (const node_t & node, std::string & s)
{
    return node >> detail::container_proxy (s);
}

inline const node_t & 
operator>> (const node_t & node, std::vector<char> & s)
{
    return node >> detail::container_proxy (s);
}

inline const node_t & 
operator>> (const node_t & node, std::deque<char> & s)
{
    return node >> detail::container_proxy (s);
}

// custom conversions
template<class Container>
inline detail::cast_raw_t<Container> 
raw (Container & c)
{
    return detail::cast_raw_t<Container> (c);
}

template<class Container>
inline const node_t & 
operator>> (const node_t & node, detail::cast_raw_t<Container> r)
{
    if (node.type () == node_type_array || node.type () == node_type_object)
        throw bad_conversion_error_t ();

    r.cont.assign (node.body ().data, node.body ().size);
    return node;
}

template<class Container>
inline detail::cast_body_t<Container> 
body (Container & c)
{
    return detail::cast_body_t<Container> (c);
}

template<class Container>
inline const node_t & 
operator>> (const node_t & node, detail::cast_body_t<Container> b)
{
    b.cont.assign (node.body ().data, node.body ().size);
    return node;
}

// node_set conversion

template<typename Target>
inline const node_set_t & 
operator>> (const node_set_t & node_set, Target & t)
{
    if (node_set.next ())
        node_set.value () >> t;
    return node_set;
}

template<typename Container>
inline const node_set_t & 
operator>> (const node_set_t & node_set, detail::cast_raw_t<Container> t)
{
    if (node_set.next ())
        node_set.value () >> t;
    return node_set;
}

template<typename Container>
inline const node_set_t & 
operator>> (const node_set_t & node_set, detail::cast_body_t<Container> t)
{
    if (node_set.next ())
        node_set.value () >> t;
    return node_set;
}

// parser conversion

template<typename Target>
inline parser_t & 
operator>> (parser_t & parser, Target & t)
{
    if (parser.next ())
        parser.value () >> t;
    return parser;
}

// SIMPLE interface

inline node_set_t
parse (const string_t & json, const char * path)
{
    return node_set_t (path, json);
}

}; // namespace japy

#endif // JAPY_JSON_PARSER_HPP
