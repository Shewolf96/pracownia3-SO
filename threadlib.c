#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>

#define MEM 64000

int counter = 0, exec_cycle = 4;
th_queue thread_queue;


typedef struct {
    thread *first;
    thread *last;

} th_queue;


typedef struct
{
    ucontext_t context;
    int state;
    thread *next;

} thread;


typedef struct
{
    int cond_val;
    th_queue q;

} semaphore;

// int getcontext(ucontext_t *)
// int setcontext(const ucontext_t *)
// void makecontext(ucontext_t *, (void *)(), int, ...)
// int swapcontext(ucontext_t *, const ucontext_t *)

thread thread_create(void func(void *), void *arg)
{
    thread t;
    getcontext(&c);
    c.uc_link=0;
    c.uc_stack.ss_sp=malloc(MEM);
    c.uc_stack.ss_size=MEM;
    c.uc_stack.ss_flags=0;
    makecontext(&c, (void*)&func, arg);

    t.context = c;
    t.state = 1; //Ready

    //...umieszczamy w kolejce...
    if(thread_queue.last == NULL)
        thread_queue.first = thread_queue.last = &t;

    else
    {
        (*thread_queue.last).next = &t;
        thread_queue.last = &t;
        (*thread_queue.last).next = thread_queue.first;
    }

    return t;
}

void thread_yield()
{
    counter = exec_cycle;
    scheduler();
}


//bez thread_preempt - wszystko się dzieje w scheduler() nopnope
//zamiast tego thread_join??
void thread_preempt()
{

}

void thread_exit();
// thred ended its work (and can be joined??)
void thread_destroy(thread *t);
// called from outside, destroys a thread
void sem_wait(semaphore *s);
void sem_signal(semaphore *s);


void scheduler()
{
    counter ++;
    if(counter >= exec_cycle)
    {
        counter = 0;
        thread_preempt();
    }
    //counting
    //swapping
    //managing the main thread queue
    //checking if state blocked/ready/waiting
}


int main(int argc, char** argv)
{




    return 0;
}
