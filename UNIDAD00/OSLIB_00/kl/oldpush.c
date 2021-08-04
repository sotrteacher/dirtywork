CONTEXT pf_index;
DWORD *pf_stack_ptr;
DWORD pf_tmp, pf_func;
int pf_n, pf_i;
void ll_push_func(CONTEXT c, void (*func)(void), int n)
{
	pf_n = n;
	pf_index = TSSsel2index(c);
	pf_func = (DWORD)func;

	pf_stack_ptr = (DWORD *)TSS_table[pf_index].esp;

	pf_stack_ptr--;
	for (pf_i = 0; pf_i < pf_n; pf_i++) {
		*(pf_stack_ptr + pf_i) = *(pf_stack_ptr + pf_i + 1);
	}

	*(pf_stack_ptr + pf_n) = pf_func;	
/*
	tmp = (DWORD) *stack_ptr;
	*stack_ptr = (DWORD)(func);
	stack_ptr--;
	*stack_ptr = tmp;
*/
	TSS_table[pf_index].esp = (DWORD)(pf_stack_ptr);
	TSS_table[pf_index].esp0= (DWORD)(pf_stack_ptr);
}

