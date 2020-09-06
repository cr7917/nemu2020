#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32

static WP wp_pool[NR_WP];
static WP *head, *free_;

void init_wp_pool() {
	int i;
	for(i = 0; i < NR_WP; i ++) {
		wp_pool[i].NO = i;
		wp_pool[i].next = &wp_pool[i + 1];
	}
	wp_pool[NR_WP - 1].next = NULL;

	head = NULL;
	free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */

WP* new_wp(){
       WP *result,*tmp;
       result=free_;
       free_=free_->next;
       result->next=NULL;
       tmp=head;
       if(tmp==NULL){
          head=result;
          tmp=head;
       }else{
          while(tmp->next!=NULL){tmp=tmp->next;}
          tmp->next=result;
       }
       return result;
}

void free_wp(WP *wp){
       WP *p,*q;
       q=free_;
       if(q==NULL){free_=wp;q=free_;}
       else{
          while(q->next!=NULL){q=q->next;}
          q->next=wp;
       }
       p=head;
       if(head==NULL){assert(0);}
       else{
         while(p->next!=NULL&&p->next->NO!=wp->NO){p=p->next;} 
         if(p->next->NO==wp->NO){p->next=p->next->next;}
         else {assert(0);}
       }
       wp->next=NULL;
       wp->value=0;
       wp->expression[0]='\0';
}

void delete_wp(int num){
       WP *target;
       target=&wp_pool[num];
       free_wp(target);
}

void info_wp(){
       WP *tmp;
       tmp=head;
       if(!tmp){assert(0);}
       else{
          while(tmp!=NULL){
              printf("watchpoint:%d,expression:%s,value(digit):%d\n",tmp->NO,tmp->expression,tmp->value);
              tmp=tmp->next;
          }
       }
}








































