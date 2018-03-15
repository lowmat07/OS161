#include <types.h>
#include <kern/errno.h>
#include <kern/unistd.h>
#include <kern/wait.h>
#include <lib.h>
#include <syscall.h>
#include <current.h>
#include <proc.h>
#include <thread.h>
#include <addrspace.h>
#include <copyinout.h>
#include <mips/trapframe.h>

static void uproc_thread(void *temp_tf, unsigned long k)
{ 
	struct trapframe *ctf, ctf_stack;
	ctf = (struct trapframe *) temp_tf;
	//fork's child gets set to 0
	ctf->tf_v0 = 0;
	ctf->tf_a3 = 0;

	ctf->tf_epc += 4;
	//(void) temp_tf;
	
	ctf_stack = *ctf;
	kfree(ctf);

	//enter mips_usermode
	mips_usermode(&ctf_stack);
	
	(void) k;
	//kprintf("hey, i successfully forked and copied tf");
}

  /* this implementation of sys__exit does not do anything with the exit code */
  /* this needs to be fixed to get exit() and waitpid() working properly */
void sys__exit(int exitcode) {

  struct addrspace *as;
  struct proc *p = curproc;
  /* for now, just include this to keep the compiler from complaining about
     an unused variable */
  (void)exitcode;

  DEBUG(DB_SYSCALL,"Syscall: _exit(%d)\n",exitcode);

  KASSERT(curproc->p_addrspace != NULL);
  as_deactivate();
  /*
   * clear p_addrspace before calling as_destroy. Otherwise if
   * as_destroy sleeps (which is quite possible) when we
   * come back we'll be calling as_activate on a
   * half-destroyed address space. This tends to be
   * messily fatal.
   */
  as = curproc_setas(NULL);
  as_destroy(as);

  /* detach this thread from its process */
  /* note: curproc cannot be used after this call */
  proc_remthread(curthread);

  /* if this is the last user process in the system, proc_destroy()
     will wake up the kernel menu thread */
  proc_destroy(p);
  
  thread_exit();
  /* thread_exit() does not return, so we should never get here */
  panic("return from thread_exit in sys_exit\n");
}

/* stub handler for sys_fork() system call */
int
sys_fork(struct trapframe *tf, pid_t *retval)
{
  /* for now, this is just a stub that always returns a PID of 1 */
  /* you need to fix this to make it work properly */
  
  //copy parent addrspace
  struct addrspace * tas;
  as_copy(curproc->p_addrspace, &tas);
  
  //allocate and copy parent trapframe to temp *ttf
  struct trapframe * ttf = (struct trapframe*)kmalloc(sizeof(struct trapframe));
  *ttf = *tf;  
  
  //allocate new child pid

  
  //create and fill in new proc struct
  struct proc * childproc = proc_create_runprogram("childproc");

  childproc->p_addrspace = tas;
  
  //thread_fork
  //thread_fork("temp", childproc, uproc_thread, NULL, 0);
  thread_fork("temp", childproc, uproc_thread, (struct trapframe*)ttf, 0);

  //parent sets retval to child pid and returns
  *retval = 1;
  return(0);
}


/* stub handler for getpid() system call                */
int
sys_getpid(pid_t *retval)
{
  /* for now, this is just a stub that always returns a PID of 1 */
  /* you need to fix this to make it work properly */
  *retval = 1;
  return(0);
}

/* stub handler for waitpid() system call                */

int
sys_waitpid(pid_t pid,
	    userptr_t status,
	    int options,
	    pid_t *retval)
{
  int exitstatus;
  int result;

  /* this is just a stub implementation that always reports an
     exit status of 0, regardless of the actual exit status of
     the specified process.   
     In fact, this will return 0 even if the specified process
     is still running, and even if it never existed in the first place.

     Fix this!
  */

  if (options != 0) {
    return(EINVAL);
  }
  /* for now, just pretend the exitstatus is 0 */
  exitstatus = 0;
  result = copyout((void *)&exitstatus,status,sizeof(int));
  if (result) {
    return(result);
  }
  *retval = pid;
  return(0);
}

