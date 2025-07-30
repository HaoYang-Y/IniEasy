#ifndef _INI_HANDLER_H_
#define _INI_HANDLER_H_
#include <unordered_map>
#include <string>
#include <fstream>

namespace multi
{
namespace ini{
class IniHandler
{
private:
    std::unordered_map<std::string,std::string> kv_;
public:
    IniHandler(const char* file);
    ~IniHandler() = default;
};

IniHandler::IniHandler(const char* file)
{
    std::ifstream ifs(file);
    std::string buffer;
    while (std::getline(ifs,buffer))
    {
        /* code */
    }
    
}

} // namespace ini
} // namespace multi

#endif