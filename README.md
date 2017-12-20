# Pracownia 3 - SO
 An implementation of a simple "user-level" thread library.

### Functions for managing threads and semaphores
 - thread thread_create(void func(void *), void *arg)
    - create a thread, attach function and return it
 - void thread_preempt(thread *t)
    - scheduler preempts a thread
 - void thread_yield()
    - thred returns control to the scheduler
 - void thread_exit()
    - thred ended its work
 - void thread_destroy(thread *t)
    - called from outside, destroys a thread
 - void sem_wait(semaphore *s) 
 - void sem_signal(semaphore *s)



### Using [ucontext](http://pubs.opengroup.org/onlinepubs/7908799/xsh/ucontext.h.html) library

 - int  getcontext(ucontext_t *)
 - int  setcontext(const ucontext_t *)
 - void makecontext(ucontext_t *, (void *)(), int, ...)
 - int  swapcontext(ucontext_t *, const ucontext_t *)

### Additional info

```
With preemption, implementing FIFO order for both thread execution and condition variable queues.
```
