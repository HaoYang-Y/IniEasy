#ifndef _INI_HANDLER_H_
#define _INI_HANDLER_H_

#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <unordered_map>

namespace multi {
namespace ini {
class IniHandler {
   public:
	using KVStruct = std::unordered_map<std::string, std::string>;

	inline IniHandler(const char *file);
	inline ~IniHandler() = default;

	template <typename T>
	inline T get_section_value(const std::string &section_name, const std::string &key, T defaultValue = T()) const {
		std::string value = has_value(section_name, key);
		if (value.empty()) {
			return defaultValue;
		}

		try {
			return convert<T>(value);
		} catch (const std::exception &e) {
			std::cerr << "转换失败（" << typeid(T).name() << "）：" << e.what() << "，使用默认值：" << defaultValue
					  << std::endl;
			return defaultValue;
		}
	}

	template <typename T>
	inline T get_value(const std::string &key, T defaultValue = T()) const {
		return get_section_value<T>("global", key, defaultValue);
	}

	inline void dump();

   private:
	inline std::string has_value(const std::string &section_name, const std::string &key) const;
	/**
	 * 解析section名称（如[server] -> server）
	 * @param str 原始行内容
	 * @return 处理后的section名称（若格式错误返回空）
	 */
	inline std::string name_handler(const std::string &str);
	inline std::pair<std::string, std::string> kv_handler(std::string &str, int line, bool &has_next);
	inline void trim(std::string &str);
	inline size_t find_comment_outside_quotes(const std::string &str);

	template <typename T>
	T convert(const std::string &value) const;

   private:
	std::unordered_map<std::string, KVStruct> sections_;
};

inline IniHandler::IniHandler(const char *file) {
	std::ifstream ifs(file);
	if (!ifs.is_open()) {
		throw std::runtime_error("无法打开INI文件: " + std::string(file));
	}
	std::string buffer;
	std::string name("global");
	int line = 0;
	bool has_next = false;
	std::string key;
	while (std::getline(ifs, buffer)) {
		++line;
		trim(buffer);
		if (buffer.empty()) {
			continue;
		}

		char first = buffer[0];

		if (has_next && first != '[' && first != ';' && first != '#') {
			sections_[name][key] += kv_handler(buffer, line, has_next).second;
			if (!has_next) {
				trim(sections_[name][key]);
			}
			continue;
		} else {
			has_next = false;
		}

		switch (first) {
			case '[':
				name = name_handler(buffer);
				break;
			case ';':
			case '#':
				break;
			default:
				std::pair<std::string, std::string> kv = kv_handler(buffer, line, has_next);
				if (kv.first.empty()) {
					continue;
				}
				sections_[name][kv.first] = kv.second;
				if (has_next) {
					key = kv.first;
				}
				break;
		}
	}
	ifs.close();
}

inline std::string IniHandler::has_value(const std::string &section_name, const std::string &key) const {
	auto section_iter = sections_.find(section_name);
	if (section_iter == sections_.end()) {
		return std::string();
	}
	auto kv_iter = section_iter->second.find(key);
	if (kv_iter == section_iter->second.end()) {
		return std::string();
	}
	return kv_iter->second;
}

inline std::string IniHandler::name_handler(const std::string &str) {
	std::string session = str;
	size_t index = session.find(']');
	if (index != std::string::npos) {
		session.erase(index);
	}
	session.erase(0, 1);
	trim(session);
	return session;
}

inline std::pair<std::string, std::string> IniHandler::kv_handler(std::string &str, int line, bool &has_next) {
	size_t comment_index = find_comment_outside_quotes(str);
	if (comment_index != std::string::npos) {
		str.erase(comment_index);
	}
	std::string key;
	std::string value;
	if (has_next) {
		has_next = false;
		value = str;
	} else {
		size_t index = str.find("=");
		if (index == std::string::npos) {
			std::cerr << "line:" << line << " Format error\n";
			return {key, value};
		}

		key = str.substr(0, index);
		value = str.substr(index + 1);

		trim(key);
	}
	trim(value);
	if (!value.empty()) {
		// 检查最后一个非空白字符是否为\（避免空白后的\被误判）
		size_t last_non_space = value.find_last_not_of(" \t");
		if (last_non_space != std::string::npos && value[last_non_space] == '\\') {
			has_next = true;
			value.erase(last_non_space);  // 移除续行符\,
			trim(value);				  // 移除\后的空白（确保拼接无冗余空格）
		}
	}
	return std::pair<std::string, std::string>(key, value);
}

inline void IniHandler::trim(std::string &str) {
	if (str.empty()) {
		return;
	}
	auto start = std::find_if(str.begin(), str.end(),
							  [](unsigned int ch) { return !std::isspace(static_cast<unsigned char>(ch)); });
	if (start == str.end()) {
		str.clear();
		return;
	}
	str.erase(str.begin(), start);
	auto end = std::find_if(str.rbegin(), str.rend(), [](unsigned int ch) {
				   return !std::isspace(static_cast<unsigned char>(ch));
			   }).base();
	str.erase(end, str.end());
	if (str.empty()) {
		return;
	}

	const char first = str.front();
	const char last = str.back();
	const bool is_double_quote = (first == '"' && last == '"');
	const bool is_single_quote = (first == '\'' && last == '\'');

	if ((is_double_quote || is_single_quote) && str.size() >= 2) {
		str.erase(str.size() - 1, 1).erase(0, 1);
	}
}

inline size_t IniHandler::find_comment_outside_quotes(const std::string &str) {
	bool in_quote = false;
	char quote_char = '\0';
	for (size_t i = 0; i < str.size(); ++i) {
		if (str[i] == '"' || str[i] == '\'') {
			if (!in_quote) {
				in_quote = true;
				quote_char = str[i];
			} else if (str[i] == quote_char) {
				in_quote = false;
			}
		}
		if (!in_quote && (str[i] == ';' || str[i] == '#')) {
			return i;  // 仅处理引号外的注释
		}
	}
	return std::string::npos;
}

inline void IniHandler::dump() {
	for (const auto &section : sections_) {
		std::cout << "\n[" << section.first << "]" << std::endl;
		for (const auto &kv : section.second) {
			std::cout << "  " << kv.first << " = " << kv.second << std::endl;
		}
	}
}

template <>
int IniHandler::convert<int>(const std::string &value) const {
	return std::stoi(value);
}

template <>
double IniHandler::convert<double>(const std::string &value) const {
	return std::stod(value);
}

template <>
float IniHandler::convert<float>(const std::string &value) const {
	return std::stof(value);
}

template <>
long IniHandler::convert<long>(const std::string &value) const {
	return std::stol(value);
}

template <>
bool IniHandler::convert<bool>(const std::string &value) const {
	std::string lower_value = value;
	std::transform(lower_value.begin(), lower_value.end(), lower_value.begin(), ::tolower);

	return lower_value == "true" ? true : false;
}

template <>
std::string IniHandler::convert<std::string>(const std::string &value) const {
	return value;
}

template <>
const char *IniHandler::convert<const char *>(const std::string &value) const {
	return value.c_str();
}

}  // namespace ini
}  // namespace multi

#endif