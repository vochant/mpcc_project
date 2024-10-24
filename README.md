# MPCC Project

## 翻译

[English](README.md) | 中文 

## 介绍

> [!CAUTION]
> MPCC Project 当前仍未完成编写，您也许不能按照给定的方式编译和运行。

### MPC

MPC 是一种脚本式、半解释性、强类型的编程语言，使用 MPASM 二进制存储代码逻辑、。MPC 是一款面向对象的编程语言，支持类、封装、构造函数、析构函数、继承、多态等功能。

```mpc
class Integer {
    private value _value = 0;
    constructor(v) {
        _value = v;
        print("An integer class created."); // library "io" required.
    }
    public method isok = function() {
        return typestr(_value) == "integer"; // library "type-utils" required.
    }
    destructor {
        print("An integer calss destructed.");
    }
    public method valueOf() {
        return _value;
    }
}

let a = new Integer(114514);
```

### MPCC Project

MPCC Project 是一款使用 C++ 编程语言的跨平台半解释性编程语言。MPC 表示 Mirekintoc PL Code。它的代码精简，不需要太长的时间用于理解，同时也易于修改、装载插件。

## 构建 & 运行

### 支持的系统

MPCC Project 当前支持绝大多数常见的操作系统，包括但不限于 Windows、macOS、Linux， 并且可以基于 Windows API 或 POSIX API 加载外置的插件。这种插件不需要更改 MPCC Project 的源代码即可运行，而是被 MPCC Project 从本地动态库读取而运行。这不是一个必须的功能，但通常会启用它，当然你也可以禁用它。另外，请注意您选中的插件是否跨平台。理论上没有安装插件的 MPCC Project 可以在任意支持 CMake 和 C++17 的操作系统上运行。

MPCC Project 并不依赖于某种特定的指令集，常见的，例如 x86、arm、RISC-V、PowerPC 都可以运行，只要符合上一条。

由于 coredef 等功能使用 `Module` 类进行解析依赖于 `nlohmann_json` 库，如果您的系统不兼容该库，请尝试在 `CMakeList.txt` 中删除该项，并将 `NO_MODULE` 改为 `true`。请注意，这将导致标准库不可外部更改。如果移动程序时没有一并移动 `res` 目录，则会产生同样的效果。

### 构建

在开始前，您需要安装 CMake 和对应的编译套件，然后运行以下命令：

```shell
cmake -B build -S . -D BUILD_TYPE=Release
cmake --build build --config Release
```

### 运行

运行一个代码文件：

```shell
mpcc e <fileName>
```

以 REPL (Read-Evaluate-Print Loop) 模式运行：

```shell
mpcc r
```

运行一个二进制文件 (`*.mpe`)：

```shell
mpcc x <fileName>
```

创建一个二进制文件 (`*.mpe`)：

```shell
mpcc c <inputFileName> <outputFileName>
```

把代码转换为 TokenList (`.tokens.txt`)：

```shell
mpcc s <inputFileName> <outputFileName>
```

把代码转换为 AST JSON (`.ast.json`)：

```shell
mpcc t <inputFileName> <outputFileName>
```

把代码文件转换为 MPASM (`.mpasm` or `.mps`)：

```shell
mpcc a <inputFileName> <outputFileName>
```

**注意**：您不能同时使用多种模式！

### 模块

1. **Token 解析器 (Lexer)** 用于划分源代码部件
2. **语法解析器 (Parser)** 用于解析源代码为 AST
3. **编译器 (Compiler)** 内置于 AST 节点中，用于输出 MPASM
4. **连接器 (Linker)** 用于更改 `JMP` 系列语句的指向
5. **虚拟机 (VM)** 用于运行 MPASM
6. **转换器 (Convertor)** 用于将二进制/内存格式转换为人类可读形式。

二进制文件的 Magic 是 `MirekE09`

## 插件

要创建一个内置插件，您需要：

### 创建文件夹

创建一个位于 `include/registry` 目录的文件，例如 `my_plugin.hpp`。

### 编写代码

写入下述内容。示例是对若干个整数求和，您也可以换成自己想要的。

```cpp
#pragma once

#include "registry/base.hpp"

class MyPlugin : public Plugin {
public:
   MyPlugin() : Plugin() {}
   void attach(std::shared_ptr<Environment> env) const override {
       env->set("sum", std::make_shared<NativeFunction>(sum));
   }
public:
   static NativeFunction::resulttype sum(NativeFunction::arglist args, Environment* env) {
       if (args.size() == 0) {
           return FormatError();
       }
       long long _sum = 0;
       for (auto i : args) {
           if (i->type != Object::Type::Integer) {
               return FormatError();
           }
           _sum += std::dynamic_pointer_cast<Integer>(i)->value;
       }
       return std::make_pair(NativeFunction::Result::OK,std::make_shared<Integer>(_sum));
   }
};
```

`NativeFunction::resulttype` 表示 `std::pair<NativeFunctions::Result, std::shared_ptr<Object>>`.

`NativeFunction::arglist` 表示 `std::vector<std::shared_ptr<Object>>`.

