# 🗃️ Pure C Generics: Dynamic Vector & Hashset Implementation

A low-level, high-efficiency systems programming project that implements fully generic, dynamic container data structures (`vector` and `hashset`) in pure, straight C. 

By avoiding the use of C++ templates, classes, or standard libraries, this project demonstrates deep manipulation of raw memory architectures. It operates purely on typeless pointers (`void *`), strict pointer arithmetic, and custom client-supplied function pointers (callbacks) to manage any data type—from primitive integers to complex custom structures and dynamic strings—with zero runtime space-bloat.

---

## 💡 Architectural Overview & Motivation

In low-resource or embedded environments (like systems software, kernels, or mobile device runtimes), C++ template instantiation can lead to heavy binary bloat. Every time a `std::vector<int>` or `std::vector<char*>` is declared, the compiler generates redundant type-specific code blocks. 

This implementation provides a **wafer-thin execution profile**. It reuses the exact same underlying assembly and functions for every single data type, making it significantly faster, light on memory, and highly maintainable.

---

## 🛠️ Core Components

### 1. The Generic Dynamic C Vector
The C `vector` functions as a highly flexible, contiguous raw memory array that mimics the scaling capabilities of high-level vectors.
* **Chunk-Based Allocation:** To maximize allocation efficiency, the vector expands in configurable chunks (`initialAllocation`) via `realloc` rather than resizing element-by-element.
* **Manual Byte Shifting:** Splicing elements in or out of arbitrary indexes (`VectorInsert` and `VectorDelete`) utilizes precision byte-shifting with `memmove` to safely handle overlapping memory zones.
* **Polymorphic Operations:** Integrates native ANSI C routines like `qsort` and `bsearch` alongside client-supplied comparison logic for real-time sorting and searching.

### 2. The Chained C Hashset
Built directly on top of the custom dynamic vector, the `hashset` provides fast bucket-indexed lookups and storage.
* **Collision Resolution via Chaining:** Resolves hash collisions elegantly by maintaining an array of custom C vectors as buckets.
* **Absolute Generic Freedom:** Leverages custom hashing functions (`HashSetHashFunction`) and mapping callbacks supplied by the client to index standalone, typeless payloads uniformly.

---

## ⚙️ Low-Level Memory Engineering Highlights

This engine is written in native procedural C and relies directly on the standard ANSI memory-management primitives:
* **`void *` Casting & Alignment:** Safely routing untyped references into aligned byte addresses across varying hardware constraints.
* **`memcpy` vs `memmove`:** Micro-optimizing buffer transfers by isolating independent memory slices with `memcpy`, and falling back to `memmove` only when overlapping memory boundaries demand safe data shifts.
* **Lifecycle Deallocation Callbacks:** Accepts dedicated element-freeing functions (`VectorFreeFunction`, `HashSetFreeFunction`) to ensure nested dynamic pointers inside client structures are cleanly torn down during memory disposal, preventing memory leaks.

---

## 📋 API Specification Preview

### Vector Interface
```c
void VectorNew(vector *v, int elemSize, VectorFreeFunction freefn, int initialAllocation);
void VectorDispose(vector *v);
int  VectorLength(const vector *v);
void *VectorNth(const vector *v, int position);
void VectorInsert(vector *v, const void *elemAddr, int position);
void VectorAppend(vector *v, const void *elemAddr);
void VectorReplace(vector *v, const void *elemAddr, int position);
void VectorDelete(vector *v, int position);
int  VectorSearch(const vector *v, const void *key, VectorCompareFunction searchfn, int startIndex, bool isSorted);
void VectorSort(vector *v, VectorCompareFunction comparefn);
void VectorMap(vector *v, VectorMapFunction mapfn, void *auxData);
