#pragma once

class ObjectBase {
public:
    enum Type {
        Error, // An error
        Boolean, // [true, false]
        Null, // null
        Byte, // Single byte (uchar)
        Int8, // BYTE Integer (char)
        Int16, // WORD Integer
        Int32, // DWORD Integer
        Integer, // QWORD Integer (long long)
        Float32, // Float32 (float)
        Float, // Float64 (double)
        String, // UTF-8 String
        Array, // Array (using std::vector)
        Instance, // An instance of a class
        Executable, // [Function, NativeFunction]
        Object, // Object
        Map, // Map (std::map)
        Set, // Set (std::set)
        Segment, // Same as std::deque
        Iterator, // Standard Iterator
        ByteArray, // ByteArray (vector<Byte>)
        File, // C++ File Stream (std::fstream)
        Native, // Native Type
    };
};