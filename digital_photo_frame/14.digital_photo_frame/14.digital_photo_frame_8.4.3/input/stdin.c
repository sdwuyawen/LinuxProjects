#include <input_manager.h>
#include <termios.h>
#include <unistd.h>
#include <stdio.h>

static int StdinDevInit(void);
static int StdinDevExit(void);
static int StdinGetInputEvent(PT_InputEvent ptInputEvent);


static T_InputOpr g_tStdinOpr = {
	.name          = "stdin",
	.DeviceInit    = StdinDevInit,
	.DeviceExit    = StdinDevExit,
	.GetInputEvent = StdinGetInputEvent,
};

static int StdinDevInit(void)
{
	struct termios tTTYState;

	//get the terminal state
	tcgetattr(STDIN_FILENO, &tTTYState);

	//turn off canonical mode
	tTTYState.c_lflag &= ~ICANON;
	//minimum of number input read.
	tTTYState.c_cc[VMIN] = 1;   /* 有一个数据时就立刻返回 */

	//set the terminal attributes.
	tcsetattr(STDIN_FILENO, TCSANOW, &tTTYState);

	return 0;
}

static int StdinDevExit(void)
{
	struct termios tTTYState;

	//get the terminal state
	tcgetattr(STDIN_FILENO, &tTTYState);

	//turn on canonical mode
	tTTYState.c_lflag |= ICANON;

	//set the terminal attributes.
	tcsetattr(STDIN_FILENO, TCSANOW, &tTTYState);

	return 0;
}

static int StdinGetInputEvent(PT_InputEvent ptInputEvent)
{
	char c;

	/* 处理数据 */
	ptInputEvent->iType = INPUT_TYPE_STDIN;

	/* 会休眠直到有输入 */
	c = fgetc(stdin);
	gettimeofday(&ptInputEvent->tTime, NULL);
#if 0	
	if (c == 'u')
	{
		ptInputEvent->iVal = INPUT_VALUE_UP;
	}
	else if (c == 'n')
	{
		ptInputEvent->iVal = INPUT_VALUE_DOWN;
	}
	else if (c == 'q')
	{
		ptInputEvent->iVal = INPUT_VALUE_EXIT;
	}
	else
	{
		ptInputEvent->iVal = INPUT_VALUE_UNKNOWN;
	}
#endif	
	return 0;
}

int StdinInit(void)
{
	return RegisterInputOpr(&g_tStdinOpr);
}

