#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"
#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint32_t);

/* We use the `readline' library to provide more flexibility to read from stdin. */
char* rl_gets() {
	static char *line_read = NULL;

	if (line_read) {
		free(line_read);
		line_read = NULL;
	}

	line_read = readline("(nemu) ");

	if (line_read && *line_read) {
		add_history(line_read);
	}

	return line_read;
}

static int cmd_c(char *args) {
	cpu_exec(-1);
	return 0;
}

static int cmd_q(char *args) {
	return -1;
}

static int cmd_si(char *args){
       int num=1;
       char *arg=strtok(NULL," "); 
       if(arg!=NULL){
          sscanf(arg,"%d",&num);
       }
       cpu_exec(num);
       return 0;
}

static int cmd_info(char *args){
       if(args[0]=='r'){
           printf("eax:0x%08x\n",cpu.eax);
           printf("ecx:0x%08x\n",cpu.ecx);
           printf("edx:0x%08x\n",cpu.edx);
           printf("ebx:0x%08x\n",cpu.ebx);
           printf("esp:0x%08x\n",cpu.esp);
           printf("ebp:0x%08x\n",cpu.ebp);
           printf("esi:0x%08x\n",cpu.esi);
           printf("edi:0x%08x\n",cpu.edi);
       }else if(args[0]=='w'){
           info_wp();
       }else assert(0);
       return 0;
}

static int cmd_p(char *args){
       bool success;
       uint32_t result;
       result=expr(args,&success);
       if(success){printf("result is 0x%08x\n",result);}
       else assert(0);       
       return 0;
}

static int cmd_x(char *args){
       int num;
       char *arg1=strtok(NULL," ");
       sscanf(arg1,"%d",&num);
       swaddr_t address;
       char *arg2=strtok(NULL," ");
       bool success;
       address=expr(arg2,&success);
       if(!success) {printf("expression is wrong\n");return 0;}
       printf("start address is 0x%08x\n",address);
       int i;
       for(i=0;i<num;i++){
           printf("address is 0x%08x,value is 0x%08x\n",address,swaddr_read(address,4));
           address+=4;
       }
       return 0;
}

static int cmd_d(char *args){
        int delete_num;
        sscanf(args,"%d",&delete_num);
        delete_wp(delete_num);
        printf("delete watchpoint%d successfully\n",delete_num);
        return 0;
}

static int cmd_w(char *args){
        WP *point;
        bool success;
        point=new_wp();
        point->value=expr(args,&success);
        strcpy(point->expression,args);
        if(!success){printf("expression is wrong\n");return 0;}
        printf("watchpoint id %d,expression is %s,value is %d\n",point->NO,args,point->value);
        return 0;
}

static int cmd_help(char *args);

static struct {
	char *name;
	char *description;
	int (*handler) (char *);
} cmd_table [] = {
	{ "help", "Display informations about all supported commands", cmd_help },
	{ "c", "Continue the execution of the program", cmd_c },
	{ "q", "Exit NEMU", cmd_q },

	/* TODO: Add more commands */
        { "si","execute N steps",cmd_si},
        { "info","print the register or watchpoint's information",cmd_info},
        { "x","scan memory",cmd_x},
        { "p","calculate the expression",cmd_p},
        { "w","set the watchpoint",cmd_w},
        { "d","delete the watchpoint",cmd_d},
};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
	/* extract the first argument */
	char *arg = strtok(NULL, " ");
	int i;

	if(arg == NULL) {
		/* no argument given */
		for(i = 0; i < NR_CMD; i ++) {
			printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
		}
	}
	else {
		for(i = 0; i < NR_CMD; i ++) {
			if(strcmp(arg, cmd_table[i].name) == 0) {
				printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
				return 0;
			}
		}
		printf("Unknown command '%s'\n", arg);
	}
	return 0;
}

void ui_mainloop() {
	while(1) {
		char *str = rl_gets();
		char *str_end = str + strlen(str);

		/* extract the first token as the command */
		char *cmd = strtok(str, " ");
		if(cmd == NULL) { continue; }

		/* treat the remaining string as the arguments,
		 * which may need further parsing
		 */
		char *args = cmd + strlen(cmd) + 1;
		if(args >= str_end) {
			args = NULL;
		}

#ifdef HAS_DEVICE
		extern void sdl_clear_event_queue(void);
		sdl_clear_event_queue();
#endif

		int i;
		for(i = 0; i < NR_CMD; i ++) {
			if(strcmp(cmd, cmd_table[i].name) == 0) {
				if(cmd_table[i].handler(args) < 0) { return; }
				break;
			}
		}

		if(i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
	}
}
