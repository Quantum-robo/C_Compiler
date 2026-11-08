#include "Layout.hpp"

static long alignUp(long v, long a) { return a > 1 ? (v + a - 1) / a * a : v; }

void LayoutTable::beginAggr(const std::string& name, bool isUnion, bool isClass) {
    if (aggrs.count(name)) return;           // a redefinition is reported by the parser
    AggrLayout a;
    a.name = name;
    a.isUnion = isUnion;
    a.isClass = isClass;
    aggrs[name] = a;
    order.push_back(name);
}

void LayoutTable::addField(const std::string& cls, const std::string& name, const Type& ty,
                           const std::vector<long>& dims) {
    std::map<std::string, AggrLayout>::iterator it = aggrs.find(cls);
    if (it == aggrs.end()) return;
    FieldInfo f;
    f.name = name;
    f.ty = ty;
    f.dims = dims;
    it->second.fields.push_back(f);
}

const AggrLayout* LayoutTable::find(const std::string& cls) const {
    std::map<std::string, AggrLayout>::const_iterator it = aggrs.find(cls);
    return it == aggrs.end() ? nullptr : &it->second;
}

const FieldInfo* LayoutTable::field(const std::string& cls, const std::string& name) const {
    const AggrLayout* a = find(cls);
    if (!a) return nullptr;
    for (size_t i = 0; i < a->fields.size(); ++i)
        if (a->fields[i].name == name) return &a->fields[i];
    return nullptr;
}

long LayoutTable::baseSize(const Type& t) const {
    if (t.aggr) {
        const AggrLayout* a = find(t.base);
        return a ? a->size : 0;               // 0: incomplete or unknown type
    }
    const std::string& b = t.base;
    if (b == "char" || b == "bool" || b == "void") return 1;
    if (b == "short") return 2;
    if (b == "double") return 8;
    return 4;                                 // int, long, float and "unknown"
}

long LayoutTable::baseAlign(const Type& t) const {
    if (t.aggr) {
        const AggrLayout* a = find(t.base);
        return a ? a->align : 1;
    }
    long s = baseSize(t);
    return s > 8 ? 8 : (s < 1 ? 1 : s);
}

long LayoutTable::objectSize(const Type& t, const std::vector<long>& dims) const {
    long n = elemSize(t);
    for (size_t i = 0; i < dims.size(); ++i) n *= dims[i];
    return n;
}

// Lay out one aggregate; fields of aggregate type are laid out first (they must be complete).
void LayoutTable::layoutOne(AggrLayout& a) {
    if (a.laidOut || a.inProgress) return;
    a.inProgress = true;
    long offset = 0, maxSize = 0, maxAlign = 1;
    for (size_t i = 0; i < a.fields.size(); ++i) {
        FieldInfo& f = a.fields[i];
        if (f.ty.aggr && f.ty.ptr == 0) {
            std::map<std::string, AggrLayout>::iterator it = aggrs.find(f.ty.base);
            if (it != aggrs.end()) layoutOne(it->second);
        }
        long al = elemAlign(f.ty);
        f.size = objectSize(f.ty, f.dims);
        if (a.isUnion) {
            f.offset = 0;
            if (f.size > maxSize) maxSize = f.size;
        } else {
            offset = alignUp(offset, al);
            f.offset = offset;
            offset += f.size;
        }
        if (al > maxAlign) maxAlign = al;
    }
    long raw = a.isUnion ? maxSize : offset;
    a.align = maxAlign;
    a.size = alignUp(raw, maxAlign);
    if (a.size < 1) a.size = 1;
    a.inProgress = false;
    a.laidOut = true;
}

void LayoutTable::print(FILE* out) const {
    for (size_t i = 0; i < order.size(); ++i) {
        const AggrLayout& a = aggrs.find(order[i])->second;
        fprintf(out, "# %s %s size=%ld align=%ld:", a.isUnion ? "union" : (a.isClass ? "class" : "struct"),
                a.name.c_str(), a.size, a.align);
        for (size_t j = 0; j < a.fields.size(); ++j)
            fprintf(out, " %s@%ld:%ld", a.fields[j].name.c_str(), a.fields[j].offset, a.fields[j].size);
        fprintf(out, "\n");
    }
}

void LayoutTable::finalize() {
    for (size_t i = 0; i < order.size(); ++i) layoutOne(aggrs[order[i]]);
}
