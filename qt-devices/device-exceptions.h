
#include <stdio.h>
#include	<cstring>
#include <stdexcept>

class	device_exception: public std::exception {
private:
	std::string message;
public:
	device_exception (const std::string &s) {
	   message = s;
	}
	const char * what () const noexcept override {
	   return message. c_str ();
	}
};

class	rtl_tcp_exception : public std::exception {
private:
	std::string message;
public:
	rtl_tcp_exception (const std::string &s) {
           message      = s;
        }

        const char *what  () const noexcept override {
           return message. c_str ();
        }
};

