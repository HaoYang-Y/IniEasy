#include "ini_handler.hpp"

int main() {
	try {
		// 1. 构造函数：加载INI文件
		// 注意：文件打开失败会抛出std::runtime_error异常
		multi::ini::IniHandler ini("test_ini_parser.ini");

		// 2. 打印所有解析结果（调试用）
		std::cout << "===== 完整配置内容 =====" << std::endl;
		ini.dump();	 // 调用优化后的dump方法，输出格式化配置

		// 3. 读取全局节（[global]）配置
		std::cout << "\n===== 全局节API测试 =====" << std::endl;

		// 3.1 读取字符串值
		std::string version = ini.get_value<std::string>("version", "unknown");
		std::cout << "全局版本号: " << version << std::endl;

		// 3.2 读取int值
		int timeout = ini.get_value<int>("timeout", 60);  // 若键不存在，返回60
		std::cout << "全局超时时间: " << timeout << "秒" << std::endl;

		// 3.3 读取double值
		double pi = ini.get_value<double>("pi", 3.14);
		std::cout << "圆周率: " << pi << std::endl;

		// 4. 读取指定节配置
		std::cout << "\n===== 指定节API测试 =====" << std::endl;

		// 4.1 读取[server]节字符串
		std::string server_host = ini.get_section_value<std::string>("server", "host", "127.0.0.1");

		std::cout << "服务器地址: " << server_host << std::endl;

		// 4.2 读取[server]节int值
		int server_port = ini.get_section_value<int>("server", "port", 80);
		std::cout << "服务器端口: " << server_port << std::endl;

		// 5. 读取嵌套节配置
		std::cout << "\n===== 嵌套节API测试 =====" << std::endl;

		// 5.1 读取[db.redis]节配置
		std::string redis_host = ini.get_section_value<std::string>("db.redis", "host");
		int redis_port = ini.get_section_value<int>("db.redis", "port");
		std::cout << "Redis地址: " << redis_host << ":" << redis_port << std::endl;

		// 5.2 读取[db.mysql]节配置（带特殊字符的值）
		std::string mysql_pwd = ini.get_section_value<std::string>("db.mysql", "password");
		std::cout << "MySQL密码: " << mysql_pwd << std::endl;  // 应保留值中的分号

		// 6. 测试默认值机制（访问不存在的键）
		std::cout << "\n===== 默认值机制测试 =====" << std::endl;

		// 6.1 不存在的节
		std::string invalid_section_val = ini.get_section_value("invalid_section", "key", "默认字符串");
		std::cout << "不存在的节返回默认值: " << invalid_section_val << std::endl;

		// 6.2 存在的节但不存在的键
		int invalid_key_int = ini.get_section_value<int>("server", "invalid_key", 10086);
		std::cout << "不存在的键返回默认值: " << invalid_key_int << std::endl;

		// 7. 测试类型转换
		std::cout << "\n===== 类型转换测试 =====" << std::endl;

		// 7.1 字符串转int（正常转换）
		int db_port = ini.get_section_value<int>("db.mysql", "port", 3306);
		std::cout << "MySQL端口（int）: " << db_port << std::endl;

		// 7.2 字符串转double（正常转换）
		double db_timeout = ini.get_section_value<double>("db.mysql", "timeout", 30.5);
		std::cout << "MySQL超时（double）: " << db_timeout << std::endl;

		// 8 判断section和key是否存在
		std::cout << "\n===== 判断section或key是否存在 =====" << std::endl;
		std::cout << "db.mysql是否存在" << ini.has_section("db.mysql") << std::endl;
		std::cout << "db.mysql下port是否存在" << ini.has_key("db.mysql", "port") << std::endl;
		// 9 获取section和key
		std::cout << "\n===== 获取section和key =====" << std::endl;
		std::unordered_set<std::string> sections = ini.get_all_section();
		for (auto& section : sections) {
			std::cout << section << " ";
		}
		std::cout << std::endl;

		std::unordered_set<std::string> keys = ini.get_section_key("db.mysql");
		for (auto& key : keys) {
			std::cout << key << " ";
		}
		std::cout << std::endl;
	} catch (const std::exception& e) {
		// 捕获所有异常（如文件打开失败）
		std::cerr << "\n发生错误: " << e.what() << std::endl;
		return 1;
	}

	return 0;
}