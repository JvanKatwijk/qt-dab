
#include <stdio.h>
#include	<cstring>
#include <stdexcept>

// Exceptions with input
//
class	sdrplay_2_exception : public std::exception {
private:
	std::string message;
public:
	sdrplay_2_exception (const std::string &s) {
	   message	= s;
	}

	const char *what  () const noexcept override {
	   return message. c_str ();
	}
};

class	sdrplay_3_exception : public std::exception {
private:
	std::string message;
public:
	sdrplay_3_exception (const std::string &s) {
	   message	= s;
	}

	const char *what  () const noexcept override {
	   return message. c_str ();
	}
};

class	hackrf_exception : public std::exception {
private:
	std::string message;
public:
	hackrf_exception (const std::string &s) {
	   message	= s;
	}

	const char *what  () const noexcept override {
	   return message. c_str ();
	}
};

class	airspy_exception : public std::exception {
private:
	std::string message;
public:
	airspy_exception (const std::string &s) {
	   message	= s;
	}

	const char *what  () const noexcept override {
	   return message. c_str ();
	}
};

class	rtlsdr_exception : public std::exception {
private:
	std::string message;
public:
	rtlsdr_exception (const std::string &s) {
	   message	= s;
	}

	const char *what  () const noexcept override {
	   return message. c_str ();
	}
};

class	lime_exception : public std::exception {
private:
	std::string message;
public:
	lime_exception (const std::string &s) {
	   message	= s;
	}

	const char *what  () const noexcept override {
	   return message. c_str ();
	}
};

class	pluto_exception : public std::exception {
private:
	std::string message;
public:
	pluto_exception (const std::string &s) {
	   message	= s;
	}

	const char *what  () const noexcept override {
	   return message. c_str ();
	}
};

class	uhd_exception : public std::exception {
private:
	std::string message;
public:
        uhd_exception (const std::string &s) {
           message      = s;
        }

        const char *what  () const noexcept override {
           return message. c_str ();
        }
};

class	colibri_exception : public std::exception {
private:
	std::string message;
public:
        colibri_exception (const std::string &s) {
           message      = s;
        }

        const char *what  () const noexcept override {
           return message. c_str ();
        }
};

class	file_exception : public std::exception {
private:
	std::string message;
public:
        file_exception (const std::string &s) {
           message      = s;
        }

        const char *what  () const noexcept override {
           return message. c_str ();
        }
};

