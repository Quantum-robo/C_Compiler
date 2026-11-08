#pragma once
// Memory layout of the language's types for the MIPS32 target.
//
//   char, bool      1 byte      short   2 bytes     int, long, float, pointers   4 bytes
//   double          8 bytes     struct / class      fields in declaration order, each aligned to its own
//                                                   alignment, total size rounded up to the largest alignment
//   union           every field at offset 0, size = largest field (rounded up to the alignment)
//
// The parser fills a LayoutTable while it reads struct / class / union bodies. Because a method may
// use members that are declared later in its class, the parser reads the program twice: the first
// pass builds the complete table, the second pass generates code with it.

#include <cstdio>
#include <string>
#include <vector>
#include <map>
#include "Types.hpp"

struct FieldInfo {
    std::string name;
    Type ty;                       // declared type (arr = number of array dimensions)
    std::vector<long> dims;        // array dimensions (empty when the field is not an array)
    long offset = 0;               // byte offset inside the object
    long size = 0;                 // total bytes occupied by the field
};

struct AggrLayout {
    std::string name;
    bool isUnion = false;
    bool isClass = false;
    std::vector<FieldInfo> fields;
    long size = 1;                 // never 0: an empty object still has an address
    long align = 1;
    bool laidOut = false;
    bool inProgress = false;
};

class LayoutTable {
public:
    void clear() { aggrs.clear(); order.clear(); }

    void beginAggr(const std::string& name, bool isUnion, bool isClass);
    void addField(const std::string& cls, const std::string& name, const Type& ty,
                  const std::vector<long>& dims);

    // Compute offsets and sizes of every aggregate (call once the table is complete).
    void finalize();

    const AggrLayout* find(const std::string& cls) const;
    const FieldInfo* field(const std::string& cls, const std::string& name) const;

    // Size / alignment of one element of the base type (pointer and array levels ignored).
    long baseSize(const Type& t) const;
    long baseAlign(const Type& t) const;

    // Size of one element of an array or pointee: pointers are 4 bytes, otherwise the base size.
    long elemSize(const Type& t) const { return t.ptr > 0 ? 4 : baseSize(t); }
    long elemAlign(const Type& t) const { return t.ptr > 0 ? 4 : baseAlign(t); }

    // Size of a value of this type once an array has decayed to a pointer.
    long valueSize(const Type& t) const { return t.arr > 0 ? 4 : elemSize(t); }

    // Bytes occupied by a variable of this type with these array dimensions.
    long objectSize(const Type& t, const std::vector<long>& dims) const;

    // One "# struct Name size=.. align=..: field@offset:size ..." line per aggregate.
    void print(FILE* out) const;

private:
    std::map<std::string, AggrLayout> aggrs;
    std::vector<std::string> order;
    void layoutOne(AggrLayout& a);
};