枚举 `NativeFunction::Result` 包含 4 个值： `OK`，`FORMAT_ERR`，`DATA_ERR` 和 `UNHANDLED_ERR`。它们分别表示：

- `OK`：没有错误发生
- `FORMAT_ERR`：以错误的格式调用
- `DATA_ERR`：以错误的参数数据调用
- `UNHANDLED_ERR`：未经处理的错误

### 修改文件

修改 `include/registry/base/selection.hpp`:

```diff
--- selection.hpp (old)
+++ selection.hpp (new)
@@ -3,6 +3,7 @@
    // Include your plugin there
    #include "registry/dynamic_load.hpp"
    #include "registry/io.hpp"
    #include "registry/constants.hpp"
+++ #include "registry/my_plugin.hpp"
@@ -16,19 +16,20 @@
    // Load your plugin there.
    _target->loadLibrary(std::make_shared<DLoadPlugin>());
    _target->loadLibrary(std::make_shared<IOPlugin>());
    _target->loadLibrary(std::make_shared<ConstantsPlugin>());
+++ _target->loadLibrary(std::make_shared<MyPlugin>());
```

### 重新构建

最后重新构建整个项目。

## 功能

### 语言

MPC 提供多种类型。除 NaitveData、Instance 和 Class 具有不确定的类型字段，其他类型（Integer、String、Float、Boolean、Function、NativeFunction、Null、Error）都有固定了类型字段。

`type-utils` 库（`include/registry/typeutils.hpp`）提供的 `typestr` 函数可以获取一个值的类型。一个类和它的实例具有相同的编号（不是完整的类型字段）。

支持运算：

- 算数运算：加、减、乘、除、取模、正、负
- 位运算：按位与、按位或、按位取反、按位异或、左移、右移
- 逻辑运算：与、或、非
- 比较运算：等于、不等于、大于、小于、大于等于、小于等于

### 支持库

| 名称 | 文件 | 功能 | 操作系统 |
| :--: | :--: | :--: | :--: |
| dynamic-load | include/registry/dynamic_load.hpp | 加载外置插件 | Windows/POSIX |
| io | include/registry/io.hpp | 输入输出| 任意 |
| constants | include/registry/constants.hpp | 常见的常量或判断函数 | 任意 |
| fileio | include/registry/fileio.hpp | 文件输入输出 | 任意 |
| type-utils | include/registry/type_utils.hpp | 类型转换和查询 | 任意 |
| system | include/registry/system.hpp | 使用系统命令 | 任意 |
| math | include/registry/math.hpp | 简单的数学功能 | 任意 |
| algorithm | include/registry/algorithm.hpp | 常见算法和函数 | 任意 |
| base | include/registry/base.hpp | 基本语言功能 | 任意 |

用户可以自己加载以下自带的插件：

| 名称 | 文件 | 功能 | 操作系统 |
| :--: | :--: | :--: | :--: |
| eval | include/registry/more/eval.hpp | 运行字符串中的代码 | 任意 |
| win32-windowing | include/registry/win32w.hpp | Win32 窗口编程 | Windows |

### 多文件

`import` 关键字被用于导入文件或二进制中的代码。当代码版本和二进制版本同时存在，MPCC Project 会优先选用二进制版本。MPCC 不会判断文件类型，而仅仅会按照 `.precf` 后缀名来认定二进制文件。

通常会在解析时完成导入操作，用户可以更改 `program/util.hpp` 中的 `doImport` 至 `false` 以在解析时不完成该步骤。请注意该更改仅改变解析逻辑而不改变运行逻辑。若在解析时完成，则抽象语法树不产生 `Import` 节点，否则产生 `Import` 节点。

被导入的文件路径以字符串形式接在 `import` 关键字后，并且仅通过字符串比较来避免重复导入。

### 国际化

MPCC 支持多种语言，当前完成的有：

- `<default>`：缺省语言，无法加载下述时自动加载，内嵌到代码
- `zh_CN`：简体中文（中国）
- `en_US`：English (United States)
- `ja_JP`：日本語（日本）

注意：并不是所有语言的翻译都经过校验。若有不当，请予以反馈。

在 `include/program/util.hpp` 中可以设置默认值，运行时使用 `-l <Language>` 参数可以临时设置。所有语言均为 `UTF-8` 格式，请注意在 Windows 平台下运行会更改当前命令行上下文的代码页，正常退出时复原，则可能会造成一定的影响。

### 标准库

`mstdlib` 是 MPCC Project 使用的默认标准库，在 `res/standard.json` 中可以增减标准库以及设置默认。

## 开发进度

- [x] 继承和多态
- [ ] break, continue, else 功能扩展
- [ ] 常量函数参数
- [ ] 更强大的参数展开
- [ ] Object (Map)
- [ ] 函数绑定
- [ ] 语法扩展
- [ ] 弱语法检查模式

## 贡献

欢迎为这个项目提交 PR 或 issue，我会非常高兴看到它！

## 许可协议

该项目在 MIT 协议下分发，另见 [LICENSE](LICENSE)。
