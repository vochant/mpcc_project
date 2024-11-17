#pragma once

#include <memory>
#include <string>

class Object {
public:
    enum class Type {
        Boolean, // [true, false]
        Null, // null
        Byte, // Single byte (uchar)
        Integer, // QWORD Integer (long long)
        Float, // Float64 (double)
        String, // UTF-8 String
        Array, // Array (using std::vector)
        Instance, // An instance of a class
        Executable, // [Function, NativeFunction]
        CommonObject, // Object
        Map, // Map (std::map)
        Set, // Set (std::set)
        Segment, // Same as std::deque
        Iterator, // Standard Iterator
        ByteArray, // ByteArray (vector<Byte>)
        File, // C++ File Stream (std::fstream)
        Native, // Native Type
        Reference, // Remote Reference
        LowReference, // Reference with C-style pointer (like "this")
        MemberFunc, // member function, can apply
        Mark // mark a class or enumerate
    } type;
public:
    Object(Type type);
    virtual std::shared_ptr<Object> make_copy() = 0;
    virtual std::string toString() = 0;
};