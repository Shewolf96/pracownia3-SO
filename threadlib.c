#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>

#define MEM 64000
#define MAX_SEM_Q 10


typedef struct thread thread;

struct thread
{
    ucontext_t context;
    char state;
    int id;
    thread *next;

};


typedef struct {
    thread *last;
} th_queue;


typedef struct {
    thread *first;
    thread *last;
    thread *q [MAX_SEM_Q];

} sem_queue;


typedef struct
{
    int cond_val;
    sem_queue q;

} semaphore;



int counter = 0, exec_cycle = 4, id = 0;
th_queue thread_queue;
ucontext_t Main, c;

void f1();
void f2();
void scheduler();
thread thread_create(void (*func), void *arg);
void thread_preempt();
void thread_yield();
void thread_exit();
void thread_destroy(thread *t);
void sem_wait(semaphore *s);
void sem_signal(semaphore *s);

int next_id()
{
	return id++;
}



int main(int argc, char** argv)
{
	getcontext(&Main);
	
	thread t1, t2;
	t1 = thread_create((void*)&f1, NULL);
	t2 = thread_create((void*)&f2, NULL);

	setcontext(&(t1.context));	
//	switchcontext(&Main, &(t1.context));
	scheduler();



    return 0;
}


void scheduler()
{
    counter ++;
    if(counter >= exec_cycle)
    {
        counter = 0;
        thread_preempt();
    }
    
}


thread thread_create(void (*func), void *arg)
{
    thread t;
    getcontext(&c);
    c.uc_link=0;
    c.uc_stack.ss_sp=malloc(MEM);
    c.uc_stack.ss_size=MEM;
    c.uc_stack.ss_flags=0;
    makecontext(&c, func, 0);

    t.context = c;
    t.state = 'r'; 
    t.id = next_id();

    //...umieszczamy w kolejce...
    if(thread_queue.last == NULL)
    {
        thread_queue.last = &t;
        t.next = &t;
	printf("a tu: %c, %d???\n", thread_queue.last-> next-> state, thread_queue.last-> next-> id);
    }

    else
    {
	printf("a tu: %c, %d???\n", thread_queue.last-> next-> state, thread_queue.last-> next-> id);
        t.next = thread_queue.last-> next;
        thread_queue.last-> next = &t;
        thread_queue.last = &t;
	
	printf("a tu: %c, %d???\n", thread_queue.last->state, thread_queue.last->id);
	printf("a tu: %c, %d???\n", thread_queue.last-> next-> state, thread_queue.last-> next-> id);
    }

    printf("bezsensu....\n");
    return t;
}


void thread_yield()
{
    counter = exec_cycle;
    scheduler();
}


void thread_preempt()
{
	printf("thread preemption\n");
	thread* cur_t = thread_queue.last;
	thread* temp_t = cur_t -> next;
	
	while(temp_t-> state != 'r')
	{

		if((temp_t-> state == 'd') || (temp_t-> state == 'j'))
		{
			cur_t-> next = temp_t-> next;
			temp_t = temp_t-> next;
		}
			
		if((temp_t-> state == 'e') || (temp_t-> state == 'w'))
			temp_t = temp_t-> next;
	}
	
	thread_queue.last = temp_t;
	swapcontext(&(cur_t->context), &(temp_t->context));

}


void thread_exit()
{
	thread_queue.last-> state = 'e';
	thread_yield();
}


void thread_destroy(thread *t)
{
	t-> state = 'd';
	thread_yield();
}


void sem_wait(semaphore *s)
{
	//s.value--;
	//if < 0 -> do kolejki, scheduler()
	//else korzysta sobie i przy wyjsciu moze dac np. sem_signal (albo moze nie dac ^^)
}


void sem_signal(semaphore *s)
{
	//s.value++;
	//jesli teraz jest <=0 tzn. ze ktosik w kolejce, trzeba go wziac stamtad i...?
}


void f1()
{
	printf("f1: Hello world\n");
	scheduler();
	printf("f1: And goodbye\n");
	scheduler();
	thread_exit();

}

void f2()
{
	printf("f2: Hello world\n");
	scheduler();
	int i;
	scheduler();
	for(i=0; i<10; i++)
	{
		printf("f2: i=%d \n", i);
		scheduler();
	}

	thread_exit();

}
