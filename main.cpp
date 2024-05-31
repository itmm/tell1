// The following declarations are needed to compile the main() function.

static int do_command();
static bool is_eof(int);
static int next(int);
static void write(int);

#include <iostream>

// main() shows the overview:
// * read all chars and write them out
// * process commands, if a '@' is read
// * bail out on errors

int main() {
	try {
		int ch { next(' ') };
		while (!is_eof(ch)) {
			if (ch == '@') { ch = do_command(); }
			else { write(ch); ch = next(ch); }
		}
	}
	catch (const std::runtime_error& err) {
		std::cerr << err.what() << "\n";
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}

// Keep track of how many characters are written. This is used as the values of declared
// variables.

static size_t pos { 0 };

static void write(char chr) {
	std::cout.put(chr); ++pos;
	if (! pos) { throw std::runtime_error { "output too long" }; }
}

static inline void write(int ch) { write(static_cast<char>(ch)); }

// Use std::stream definition of end of file.

static bool is_eof(int ch) { return ch == decltype(std::cin)::traits_type::eof(); }

// Read next character, if end of file is not reached

static int next(int ch) { if (! is_eof(ch)) { ch = std::cin.get(); } return ch; }

// Needed to compile do_command().

void write_pos(size_t value);

#include <map>
#include <sstream>

static std::map<std::string, size_t> vars;

// Look at the next character after '@':
// * '@': write a '@'
// * '!': set a variable with the following name to the current position
// * '?': write out the position of a variable in decimal notation

static inline int do_command() {
	int ch { next('@') };
	if (is_eof(ch)) { throw std::runtime_error { "'@' at end of file" }; }
	switch (ch) {
		case '@': write(ch); ch = next(ch); break;
		case '!': case '?': {
			bool set { ch == '!' };
			std::string name;
			ch = next(ch);
			while (isalnum(ch) || ch == '_') {
				name += static_cast<char>(ch); ch = next(ch);
			}
			while (!is_eof(ch) && ch <= ' ') { ch = next(ch); }
			if (ch == '@') { ch = next(ch); }
			if (set) {
				vars[name] = pos;
			} else {
				auto got { vars.find(name) };
				if (got == vars.end()) {
					std::ostringstream msg;
					msg << "var '" << name << "' used before declaration";
					throw std::runtime_error(msg.str());
				}
				write_pos(got->second);
			}
			break;
		}
		default: {
			std::ostringstream msg;
			msg << "unknown command '" << static_cast<char>(ch) << "' after '@'";
			throw std::runtime_error(msg.str());
		}
	}
	return ch;
}

// Recursive version to write out non-negative decimal.

void write_pos(size_t value) {
	auto rest { value / 10 };
	if (rest) { write_pos(rest); }
	write(static_cast<char>('0' + (value % 10)));
}

