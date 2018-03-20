#include <stdio.h>
#include <sys/time.h>
#include <signal.h>
#include <stdlib.h>
#include <ucontext.h>
#include <unistd.h>

#include "mythread.h"
#include "interrupt.h"

#include "queue.h"

TCB* scheduler();
void activator();
void timer_interrupt(int sig);
void network_interrupt(int sig);

/* Array of state thread control blocks: the process allows a maximum of N threads */
static TCB t_state[N]; 

/* Current running thread */
static TCB* running;
static int current = 0;

/* Variable indicating if the library is initialized (init == 1) or not (init == 0) */
static int init=0;

/* Thread control block for the idle thread */
static TCB idle;
static void idle_function(){
  while(1);
}
/* Colas de distintas prioridadesa */
static struct queue* colaA;
static struct queue* colaB;

/* Initialize the thread library */
void init_mythreadlib() {
  int i;  
  /* Create context for the idle thread */
  if(getcontext(&idle.run_env) == -1){
    perror("*** ERROR: getcontext in init_thread_lib");
    exit(-1);
  }
  idle.state = IDLE;
  idle.priority = SYSTEM;
  idle.function = idle_function;
  idle.run_env.uc_stack.ss_sp = (void *)(malloc(STACKSIZE));
  idle.tid = -1;
  if(idle.run_env.uc_stack.ss_sp == NULL){
    printf("*** ERROR: thread failed to get stack space\n");
    exit(-1);
  }
  idle.run_env.uc_stack.ss_size = STACKSIZE;
  idle.run_env.uc_stack.ss_flags = 0;
  idle.ticks = QUANTUM_TICKS;
  makecontext(&idle.run_env, idle_function, 1); 

  t_state[0].state = INIT;
  t_state[0].priority = LOW_PRIORITY;
  t_state[0].ticks = QUANTUM_TICKS;
  if(getcontext(&t_state[0].run_env) == -1){
    perror("*** ERROR: getcontext in init_thread_lib");
    exit(5);
  } 

  for(i=1; i<N; i++){
    t_state[i].state = FREE;
  }
 
  t_state[0].tid = 0;
  running = &t_state[0];

  /* Initialize network and clock interrupts */
  init_network_interrupt();
  init_interrupt();
  /* Initialize queue */
  disable_interrupt();
  disable_network_interrupt();
  colaA = queue_new();
  colaB = queue_new();
  enable_network_interrupt();
  enable_interrupt();
}


/* Create and intialize a new thread with body fun_addr and one integer argument */ 
int mythread_create (void (*fun_addr)(),int priority)
{
  int i;
  
  if (!init) { init_mythreadlib(); init=1;}
  for (i=0; i<N; i++)
    if (t_state[i].state == FREE) break;
  if (i == N) return(-1);
  if(getcontext(&t_state[i].run_env) == -1){
    perror("*** ERROR: getcontext in my_thread_create");
    exit(-1);
  }
  t_state[i].state = INIT;
  t_state[i].priority = priority;
  /* Añadimos la siguiente línea para que cada hilo tenga su rodaja completa al inicializarse*/
  t_state[i].ticks = QUANTUM_TICKS;
  t_state[i].function = fun_addr;
  t_state[i].run_env.uc_stack.ss_sp = (void *)(malloc(STACKSIZE));
  if(t_state[i].run_env.uc_stack.ss_sp == NULL){
    printf("*** ERROR: thread failed to get stack space\n");
    exit(-1);
  }
  t_state[i].tid = i;
  t_state[i].run_env.uc_stack.ss_size = STACKSIZE;
  t_state[i].run_env.uc_stack.ss_flags = 0;
  makecontext(&t_state[i].run_env, fun_addr, 1); 
  /* Encolamos los hilo a medida que se van creando según su prioridad */
  if (priority == 1) {
    /* Si el hilo es de alta prioridad */
    disable_interrupt();
    disable_network_interrupt();
    enqueue(colaA, &t_state[i]);
    enable_network_interrupt();
    enable_interrupt();
    /* Se comprueba si el hilo ejecutandose es de baja prioridad, para llamar al planificador */
    if (running->priority == 0)
     {
       TCB* new = scheduler();   
      activator(new);
     } 
    
  } else {
    disable_interrupt();
    disable_network_interrupt();
    enqueue(colaB, &t_state[i]);
    enable_network_interrupt();
    enable_interrupt();
  }
  
  return i;
} /****** End my_thread_create() ******/

