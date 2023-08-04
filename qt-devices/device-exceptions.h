
#include	<stdio.h>
#include <stdexcept>

#define MAX_MESSAGE_SIZE 256

// Exceptions with input
//
class	sdrplay_2_exception : public std::exception {
private:
	char text [MAX_MESSAGE_SIZE];
public:
	sdrplay_2_exception (const std::string &s) {
	   sprintf (text, "%s ", s. c_str ());
	}

	const char *what  () const noexcept override {
	   return text;
	}
};

class	sdrplay_3_exception : public std::exception {
private:
	char text [MAX_MESSAGE_SIZE];
public:
	sdrplay_3_exception (const std::string &s) {
	   sprintf (text, "%s ", s. c_str ());
	}

	const char *what  () const noexcept override {
	   return text;
	}
};

class	hackrf_exception : public std::exception {
private:
	char text [MAX_MESSAGE_SIZE];
public:
	hackrf_exception (const std::string &s) {
	   sprintf (text, "%s ", s. c_str ());
	}

	const char *what  () const noexcept override {
	   return text;
	}
};

class	airspy_exception : public std::exception {
private:
	char text [MAX_MESSAGE_SIZE];
public:
	airspy_exception (const std::string &s) {
	   sprintf (text, "%s ", s. c_str ());
	}

	const char *what  () const noexcept override {
	   return text;
	}
};

class	rtlsdr_exception : public std::exception {
private:
	char text [MAX_MESSAGE_SIZE];
public:
	rtlsdr_exception (const std::string &s) {
	   sprintf (text, "%s ", s. c_str ());
	}

	const char *what  () const noexcept override {
	   return text;
	}
};

class	lime_exception : public std::exception {
private:
	char text [MAX_MESSAGE_SIZE];
public:
	lime_exception (const std::string &s) {
	   sprintf (text, "%s ", s. c_str ());
	}

	const char *what  () const noexcept override {
	   return text;
	}
};

class	pluto_exception : public std::exception {
private:
	char text [MAX_MESSAGE_SIZE];
public:
	pluto_exception (const std::string &s) {
	   sprintf (text, "%s ", s. c_str ());
	}

	const char *what  () const noexcept override {
	   return text;
	}
};

