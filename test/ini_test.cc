#include "ini_handler.hpp"

int main() {
	multi::ini::IniHandler ini("test_ini_parser.ini");

	std::cout << ini.get_section_value("db.redis", "port") << "\n";
	std::cout << ini.get_section_int("db.redis", "port") << "\n";
}