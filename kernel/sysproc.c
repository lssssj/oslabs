#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_exit(void)
{
  int n;
  if(argint(0, &n) < 0)
    return -1;
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  if(argaddr(0, &p) < 0)
    return -1;
  return wait(p);
}

uint64
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;
  backtrace();
  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

void 
restorectx(struct proc *p)
{
  p->trapframe->ra = p->handler.ra;
  p->trapframe->sp = p->handler.sp;
  p->trapframe->gp = p->handler.gp;
  p->trapframe->tp = p->handler.tp;
  p->trapframe->t0 = p->handler.t0;
  p->trapframe->t1 = p->handler.t1;
  p->trapframe->t2 = p->handler.t2;
  p->trapframe->s0 = p->handler.s0;
  p->trapframe->s1 = p->handler.s1;
  p->trapframe->a0 = p->handler.a0;
  p->trapframe->a1 = p->handler.a1;
  p->trapframe->a2 = p->handler.a2;
  p->trapframe->a3 = p->handler.a3;
  p->trapframe->a4 = p->handler.a4;
  p->trapframe->a5 = p->handler.a5;
  p->trapframe->a6 = p->handler.a6;
  p->trapframe->a7 = p->handler.a7;
  p->trapframe->s2 = p->handler.s2;
  p->trapframe->s3 = p->handler.s3;
  p->trapframe->s4 = p->handler.s4;
  p->trapframe->s5 = p->handler.s5;
  p->trapframe->s6 = p->handler.s6;
  p->trapframe->s7 = p->handler.s7;
  p->trapframe->s8 = p->handler.s8;
  p->trapframe->s9 = p->handler.s9;
  p->trapframe->s10 = p->handler.s10;
  p->trapframe->s11 = p->handler.s11;
  p->trapframe->t3 = p->handler.t3;
  p->trapframe->t4 = p->handler.t4;
  p->trapframe->t5 = p->handler.t5;
  p->trapframe->t6 = p->handler.t6;
}

uint64
sys_sigreturn(void)
{
  struct proc* p = myproc();
  p->trapframe->epc = p->handler.opc;
  restorectx(p);
  p->handler.finished = 1;
  return 0;
}

uint64
sys_sigalarm(void)
{
  struct proc* p = myproc();
  int n;
  uint64 ip;
  if(argint(0, &n) < 0)
    return -1;
  p->handler.internal = n;
  p->handler.left = n;
  p->handler.finished = 1;
  if(argaddr(1, &ip) < 0)
    return -1;
  p->handler.func = (void (*)(void))ip;
  return 0;
}