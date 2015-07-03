#include <input_manager.h>
#include <termios.h>
#include <unistd.h>
#include <stdio.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

static int g_fd_button;

static int ButtonDevInit(void);
static int ButtonDevExit(void);
static int ButtonGetInputEvent(PT_InputEvent ptInputEvent);


static T_InputOpr g_tButtonOpr = {
	.name          = "button",
	.DeviceInit    = ButtonDevInit,
	.DeviceExit    = ButtonDevExit,
	.GetInputEvent = ButtonGetInputEvent,
};

static int ButtonDevInit(void)
{
//	g_fd_button = open("/dev/buttons", O_RDWR | O_NONBLOCK);

	/* 以阻塞方式打开 */
	g_fd_button = open("/dev/buttons", O_RDWR);
	if (g_fd_button < 0)
	{
		printf("can't open /dev/buttons\n");
		return -1;
	}
	
	return 0;
}

static int ButtonDevExit(void)
{
	return 0;
}

static int ButtonGetInputEvent(PT_InputEvent ptInputEvent)
{
	/* 如果有数据就读取、处理、返回
	 * 如果没有数据, 立刻返回, 不等待
	 */

	int ret;
	unsigned char key_val;
	
	ret = read(g_fd_button, &key_val, 1);
	if(ret == 1)
	{
//		printf("key_val: 0x%x, ret = %d\n", key_val, ret);
		if (key_val == 0x01)
		{
			ptInputEvent->iKey = INPUT_VALUE_UP;
		}
		else if (key_val == 0x02)
		{
			ptInputEvent->iKey = INPUT_VALUE_DOWN;
		}
		else if (key_val == 0x03)
		{
			ptInputEvent->iKey = INPUT_VALUE_EXIT;
		}
		else if (key_val == 0x04)
		{
			ptInputEvent->iKey = INPUT_VALUE_UNKNOWN;
		}
		else if (key_val == 0x05)
		{
			ptInputEvent->iKey = INPUT_VALUE_UNKNOWN;
		}
		else
		{
			return -1;
		}
		return 0;
	}
	else
	{
		return -1;
	}
//	printf("key_val: 0x%x, ret = %d\n", key_val, ret);
}

int ButtonInit(void)
{
	return RegisterInputOpr(&g_tButtonOpr);
}


