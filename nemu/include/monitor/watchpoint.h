#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

typedef struct watchpoint {
	int NO;
	struct watchpoint *next;
       
	/* TODO: Add more members if necessary */
        uint32_t value;
        char expression[32];
} WP;

#endif

WP* new_wp();
void free_wp(WP *);
void delete_wp(int );
void info_wp();
