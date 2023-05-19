

#include <stdexcept>

#define MAX_MESSAGE_SIZE 256

// Exceptions with input
//
class	sdrplay_2_exception : public std::exception {
private:
	std::string message;
public:
	sdrplay_2_exception (const std::string &s) {
	   message = s;
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
	   message = s;
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
	   message = s;
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
	   message = s;
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
	   message = s;
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
	   message = s;
	}

	const char *what  () const noexcept override {
	   return message. c_str ();
	}
};

