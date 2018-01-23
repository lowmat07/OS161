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
#include <test.h>

int LTHREADS = 10;
int nlockloops;

static struct semaphore *tsem = NULL;
static struct lock *tlock = NULL;
static int counter = 0;

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

static
void
init_lock(void)
{
	if (tlock==NULL) {
		tlock = lock_create("tlock");
		if (tlock == NULL) {
			panic("lockthreadtest: lock_create failed\n");
		}
	}
}

static
void
mylockthread(void *junk, unsigned long num)
{
	//int ch = '0' + num;
	int i;

	(void)junk;
	(void)num;

	for(i=0; i<nlockloops; i++)
	{
		lock_acquire(tlock);
		counter++;
		lock_release(tlock);
	}

	//kprintf("ch %d\n",ch);
	//putch(ch);
	//for(i=0; i<200000; i++);

	V(tsem);
}

static
void
mylockrunthreads(void)
{
	char name[16];
	int i, result;

	for (i=0; i<LTHREADS; i++) {
		snprintf(name, sizeof(name), "threadtest%d", i);
		init_lock();
		result = thread_fork(name, NULL,
				     mylockthread,
				     NULL, i);
		if (result) {
			panic("threadtest: thread_fork failed %s)\n", 
			      strerror(result));
		}
	}

	
	for (i=0; i<LTHREADS; i++) {
		P(tsem);
	}
	
}


int
mylockthreadtest(int nargs, char **args)
{
	//(void)nargs;
	//(void)args;
	if(nargs > 1)
	{
		int ch = atoi(args[1]);
		LTHREADS = ch;
	}
	nlockloops = (nargs>2) ? atoi(args[2]) : 3;

	init_sem();
	kprintf("Starting me unsafe threadtest...\n");
	mylockrunthreads();
	kprintf("counter should be: %d, but is: %d\n", LTHREADS*nlockloops, counter);
	kprintf("Me thread test done.\n");

	return 0;
}
