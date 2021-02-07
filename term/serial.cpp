#include "serial.h"

#ifdef _WIN32
	#include <Windows.h>
	typedef HANDLE serial_handle_t;
	#define DEFAULT_COM_PORT "\\\\.\\COM1"
	#pragma warning(disable:4996)
#else
	#include <unistd.h>
	#include <fcntl.h>
	#include <termios.h>
	#include <sys/stat.h>
	typedef int serial_handle_t;
#endif

struct Serial::Handle {
	serial_handle_t fd;
	Handle(serial_handle_t fd)
		: fd(fd)
	{
	}
};

Serial::Serial()
{
}

bool Serial::open(Option *option)
{
	option_ = *option;
#ifdef _WIN32
	serial_handle_t fd;
	DCB dcb;
	fd = CreateFileA(option_.port.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (fd == INVALID_HANDLE_VALUE) {
		return false;
	}

	GetCommState(fd, &dcb);
	dcb.BaudRate = option_.speed;
	dcb.fBinary = TRUE;
	dcb.fParity = FALSE;
	dcb.fOutxCtsFlow = FALSE;
	dcb.fOutxDsrFlow = FALSE;
	dcb.fDtrControl = DTR_CONTROL_ENABLE;
	dcb.fDsrSensitivity = FALSE;
	dcb.fTXContinueOnXoff = FALSE;
	dcb.fOutX = FALSE;
	dcb.fInX = FALSE;
	dcb.fErrorChar = FALSE;
	dcb.fNull = FALSE;
	dcb.fRtsControl = RTS_CONTROL_ENABLE;
	dcb.fAbortOnError = FALSE;
	dcb.ByteSize = 8;
	dcb.Parity = NOPARITY;
	dcb.StopBits = ONESTOPBIT;
	SetCommState(fd, &dcb);

	handle_ = std::make_shared<Handle>(fd);
	return true;
#else
	int speed;
	serial_handle_t fd;
	struct termios attr;

	fd = ::open(option->port.c_str(), O_RDWR | O_NOCTTY);
	if (fd < 0) {
		return false;
	}

	tcgetattr(fd, &attr);
	option->saveattr = attr;

	switch (option->speed) {
//	case      50:	speed =      B50;		break;
//	case      75:	speed =      B75;		break;
//	case     110:	speed =     B110;		break;
//	case     134:	speed =     B134;		break;
//	case     150:	speed =     B150;		break;
// 	case     200:	speed =     B200;		break;
	case     300:	speed =     B300;		break;
	case     600:	speed =     B600;		break;
	case    1200:	speed =    B1200;		break;
//	case    1800:	speed =    B1800;		break;
	case    2400:	speed =    B2400;		break;
	case    4800:	speed =    B4800;		break;
	case    9600:	speed =    B9600;		break;
	case   19200:	speed =   B19200;		break;
	case   38400:	speed =   B38400;		break;
	case   57600:	speed =   B57600;		break;
	case  115200:	speed =  B115200;		break;
	case  230400:	speed =  B230400;		break;
//	case  460800:	speed =  B460800;		break;
//	case  500000:	speed =  B500000;		break;
//	case  576000:	speed =  B576000;		break;
//	case  921600:	speed =  B921600;		break;
//	case 1000000:	speed = B1000000;		break;
//	case 1152000:	speed = B1152000;		break;
//	case 1500000:	speed = B1500000;		break;
//	case 2000000:	speed = B2000000;		break;
//	case 2500000:	speed = B2500000;		break;
//	case 3000000:	speed = B3000000;		break;
//	case 3500000:	speed = B3500000;		break;
//	case 4000000:	speed = B4000000;		break;
	default:
		speed = B38400;
		break;
	}

	cfsetispeed(&attr, speed);
	cfsetospeed(&attr, speed);
	cfmakeraw(&attr);

	attr.c_cflag &= ~CSIZE;
	attr.c_cflag |= CS8 | CLOCAL | CREAD;
	attr.c_iflag = 0;
	attr.c_oflag = 0;
	attr.c_lflag = 0;
	attr.c_cc[VMIN] = 1;
	attr.c_cc[VTIME] = 0;

	tcsetattr(fd, TCSANOW, &attr);

	handle_ = std::make_shared<Handle>(fd);
	return true;
#endif
}

void Serial::close()
{
#ifdef _WIN32
	CloseHandle(handle_->fd);
#else
	tcsetattr(handle_->fd, TCSANOW, &option_.saveattr);
	::close(handle_->fd);
#endif
	handle_.reset();
}

int Serial::write(const void *ptr, int len)
{
#ifdef _WIN32
	DWORD bytes = 0;
	if (WriteFile(handle_->fd, ptr, len, &bytes, NULL)) {
		return bytes;
	}
	DWORD e = GetLastError();
	return 0;
#else
	return ::write(handle_->fd, ptr, len);
#endif
}

int Serial::read(void *ptr, int len)
{
	unsigned long bytes;
#ifdef _WIN32
	bytes = 0;
	if (ReadFile(handle_->fd, ptr, len, &bytes, NULL)) {
		return bytes;
	}
	return 0;
#else
	return ::read(handle_->fd, ptr, len);
#endif
}