/* Read network syscall */
int read_network()
{
   return 1;
}

/* Network interrupt  */
void network_interrupt(int sig)
{
} 


/* Free terminated thread and exits */
void mythread_exit() {
  int tid = mythread_gettid();  

  printf("*** THREAD %d FINISHED\n", tid);  
  t_state[tid].state = FREE;
  free(t_state[tid].run_env.uc_stack.ss_sp); 

  TCB* next = scheduler();
  activator(next);
}

/* Sets the priority of the calling thread */
void mythread_setpriority(int priority) {
  int tid = mythread_gettid();  
  t_state[tid].priority = priority;
}

/* Returns the priority of the calling thread */
int mythread_getpriority(int priority) {
  int tid = mythread_gettid();  
  return t_state[tid].priority;
}


/* Get the current thread id.  */
int mythread_gettid(){
  if (!init) { init_mythreadlib(); init=1;}
  return current;
}


/* RR con prioridad */
TCB* scheduler(){
  disable_interrupt();
  disable_network_interrupt();
  TCB* aux;
  /* Si hay un hilo de alta prioridad listo */
  if (!queue_empty(colaA))
  {
    /* Lo desencolamos */
    aux = dequeue(colaA);
  } else {
    /* Si no */
    if (!queue_empty(colaB))
    {
      /* Desencolamos el siguiente listo de prioridad baja */
      aux = dequeue(colaB);
    } else {
      /* Si no quedan más hilos en la cola y el hilo que se está ejecutando no ha terminado */
      if(running->state == 1) {
        /* Devolvemos el hilo que se está ejecutando */
        aux = running;
      } else {
        /* Si el hilo en ejecución ha terminado y no quedan más, se devuelve el idle */
        aux = &idle;
      }
    }
  }
  enable_network_interrupt();
  enable_interrupt();
  return aux;
  printf("mythread_free: No thread in the system\nExiting...\n"); 
  exit(1); 
}


/* Timer interrupt  */
void timer_interrupt(int sig)
{
  /* Si el hilo es de prioridad baja, se sigue el Round Robin*/
  if(running->priority == 0){
    running->ticks--;
    /* Comprobamos si el hilo en ejecución ha terminado su cuanto */
    if(running->ticks == 0) {
      TCB* aux = scheduler();
      activator(aux);
    }
  }
  
} 

/* Activator */
void activator(TCB* next){
  TCB* prevrunning = running;
  running = next;
  current = next->tid;
  /* Si el hilo anterior ha terminado su cuanto */
  if (prevrunning->ticks == 0)
  {/* Se restablece  */
    prevrunning->ticks = QUANTUM_TICKS;
  }
  /* Se comprueba si el hilo que se va a ejecutar es el idle */
  if(running->state == 3){
    printf("*** FINISH\n");
    /* Se sale del programa */
    exit(0);
  }
  /* Comprobamos si el hilo que va a ser expulsado ha terminado su ejecución */
  if(!prevrunning->state) {
    printf("*** THREAD %i TERMINATED: SETCONTEXT OF %i\n", prevrunning->tid,current);
    setcontext(&(running->run_env));
    printf("mythread_free: After setcontext, should never get here!!...\n");  
  }
  disable_interrupt();
  disable_network_interrupt();
  /* Si el hilo que va a salir no ha terminado su ejecución y no es el mismo que estaba ejecutandose */
  if(prevrunning->tid != current) {
    /* Se restablece su cuanto */
    prevrunning->ticks = QUANTUM_TICKS;
    /* Lo encolamos */
    enqueue(colaB, prevrunning);
  }
  /* Solo encolaremos de nuevo los hilos que sean de baja prioridad, porque los de alta siguen un FIFO y no hay cambios de contexto voluntarios */
  enable_network_interrupt();
  enable_interrupt();
  if (prevrunning->priority == 0 && running->priority == 1)
  {
    printf("*** THREAD %i PREEMTED: SETCONTEXT OF %i\n", prevrunning->tid,current);
  } else {
    /* Imprimimos que se va a cambiar de contexto entre dos hilos distintos */
    if (prevrunning->tid != current)
    {
      printf("*** SWAPCONTEXT FROM %i TO %i\n", prevrunning->tid,current);
    }
  }
  if (prevrunning->tid != current)
  {/* Solo realizamos el cambio de contexto si los dos hilos son distintos */
    swapcontext(&(prevrunning->run_env),&(running->run_env));
  }
  //printf("mythread_free: After setcontext, should never get here!!...\n");  
}