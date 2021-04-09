#include <stdio.h>
#include <ucontext.h>
#include <iostream>
void fun(){
    std::cout << 22 << std::endl;
    std::cout << 333 << std::endl;
}

void context_test(){
    char stack[1024*128];
    ucontext_t child,test;
    getcontext(&child);
    child.uc_stack.ss_flags = 0;
    child.uc_stack.ss_size = sizeof(stack);
    child.uc_stack.ss_sp = stack;
    child.uc_link = &test;
    makecontext(&child,fun,0);
    // setcontext(&child);
    swapcontext(&test,&child); // getcontext to test, then setcontext of child.
    std::cout << "main" << std::endl;
};

int main(){
    context_test();
    return 0;
}