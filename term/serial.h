#ifndef SERIAL_H
#define SERIAL_H

#include <string>
#include <memory>

#ifndef _WIN32
#include <termios.h>
#endif

class Serial {
public:
	struct Handle;
	struct Option {
		std::string port;
		int speed;
#ifndef _WIN32
		struct termios saveattr;
#endif
	};
	Option option_;
	std::shared_ptr<Handle> handle_;
public:
	Serial();
	bool open(Option *option);
	void close();
	int write(const void *ptr, int len);
	int read(void *ptr, int len);
};

#endif // SERIAL_H
