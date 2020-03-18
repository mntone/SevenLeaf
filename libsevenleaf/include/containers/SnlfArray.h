#ifndef _SNLF_CONTAINERS_ARRAY_H
#define _SNLF_CONTAINERS_ARRAY_H

#include "SnlfConfig.h"

#include <stdbool.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef uint32_t SnlfArraySizeType;

typedef struct {
  intptr_t array;
  SnlfArraySizeType size, capacity;
} SnlfArray;
typedef SnlfArray *SnlfArrayRef;

static inline void _SnlfArrayInit(SnlfArrayRef bag) {
  assert(bag);
  
  bag->array    = NULL;
  bag->size     = 0;
  bag->capacity = 0;
}

static inline void _SnlfArrayRelease(SnlfArrayRef bag) {
  assert(bag);
  assert(bag->array);
  
  free(bag->array);
}

static inline SnlfArraySizeType _SnlfArrayReserve(SnlfArrayRef bag, SnlfArraySizeType newCapacity, size_t itemSize) {
  assert(bag);
  
  if (newCapacity <= bag->capacity) {
    return bag->capacity;
  }
  
  SnlfArraySizeType targetCapacity = SNLF_ALIGN(newCapacity * itemSize);
  intptr_t temp = (intptr_t)realloc(bag->array, newCapacity);
  if (!temp) {
    return bag->capacity;
  }
  
  bag->array    = temp;
  bag->capacity = targetCapacity;
  return targetCapacity;
}

static inline intptr_t *_SnlfArrayGetPointerAt(SnlfArrayRef bag, SnlfArraySizeType index, size_t itemSize) {
  return (intptr_t *)((uint8_t *)bag->array + itemSize * index);
}

static inline bool _SnlfArrayAppend(SnlfArrayRef bag, intptr_t *ptr, size_t itemCount, size_t itemSize) {
  assert(bag);
  assert(ptr);
  
  const SnlfArraySizeType oldSize = bag->size;
  const SnlfArraySizeType newSize = oldSize + itemCount;
  if (_SnlfArrayReserve(bag, newSize, itemSize) < newSize) {
    return true;
  }
  
  memcpy(_SnlfArrayGetPointerAt(bag, oldSize, itemSize), ptr, itemSize * itemCount);
  
  bag->size += itemCount;
  return false;
}

static inline bool _SnlfArrayIndexOf(SnlfArrayRef bag, intptr_t item, SnlfArraySizeType *index, size_t itemSize) {
  assert(bag);
  assert(item);
  
  for (SnlfArraySizeType i = 0; i < bag->size; ++i) {
    intptr_t *compare = _SnlfArrayGetPointerAt(bag, i, itemSize);
    if (memcmp(*compare, item, itemSize) == 0) {
      *index = i;
      return false;
    }
  }
  return true;
}

static inline bool _SnlfArrayInsertAt(SnlfArrayRef bag, SnlfArraySizeType index, intptr_t *ptr, size_t itemCount, size_t itemSize) {
  assert(bag);
  assert(ptr);
  assert(index <= bag->size);
  
  if (index == bag->size) {
    return _SnlfArrayAppend(bag, ptr, itemCount, itemSize);
  }
  
  const SnlfArraySizeType oldSize = bag->size;
  const SnlfArraySizeType newSize = oldSize + itemCount;
  if (_SnlfArrayReserve(bag, newSize, itemSize) < newSize) {
    return true;
  }
  
  memmove(_SnlfArrayGetPointerAt(bag, index + itemCount, itemSize),
          _SnlfArrayGetPointerAt(bag, index, itemSize),
          itemSize * (oldSize - index));
  memcpy(_SnlfArrayGetPointerAt(bag, index, itemSize), ptr, itemSize * itemCount);
  
  bag->size += itemCount;
  return false;
}

static inline void _SnlfArrayRemoveAt(SnlfArrayRef bag, SnlfArraySizeType index, size_t itemSize) {
  assert(bag);
  assert(index < bag->size);
  
  if (--bag->size == 0) {
    return;
  }
  
  memmove(_SnlfArrayGetPointerAt(bag, index, itemSize),
          _SnlfArrayGetPointerAt(bag, index + 1, itemSize),
          itemSize * (bag->size - index));
}

static inline bool _SnlfArrayRemove(SnlfArrayRef bag, intptr_t item, SnlfArraySizeType *index, size_t itemSize) {
  assert(bag);
  assert(item);
  assert(index);
  
  if (_SnlfArrayIndexOf(bag, item, index, itemSize)) {
    return true;
  }
  
  _SnlfArrayRemoveAt(bag, *index, itemSize);
  return false;
}

#define _SAGetBagPointer(array) &array.__bag
#define _SAGetItemSize(array)   sizeof(*array.data)

#define SNLF_ARRAY(__TYPE__) \
  union { \
    SnlfArray __bag; \
    struct { \
      __TYPE__ *data; \
      SnlfArraySizeType size, capacity; \
    }; \
  }

#define SnlfArrayInit(array)                   _SnlfArrayInit(_SAGetBagPointer(array))
#define SnlfArrayRelease(array)                _SnlfArrayRelease(_SAGetBagPointer(array))
#define SnlfArrayGetPointerAt(array, index)    _SnlfArrayGetPointerAt(_SAGetBagPointer(array), index, _SAGetItemSize(array))
#define SnlfArrayAppend(array, item)           _SnlfArrayAppend(_SAGetBagPointer(array), (intptr_t *)&item, 1, _SAGetItemSize(array))
#define SnlfArrayIndexOf(array, item, index)   _SnlfArrayIndexOf(_SAGetBagPointer(array), item, index, _SAGetItemSize(array))
#define SnlfArrayInsertAt(array, index, item)  _SnlfArrayInsertAt(_SAGetBagPointer(array), index, (intptr_t *)&item, 1, _SAGetItemSize(array))
#define SnlfArrayRemoveAt(array, index)        _SnlfArrayRemoveAt(_SAGetBagPointer(array), index, _SAGetItemSize(array))
#define SnlfArrayRemove(array, item, indexPtr) _SnlfArrayRemove(_SAGetBagPointer(array), item, indexPtr, _SAGetItemSize(array))

#define SnlfArrayForeach(__ARRAY__) \
  intptr_t start = __ARRAY__.__bag.array; \
  intptr_t end = __ARRAY__.__bag.array + __ARRAY__.__bag.size; \
  for (intptr_t ptr = start; ptr != end; ++ptr)

#ifdef __cplusplus
}
#endif

#endif // _SNLF_CONTAINERS_ARRAY_H
