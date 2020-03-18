#ifndef _SNLF_CONTAINERS_LOCKFREEQUEUE_H
#define _SNLF_CONTAINERS_LOCKFREEQUEUE_H

#include "SnlfLockFreeQueue+Prototypes.h"

#ifdef _WIN32
#  ifndef STRICT
#    define STRICT
#  endif
#  define WIN32_LEAN_AND_MEAN
#  include <Windows.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

struct _SnlfLockFreeQueueNode {
  intptr_t data;
  struct _SnlfLockFreeQueueNode *next;
};

static inline bool SnlfLockFreeQueueInit(SnlfLockFreeQueue *queue) {
  struct _SnlfLockFreeQueueNode *node = SnlfAlloc(struct _SnlfLockFreeQueueNode);
  if (!node) {
    return true;
  }
  
  node->data = NULL;
  node->next = NULL;
  
  queue->head = node;
  queue->tail = node;
  return false;
}

static inline bool SnlfLockFreeQueueEnqueue(SnlfLockFreeQueue *queue, intptr_t data) {
  struct _SnlfLockFreeQueueNode *node = SnlfAlloc(struct _SnlfLockFreeQueueNode);
  if (!node) {
    return true;
  }
  node->data = data;
  node->next = NULL;
  
  while (1) {
    const struct _SnlfLockFreeQueueNode *tail = (struct _SnlfLockFreeQueueNode *)queue->tail;
    const struct _SnlfLockFreeQueueNode *next = (struct _SnlfLockFreeQueueNode *)tail->next;

    // Check queue if write from other thread
    const struct _SnlfLockFreeQueueNode *tail2 = (struct _SnlfLockFreeQueueNode *)queue->tail;
    if (tail != tail2) {
      continue;
    }

    if (next == NULL) {
      if (atomic_compare_exchange_weak((atomic_intptr_t *)&tail->next, (intptr_t *)&next, node)) {
        atomic_compare_exchange_strong((atomic_intptr_t *)&queue->tail, (intptr_t *)&tail, node);
        return false;
      }
    } else {
      atomic_compare_exchange_strong((atomic_intptr_t *)&queue->tail, (intptr_t *)&tail, next);
    }
  }
}

static inline intptr_t SnlfLockFreeQueueDequeue(SnlfLockFreeQueue *queue) {
  while (1) {
    const struct _SnlfLockFreeQueueNode *head = queue->head;
    const struct _SnlfLockFreeQueueNode *tail = queue->tail;
    const struct _SnlfLockFreeQueueNode *next = head->next;
    
    // Check queue if write from other thread
    const struct _SnlfLockFreeQueueNode *head2 = queue->head;
    if (head != head2) {
      continue;
    }
    
    if (head == tail) {
      if (next == NULL) {
        return NULL;
      }
      atomic_compare_exchange_strong((atomic_intptr_t *)&tail->next, (intptr_t *)&tail, next);
    } else {
      intptr_t result = next->data;
      if ((atomic_intptr_t *)atomic_compare_exchange_weak((atomic_intptr_t *)&queue->head, (intptr_t *)&head, next)) {
        SnlfDealloc(head);
        return result;
      }
    }
  }
}

static inline void SnlfLockFreeQueueUninit(SnlfLockFreeQueue *queue) {
  while (SnlfLockFreeQueueDequeue(queue)) {
  }
  SnlfDealloc(queue);
}

#ifdef __cplusplus
}
#endif

#endif // _SNLF_CONTAINERS_LOCKFREEQUEUE_H
