
#include "serial.h"
#include <signal.h>
#include <termios.h>
#include <thread>
#include <thread>

class Connection {
private:
	Serial serial_;

public:
	Connection()
	{
	}
public:
	bool open(Serial::Option *option)
	{
		return serial_.open(option);
	}
	void close()
	{
		serial_.close();
	}

	int write(void const *ptr, int len)
	{
		return serial_.write(ptr, len);
	}

	int read(void *ptr, int len)
	{
		return serial_.read(ptr, len);
	}

};

Connection conn;
struct termios term_org;

void onSIGINT(int sig)
{
	tcsetattr(0, TCSANOW, &term_org);
	conn.close();
	exit(99);
}

int main()
{
	Serial::Option opt;
	opt.port = "/dev/ttyUSB0";
	opt.speed = 115200;
	if (!conn.open(&opt)) {
		exit(1);
	}

	signal(SIGINT, onSIGINT);

	struct termios term;
	tcgetattr(0, &term_org);
	term = term_org;
	term. c_lflag &= ~(ICANON | ECHO);
	term.c_cc[VMIN] = 1;
	term.c_cc[VTIME] = 0;
	tcsetattr(0, TCSANOW, &term);

	std::thread rx_thread([&](){
		while (1) {
			char tmp[100];
			int n = conn.read(tmp, 100);
			fwrite(tmp, 1, n, stdout);
			fflush(stdout);
		}
	});

	std::thread tx_thread([&](){
		while (1) {
			char c = getchar();
			conn.write(&c, 1);
		}
	});

	while (1) {
		std::this_thread::yield();
	}

	tcsetattr(0, TCSANOW, &term_org);
	conn.close();
	return 0;
}
