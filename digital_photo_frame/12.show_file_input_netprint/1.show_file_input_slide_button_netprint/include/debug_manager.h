#ifndef _DEBUG_MANAGER_H
#define _DEBUG_MANAGER_H

#define	KERN_EMERG	"<0>"	/* system is unusable			*/
#define	KERN_ALERT		"<1>"	/* action must be taken immediately	*/
#define	KERN_CRIT		"<2>"	/* critical conditions			*/
#define	KERN_ERR		"<3>"	/* error conditions			*/
#define	KERN_WARNING	"<4>"	/* warning conditions			*/
#define	KERN_NOTICE	"<5>"	/* normal but significant condition	*/
#define	KERN_INFO		"<6>"	/* informational			*/
#define	KERN_DEBUG	"<7>"	/* debug-level messages			*/

#define DEFAULT_DBGLEVEL  4

typedef struct DebugOpr {
	char *name;
	int isCanUse;
	int (*DebugInit)(void);
	int (*DebugExit)(void);
	int (*DebugPrint)(char *strData);	
	struct DebugOpr *ptNext;
}T_DebugOpr, *PT_DebugOpr;


int DebugInit(void);
int InitDebugChannels(void);
int StdoutInit(void);
int NetPrintInit(void);
int RegisterDebugOpr(PT_DebugOpr ptDebugOpr);
void ShowDebugOpr(void);
PT_DebugOpr GetDebugOpr(char *pcName);
int SetDbgLevel(char *strBuf);
int SetDbgChannel(char *strBuf);
int DebugPrint(const char *pcFormat, ...);
int NetDbgFlush(void);


#endif