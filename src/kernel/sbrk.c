#include <kernel/sbrk.h>
#include <sys/types.h>

/* Defined by the linker */
extern char *__stack;
extern char end;

caddr_t sbrk(int incr) {
  static char *heap_end;
  char *prev_heap_end;
 
  if (heap_end == 0) {
    heap_end = &end;
  }
  prev_heap_end = heap_end;
  if (heap_end + incr > __stack) {
    /*
     *write (1, "Heap and stack collision\n", 25);
     */
    abort();
  }

  heap_end += incr;
  return (caddr_t) prev_heap_end;
}

