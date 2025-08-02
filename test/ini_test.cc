#include "../include/ini_handler.hpp"

int main(){
    multi::ini::IniHandler ini("test_ini_parser.ini");
    ini.dump();
}