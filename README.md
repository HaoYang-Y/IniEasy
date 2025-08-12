# IniEasy

一个轻量、单头文件的C++11 INI解析器，专注于读取固定配置，零依赖，简单易用。

## 🌟 特性

- **单头文件 & 零依赖**：只需包含 `ini_handler.h`即可使用，无需额外依赖库，轻松集成到任何项目。
- **专注“读取固定配置”**：针对已知节（section）和键（key）的场景优化，接口简洁无冗余。
- **完善的语法支持**：
  - 自动去除键和值前后的空白（例如 `key  =  value`解析为 `key=value`）。
  - 支持续行符 `\`拼接多行值（类似C语言 `#define`的续行逻辑，自动忽略换行）。
  - 正确处理引号内的特殊字符（单引号 `'`或双引号 `"`内的 `;`和 `#`不视为注释）。
  - 同时支持 `;`和 `#`作为注释标记，且仅识别引号外的注释。
  - 保留引号内的空白（例如 `"  带空格的值  "`解析为 `带空格的值`）。
- **类型安全**：内置 `int`、`double`、`std::string`类型转换，支持自定义默认值。
- **灵活的行为控制**：
  - 节名和键名区分大小写（与多数INI标准一致）。
  - 支持无节名的全局键（自动归为 `[global]`节）。
  - 重复的节/键会被后出现的内容覆盖（符合常见配置文件直觉）。

## 🚀 快速开始

### 安装

将 `ini_handler.hpp`复制到你的项目中，直接包含即可：

```cpp
#include "ini_handler.hpp"
```

### 基础用法

请参考 test 文件夹

### API参考

| 方法                                                                                  | 说明                                                          |
| :------------------------------------------------------------------------------------ | :------------------------------------------------------------ |
| IniHandler(const char* file)                                                          | 构造函数：加载并解析指定的 INI 文件（文件打开失败时抛异常）。 |
| T get_value < T >(key, default)                                                       | 从[global]节读取字符串，若键不存在则返回默认值。T 为基础类型  |
| T get_section_value < T >(section, key, default)                                      | 从指定节读取字符串，若节或键不存在则返回默认值。T 为基础类型  |
| bool has_section(const std::string &section)                                          | 判断该section是否存在                                         |
| bool has_key(const std::string &section, const std::string &key)                      | 判断该section下的key是否存在                                  |
| std::unordered_set[std::string](std::string) get_all_section()                           | 获取配置文件里所有的section name                              |
| std::unordered_set[std::string](std::string) get_section_key(const std::string &section) | 获取section所属的所有key                                      |
| dump()                                                                                | 打印所有解析到的配置（调试用）。                              |

### 行为说明

1. 空白处理：
   - 自动去除键和值前后的空白（如 key = value →key=value）。
   - 保留引号内的空白（如" abc "→ abc ）。
2. 引号处理：
   - 自动去除值外层的单引号或双引号（如'value'→value，"value"→value）。
   - 引号内的;和#视为普通字符（如"val;ue"→val;ue）。
3. 多行值处理：
   - 仅识别带续行符 \ 的多行值（如key=line1\+ 下一行line2→line1line2）。
   - 未带 \ 的换行视为无效行（如缩进的多行文本不会自动拼接）。
4. 错误处理：
   - 无效格式的行（如无=的键值对）会打印错误提示并忽略。
   - 类型转换失败（如"abc"转int）会返回默认值并打印错误。

## ⚠️  局限性

- 不支持写入 INI 文件（专注于读取场景）
- 不处理转义字符（如引号内的\"会被视为普通字符）。
