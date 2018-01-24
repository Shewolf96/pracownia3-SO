#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>

#define MEM 64000
#define MAX_SEM_Q 10


typedef struct thread thread;

int counter = 0, exec_cycle = 4, id = 0;

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


typedef struct
{
    int value;
    int first;
    int last;
    int q_size; 
    thread *tab [MAX_SEM_Q];


} semaphore;

/*___________________________________________________________*/

th_queue thread_queue;

ucontext_t Main, c;

semaphore *semaphore1;

/*___________________________________________________________*/

void scheduler();

thread* thread_create(void (*func), void *arg);
void thread_preempt();
void thread_yield();
void thread_exit();
void thread_destroy(thread *t);
int thread_join(thread *t);

void sem_wait(semaphore *s);
void sem_signal(semaphore *s);
void sem_push(semaphore *s);
thread* sem_pull(semaphore *s);
semaphore* sem_create(int init_value);

int next_id();
void start();


void f1();
void f2();
void f3();
void f4();

/*___________________________________________________________*/

int main(int argc, char** argv)
{
	start();
	semaphore1 = sem_create(0);
	thread *t1, *t2, *t3, *t4;
	t1 = thread_create((void*)&f1, NULL);
	t2 = thread_create((void*)&f2, NULL);
	t3 = thread_create((void*)&f3, NULL);
	t4 = thread_create((void*)&f4, NULL);
	
    
    for (int i = 0; i < 10; i++)
    {
        printf("Main: %d\n", i);
	    scheduler();
	}
	
	thread_destroy(t2);
	scheduler();
	
	sem_signal(semaphore1);
	scheduler();

    while (thread_join(t1))
        thread_yield();
        
    while (thread_join(t2))
        thread_yield();
        
    while (thread_join(t3))
        thread_yield();

    while (thread_join(t4))
        thread_yield();
    
    return 0;
}

/*___________________________________________________________*/

void start()
{
	getcontext(&Main);
	Main.uc_link=0;
    Main.uc_stack.ss_sp=malloc(MEM);
    Main.uc_stack.ss_size=MEM;
    Main.uc_stack.ss_flags=0;
	thread* t = malloc(sizeof(thread));
	t->context = Main;
	t->state = 'r';
	t->id = next_id();
	t->next = t;
	thread_queue.last = t;
}


thread* thread_create(void (*func), void *arg)
{
    thread* t = malloc(sizeof(thread));
    getcontext(&c);
    c.uc_link=0;
    c.uc_stack.ss_sp=malloc(MEM);
    c.uc_stack.ss_size=MEM;
    c.uc_stack.ss_flags=0;
    makecontext(&c, func, 0);

    t->context = c;
    t->state = 'r'; 
    t->id = next_id();

    
    if(thread_queue.last == NULL)
    {
        thread_queue.last = t;
        t->next = t;
    }

    else
    {
        ucontext_t *temp = &thread_queue.last->context;
        t->next = thread_queue.last-> next;
        thread_queue.last-> next = t;
        thread_queue.last = t;
        counter = 0;
        swapcontext(temp, &thread_queue.last->context);
    }

    return t;
}

semaphore* sem_create(int init_value)
{
    semaphore *s = malloc(sizeof(semaphore));
    s->value = 0;
    s->first = 0;
    s->last = -1;
    s->q_size = 0;; 
    return s;
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


void thread_preempt()
{
	thread* last_t = thread_queue.last;
	thread* cur_t = thread_queue.last;
	thread* temp_t = cur_t -> next;
	//printf("preempt thread %d\n", cur_t->id);
	
	while(temp_t-> state != 'r')
	{

		if((temp_t-> state == 'd') || (temp_t-> state == 'j'))
		{
		    //printf("thread %d out of thread_queue\n", temp_t->id);
			cur_t-> next = temp_t-> next;
			temp_t = temp_t-> next;
		}
			
		if((temp_t-> state == 'e') || (temp_t-> state == 'w'))
		{
		    cur_t = cur_t-> next;
			temp_t = temp_t-> next;
		}
	}
	
	thread_queue.last = temp_t;
	swapcontext(&(last_t->context), &(temp_t->context));

}


int thread_join(thread *t)
{

    if(t-> state == 'd' || t-> state == 'j')
    {
        printf("not succeeded in joining thread %d\n", t->id);
        return 0;
    }
    
    if(t-> state != 'e') return 1;
        
    printf("thread %d joined\n", t->id);
    t-> state = 'j';
    return 0;
}


void thread_yield()
{
    counter = exec_cycle;
    scheduler();
}


void thread_exit()
{
	thread_queue.last-> state = 'e';
	printf("thread %d exited\n", thread_queue.last -> id);
	thread_yield();
}


void thread_destroy(thread *t)
{
	t-> state = 'd';
	thread_yield();
}


void sem_wait(semaphore *s)
{
	s->value--;

	printf("sem_wait() call, value = %d\n", s->value);	
	if(s->value < 0)
	{

	    thread_queue.last->state = 'w';
	    sem_push(s);
	    thread_yield();
	}
}


void sem_signal(semaphore *s)
{
	s->value++;

    printf("sem_signal() call, value = %d\n", s->value);	
	if(s->value <= 0)
	{
	    thread *t = sem_pull(s);
	    if (t->state == 'w')
	    {
	        t->state = 'r';
	        /*counter = 0;
        	thread* cur_t = thread_queue.last;
        	thread_queue.last = t;
        	swapcontext(&(cur_t->context), &(t->context));*/
	    }
	    
	    else sem_signal(s);
	}

}

void sem_push(semaphore *s)
{
    s-> last = (s-> last + 1) % MAX_SEM_Q;
    s-> tab[s-> last] = thread_queue.last;
    s-> q_size++;
}

thread* sem_pull(semaphore *s)
{
    //if(sem_empty(s)) to jakis error powinien byc
    //if s->my_queue ->first ....
    //printf("... %lu\n", sizeof(sem_q1.tab)/sizeof(thread*));
    
    thread *t = s-> tab[s-> first];
    s-> first = (s-> first + 1) % MAX_SEM_Q;
    s-> q_size--;
    
    return t;
}


int next_id()
{
	return id++;
}

/*___________________________________________________________*/

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

void f3()
{
	printf("f3: Hello world\n");
	scheduler();
	int i;
	scheduler();
	sem_wait(semaphore1);
	scheduler();
	for(i=0; i<10; i++)
	{
		printf("f3: i=%d \n", i);
		scheduler();
	}
	sem_signal(semaphore1);
	
	thread_exit();
}

void f4()
{
    printf("f4: Hello world\n");
    scheduler();
    sem_wait(semaphore1);
    scheduler();
    printf("f4: thread %d gained access to semaphore1\n", thread_queue.last->id);    

    thread_exit();
}

