# MPCC Project

## 翻译

[English](README.md) | 中文 

## 介绍

> [!CAUTION]
> MPCC Project 当前仍未完成编写，您也许不能按照给定的方式编译和运行。

### MPC

MPC 是一种脚本式、解释性、强类型的编程语言。MPC 是一款面向对象的编程语言，支持类、封装、构造函数、析构函数、继承、多态等功能。

```mpc
class IntegerClass {
    private value _value = 0;
    constructor(v : Integer) {
        _value = v;
        print("An integer class created."); // library "io" required.
    }
    public method isok() {
        return typestr(_value) == "integer"; // library "type-utils" required.
    }
    destructor {
        print("An integer calss destructed.");
    }
    public method valueOf() {
        return _value;
    }
}

let a = new IntegerClass(114514);
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

**注意**：您不能同时使用多种模式！

### 模块

1. **Token 解析器 (Lexer)** 用于划分源代码部件
2. **语法解析器 (Parser)** 用于解析源代码为 AST
5. **虚拟机 (VM)** 用于运行 AST
6. **转换器 (Convertor)** 用于将二进制/内存格式转换为人类可读形式。

二进制 AST 保存文件的 Magic 是 `MirekE09`

## 插件

要创建一个内置插件，您需要：

### 创建类和代码文件

在 `include/plugins/plugin.hpp` 中的命名空间 `Plugins` 添加如下类，若名称为 `MyPlugin`：

```cpp
class MyPlugin : public Plugin {
protected:
    void enable();
public:
    MyPlugin();
}
```

创建一个位于 `src/plugins` 目录的文件，例如 `my_plugin.cpp`。

### 编写代码

写入下述内容。示例是对若干个整数求和，您也可以换成自己想要的。

```cpp
#include "plugins/plugin.hpp"
#include "vm_error.hpp"
#include "object/integer.hpp"

Plugins::MyPlugin::MyPlugin() {}

std::shared_ptr<Object> Integer_Sum(Args args) {
    plain(args);
    if (args.size() == 0) {
        throw VMError("(MyPlugin)Integer_Sum", "Incorrect Format");
    }
    long long _sum = 0;
    for (auto& i : args) {
        if (i->type != Object::Type::Integer) {
            throw VMError("(MyPlugin)Integer_Sum", "Incompatible Argument Type");
        }
        _sum += std::dynamic_pointer_cast<Integer>(i)->value;
    }
    return std::make_shared<Integer>(_sum);
}

void Plugins::MyPlugin::enable() {
    regist("sum", Integer_Sum);
}
```

其中，`plain` 函数可以将 `args` 中所有的引用转换为非引用。`regist` 可以注册两种类型：`std::shared_ptr<Object>` 或 `NFunc`，其中 `NFunc` 表示一个符合以上格式的函数。另外，`Args` 是 `std::vector<std::shared_ptr<Object>>` 的简写。通常地，插件报错应当抛出 `VMError`。

### 修改文件

修改 `src/main.cpp`，添加对该插件的注册行为，即 `Program.loadLibrary(makePlugin<Plugins::MyPlugin>())`。

### 重新构建

最后重新构建整个项目。

## 功能

### 语言功能

MPC 提供多种类型。除 Naitve 和 Instance 具有不确定的类型字段，其他类型（如 Integer、String、Float、Boolean、Executable、Null）都有固定的类型字段。

支持运算：

- 算数运算：加、减、乘、除、取模、正、负、幂
- 位运算：按位与、按位或、按位取反、按位异或、左移、右移
- 逻辑运算：与、或、非
- 比较运算：等于、不等于、大于、小于、大于等于、小于等于、严格等于、严格不等于
- 其他运算：取下标、构造范围、装饰、调用、自增、自减、赋值、运算赋值、三目运算符

### 支持库

| 类名 | 文件 | 功能 | 操作系统 |
| :--: | :--: | :--: | :--: |
| DynamicLoad | src/plugins/dynamic_load.cpp | 加载外置插件 | Windows/POSIX |
| IO | src/plugins/io.cpp | 输入输出| 任意 |
| FileIO | src/plugins/fileio.cpp | 文件输入输出 | 任意 |
| Math | src/plugins/math.hpp | 简单的数学功能 | 任意 |
| Base | src/plugins/base.cpp | 基本语言功能 | 任意 |

### 多文件

`import` 关键字被用于导入文件或二进制中的代码。

被导入的文件路径以字符串形式接在 `import` 关键字后，并且仅通过字符串比较来避免重复导入。

### 标准库

`mstdlib` 是 MPCC Project 使用的默认标准库，在 `res/standard.json` 中可以增减标准库以及设置默认。

## 贡献

欢迎为这个项目提交 PR 或 issue，我会非常高兴看到它！

感谢 `lighzy-interpreter` 提供的代码参考和 CMake 模板

## 许可协议

该项目在 MIT 协议下分发，另见 [LICENSE](LICENSE)。
