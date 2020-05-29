/***************** queue.c file *****************/
int enqueue(PROC **queue, PROC *p)
{
  PROC *q = *queue;
  if (q == 0 || p->priority > q->priority){
    *queue = p;
    p->next = q;
  }
  else{
    while (q->next && p->priority <= q->next->priority)
      q = q->next;
    p->next = q->next;
    q->next = p;
  }
}/*end enqueue()*/

PROC *dequeue(PROC **queue)
{
  PROC *p = *queue;
  if (p)
    *queue = (*queue)->next;
  return p;
}/*end dequeue()*/

int printList(char *name, PROC *p)
{
  printf("%s = ", name);
  while(p){
    printf("[%d %d]->", p->pid, p->priority);
    p = p->next;
  }
  printf("NULL\n");
}/*end printList()*/


