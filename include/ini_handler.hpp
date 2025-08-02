#ifndef _INI_HANDLER_H_
#define _INI_HANDLER_H_
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <cctype>

namespace multi
{
namespace ini{
class IniHandler
{
public:
    using KVStruct=std::unordered_map<std::string,std::unordered_set<std::string>>;

    IniHandler(const char* file);
    ~IniHandler() = default;
    
    std::string get_value(const std::string& section_name ,const std::string& key ,const std::string& defaultValue = std::string()){
    }
    
    std::string get_value(const std::string& key ,const std::string& defaultValue = std::string()){
        return get_value("global",key ,defaultValue);
    }
    void dump();
private:
    std::string name_handler(const std::string& str);
    std::pair<std::string,std::string> kv_handler(std::string& str,int line);
    std::string trim(const std::string& str);
private:
    std::unordered_map<std::string,KVStruct> section_;
};

IniHandler::IniHandler(const char* file)
{
    std::ifstream ifs(file);
    std::string buffer;
    std::string name("global");
    int line = 0;
    while (std::getline(ifs,buffer))
    {
        ++line;
        if(buffer.empty()){continue;}
        char first = buffer[0];
        if(first == ' '){
            buffer = trim(buffer);
            if(buffer.empty()){
                continue;
            }
            first = buffer[0];
        }
        if(first == '['){name = name_handler(buffer);}
        else if(first == ';' |first == '#' ){continue;}
        else {
            std::pair<std::string,std::string> kv = kv_handler(buffer,line);
            section_[name][kv.first].insert(kv.second);
        }
    }
    ifs.close(); 
}
std::string IniHandler::name_handler(const std::string& str){
    size_t index = str.find(']');
    if(index == std::string::npos){
        return str.substr(1,str.size()+1);
    }
    return str.substr(1,index-1);
}
std::pair<std::string,std::string> IniHandler::kv_handler(std::string& str,int line){
    size_t comment_index = str.find(";");
    if(comment_index != std::string::npos){
        str = str.substr(0,comment_index);
    }
    comment_index = str.find("#");
    if(comment_index != std::string::npos){
        str = str.substr(0,comment_index);
    }
    std::string key;
    std::string value;
    size_t index = str.find("=");
    if(index == std::string::npos){
        std::cerr << "line:" << line << " Format error\n";
    }
    key = trim(str.substr(0,index));
    value = trim(str.substr(index+1,str.size()+1));
    return std::pair<std::string,std::string>(key,value);
}
std::string IniHandler::trim(const std::string& str){
    auto start = std::find_if(str.begin(), str.end(), 
        [](int ch) { return !std::isspace(ch); });
    if(start == str.end()){return "";}
    auto end = std::find_if(str.rbegin(), str.rend(), 
        [](int ch) { return !std::isspace(ch); }).base();
    
    return std::string(start, end);
}
void IniHandler::dump(){
    for(auto& session : section_){
        std::cout <<"section==========star\n";
        std::cout <<session.first <<"\n";
        for(auto& kv :session.second){
            std::cout << "key:"<<kv.first <<"\nvalue:";
            for(auto& v : kv.second){
                std::cout <<v<<"、";
            }
            std::cout<<"\n";
        }
        std::cout <<"section==========end\n";
    }
}
} // namespace ini
} // namespace multi

#endif