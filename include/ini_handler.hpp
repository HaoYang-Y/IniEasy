#ifndef _INI_HANDLER_H_
#define _INI_HANDLER_H_

#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <unordered_set>

namespace multi {
namespace ini {
class IniHandler {
   public:
	using KVStruct = std::unordered_map<std::string, std::string>;

	inline IniHandler(const char* file);
	inline ~IniHandler() = default;

	inline std::string get_section_value(std::string section_name, std::string key,
										 std::string defaultValue = "") const;

	inline int get_section_int(std::string section_name, std::string key, int defaultValue = 0);
	inline double get_section_double(std::string section_name, std::string key, double defaultValue = 0);

	inline std::string get_value(std::string key, std::string defaultValue = std::string()) const;

	inline int get_int(std::string key, int defaultValue = 0);
	inline double get_double(std::string key, double defaultValue = 0);

	inline void dump();

   private:
	/**
	 * 解析section名称（如[server] -> server）
	 * @param str 原始行内容
	 * @return 处理后的section名称（若格式错误返回空）
	 */
	inline std::string name_handler(const std::string& str);
	inline std::pair<std::string, std::string> kv_handler(std::string& str, int line);
	inline void trim(std::string& str);

   private:
	std::unordered_map<std::string, KVStruct> section_;
};

inline IniHandler::IniHandler(const char* file) {
	std::ifstream ifs(file);
	std::string buffer;
	std::string name("global");
	int line = 0;
	while (std::getline(ifs, buffer)) {
		++line;
		if (buffer.empty()) {
			continue;
		}
		char first = buffer[0];
		if (first == ' ') {
			trim(buffer);
			if (buffer.empty()) {
				continue;
			}
			first = buffer[0];
		}
		if (first == '[') {
			name = name_handler(buffer);
		} else if (first == ';' | first == '#') {
			continue;
		} else {
			std::pair<std::string, std::string> kv = kv_handler(buffer, line);
			section_[name][kv.first] = kv.second;
		}
	}
	ifs.close();
}

inline std::string IniHandler::get_section_value(std::string section_name, std::string key,
												 std::string defaultValue) const {
	auto section_iter = section_.find(section_name);
	if (section_iter == section_.end()) {
		return defaultValue;
	}
	auto kv_iter = section_iter->second.find(key);
	if (kv_iter == section_iter->second.end()) {
		return defaultValue;
	}
	return kv_iter->second;
}

inline int IniHandler::get_section_int(std::string section_name, std::string key, int defaultValue) {
	try {
		std::string tmp = get_section_value(section_name, key);
		return tmp == " " ? defaultValue : std::stoi(tmp);
	} catch (const std::invalid_argument& e) {
		std::cerr << "转换int失败（无效参数）：" << e.what() << std::endl;
		return defaultValue;
	} catch (const std::out_of_range& e) {
		std::cerr << "转换int失败（超出范围）：" << e.what() << std::endl;
		return defaultValue;
	}
}

inline double IniHandler::get_section_double(std::string section_name, std::string key, double defaultValue) {
	try {
		std::string tmp = get_section_value(section_name, key);
		return tmp == " " ? defaultValue : std::stod(tmp);
	} catch (const std::exception& e) {
		std::cerr << "转换double失败：" << e.what() << std::endl;
		return defaultValue;
	}
}

inline std::string IniHandler::get_value(std::string key, std::string defaultValue) const {
	std::string section_name("global");
	return get_section_value(section_name, key, defaultValue);
}
inline std::string IniHandler::name_handler(const std::string& str) {
	size_t index = str.find(']');
	if (index == std::string::npos) {
		return str.substr(1, str.size() + 1);
	}
	return str.substr(1, index - 1);
}

inline int IniHandler::get_int(std::string key, int defaultValue) {
	try {
		std::string tmp = get_value(key);
		return tmp == " " ? defaultValue : std::stoi(tmp);
	} catch (const std::invalid_argument& e) {
		std::cerr << "转换int失败（无效参数）：" << e.what() << std::endl;
		return defaultValue;
	} catch (const std::out_of_range& e) {
		std::cerr << "转换int失败（超出范围）：" << e.what() << std::endl;
		return defaultValue;
	}
}

inline double IniHandler::get_double(std::string key, double defaultValue) {
	try {
		std::string tmp = get_value(key);
		return tmp == " " ? defaultValue : std::stod(tmp);
	} catch (const std::exception& e) {
		std::cerr << "转换double失败：" << e.what() << std::endl;
		return defaultValue;
	}
}

inline std::pair<std::string, std::string> IniHandler::kv_handler(std::string& str, int line) {
	size_t comment_index = str.find(";");
	if (comment_index != std::string::npos) {
		str = str.substr(0, comment_index);
	}
	comment_index = str.find("#");
	if (comment_index != std::string::npos) {
		str = str.substr(0, comment_index);
	}

	size_t index = str.find("=");
	if (index == std::string::npos) {
		std::cerr << "line:" << line << " Format error\n";
	}

	std::string key = str.substr(0, index);
	std::string value = str.substr(index + 1, str.size() + 1);

	trim(key);
	trim(value);
	return std::pair<std::string, std::string>(key, value);
}

inline void IniHandler::trim(std::string& str) {
	if (str.empty()) {
		return;
	}
	auto start = std::find_if(str.begin(), str.end(), [](int ch) { return !std::isspace(ch); });
	if (start == str.end()) {
		return str.clear();
	}
	auto end = std::find_if(str.rbegin(), str.rend(), [](int ch) { return !std::isspace(ch); }).base();
	str = std::string(start, end);
	if (str.empty()) {
		return;
	}

	const char first = str.front();
	const char last = str.back();
	const bool is_double_quote = (first == '"' && last == '"');
	const bool is_single_quote = (first == '\'' && last == '\'');

	if ((is_double_quote || is_single_quote) && str.size() >= 2) {
		str = str.substr(1, str.size() - 2);
	}
}

inline void IniHandler::dump() {
	for (auto& section : section_) {
		std::cout << section.first << "\n";
		for (auto& kv : section.second) {
			std::cout << "key:" << kv.first << "value:" << kv.second << "\n";
		}
	}
}
}  // namespace ini
}  // namespace multi

#endif