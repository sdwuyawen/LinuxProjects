
#ifndef _PAGE_MANAGER_H
#define _PAGE_MANAGER_H

typedef struct PageAction {
	char *name;
	int (*Run)(void);
	int (*GetInputEvent)(PT_Layout atLayout, PT_InputEvent ptInputEvent);
	int (*Prepare)();
	struct PageAction *ptNext;
}T_PageAction, *PT_PageAction;

typedef struct Layout {
	int iTopLeftX;
	int iTopLeftY;
	int iBotRightX;
	int iBotRightY;
	char *strIconName;
}T_Layout, *PT_Layout;

#define ID(name)   (int(name[0]) + int(name[1]) + int(name[2]) + int(name[3]))

#endif /* _PAGE_MANAGER_H */

