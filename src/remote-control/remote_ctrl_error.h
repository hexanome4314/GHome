#ifndef __REMOTE_CTRL_ERROR_H__
#define __REMOTE_CTRL_ERROR_H__

enum {
	RCTRL_OK = 0,
	RCTRL_CANNOT_OPEN_SOCKET = -1,
	RCTRL_CANNOT_BIND_SOCKET = -2,
	RCTRL_CANNOT_LAUNCH_ACCEPT_THREAD = -3,
	RCTRL_COMMAND_OK = 1,
	RCTRL_COMMAND_LOGOUT = 0,
	RCTRL_UNKNOWN_COMMAND = -4,
	RCTRL_CANNOT_ADD_COMMAND = -5
};

#endif // __REMOTE_CTRL_ERROR_H__
