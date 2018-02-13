/*
 * Copyright (c) 2000, 2001, 2002, 2003, 2004, 2005, 2008, 2009
 *	The President and Fellows of Harvard College.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE UNIVERSITY AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE UNIVERSITY OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/*
 * Thread test code.
 */
#include <types.h>
#include <lib.h>
#include <thread.h>
#include <synch.h>
#include <spinlock.h>
#include <test.h>

static int STHREADS = 10;
static int slockloops;

static struct semaphore *tsem = NULL;
static struct spinlock *tspinlk = NULL;
//struct spinlock *tspinlk = NULL;
//static struct lock *tlock = NULL;
static int counter = 0;

/*
static
void
init_sem(void)
{
	if(tsem==NULL){
		tsem = sem_create("tsem", 0);
		if(tsem == NULL){
			panic("lockthreadtest: sem_create failed\n");
		}
	}
}
*/
static
void
init_spinlock(void)
{
	if(tsem==NULL){
		tsem = sem_create("tsem",0);
		if(tsem == NULL){
			panic("lockthreadtest: sem_create failed\n");
		}
	}
	//----
	if (tspinlk==NULL) {
		spinlock_init(tspinlk);
		if (tspinlk == NULL) {
			panic("spinlockthreadtest: spinlock_init failed\n");
		}
	}
}

static
void
myspinlockthread(void *junk, unsigned long num)
{
	//int ch = '0' + num;
	int i;

	(void)junk;
	(void)num;

	for(i=0; i<slockloops; i++)
	{
		spinlock_acquire(tspinlk);
		counter++;
		spinlock_release(tspinlk);
	}

	//kprintf("ch %d\n",ch);
	//putch(ch);
	//for(i=0; i<200000; i++);

	V(tsem);
}

static
void
myspinlockrunthreads(void)
{
	char name[16];
	int i, result;

	for (i=0; i<STHREADS; i++) {
		snprintf(name, sizeof(name), "threadtest%d", i);
		init_spinlock();
		result = thread_fork(name, NULL,
				     myspinlockthread,
				     NULL, i);
		if (result) {
			panic("myspinlockthreadtest: thread_fork failed %s)\n", 
			      strerror(result));
		}
	}

	
	for (i=0; i<STHREADS; i++) {
		P(tsem);
	}
	
}


int
myspinlockthreadtest(int nargs, char **args)
{
	//(void)nargs;
	//(void)args;
	if(nargs > 1)
	{
		int ch = atoi(args[1]);
		STHREADS = ch;
	}
	slockloops = (nargs>2) ? atoi(args[2]) : 3;

	//init_spinlock();
	kprintf("Starting me unsafe threadtest...\n");
	myspinlockrunthreads();
	kprintf("counter should be: %d, but is: %d\n", STHREADS*slockloops, counter);
	kprintf("Me thread test done.\n");

	return 0;
}
