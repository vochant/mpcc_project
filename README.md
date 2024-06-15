# MPCC Project

> This README.md is only available in Simplified Chinese.
> If you cannot read Simplified Chinese, please use a translator.
> We do not guarantee that the automatic translation results are accurate,
> and unless you are certain that the original expression is incorrect,
> you should not submit an issue regarding language expression.

> [!CAUTION]
> MPCC Project 当前仍未完成编写，您也许尚不能按照给定的方式编译和运行。

## 介绍

**MPCC Project** 是一款使用 C++ 编程语言的跨平台解释性编程语言。MPC 表示 Mirekintoc's "Portable & Customizable" PL。它的代码精简，不需要太长的时间用于理解，同时也易于修改、装载插件。

```mpc
// Hello World in MPC Programming language
print("Hello World");
```

MPC 是一种脚本式、解释性、强类型的多态编程语言，使用抽象语法树存储代码逻辑，代码逻辑可以轻易地保存到二进制文件中。

```mpc
class Integer {
    private _value = 0;
    print("An integer class created."); // library "io" required.
    constructor(v) {
        _value = v;
    }
    public isok = function() {
        return typestr(_value) == "integer"; // library "type-utils" required.
    }
    destructor {
        print("An integer calss destructed.");
    }
    function valueOf() {
        return _value;
    }
}

let a = Integer(114514);
```

MPC 是一款面向对象的编程语言，支持类、封装、构造函数、析构函数、继承、多态等功能。

## TODO

- [x] 继承和多态
- [ ] break, continue, else 功能扩展
- [ ] 常量函数参数
- [ ] 更强大的参数展开
- [ ] Object (Map)
- [ ] 函数绑定
- [ ] 语法扩展
- [ ] 弱语法检查模式

## 编译和运行

### 兼容性

MPCC Project 当前支持绝大多数常见的操作系统，包括但不限于 Windows、macOS、Linux， 并且可以基于 Windows API 或 POSIX API 加载外置的插件。这种插件不需要更改 MPCC Project 的源代码即可运行，而是被 MPCC Project 从本地动态库读取而运行。这不是一个必须的功能，但通常会启用它，当然你也可以禁用它。另外，请注意您选中的插件是否跨平台。理论上没有安装插件的 MPCC Project 可以在任意支持 CMake 和 C++17 的操作系统上运行。

MPCC Project 并不依赖于某种特定的指令集，常见的，例如 x86、arm、RISC-V、PowerPC、Loongarch 都可以运行，只要符合上一条。

由于 I18N、ASTDLIB 等功能使用 `Module` 类进行解析依赖于 `nlohmann_json` 库，如果您的系统不兼容该库，请尝试在 `CMakeList.txt` 中删除该项，并将 `NO_MODULE` 改为 `true`。请注意，这将导致错误信息退化为缺省模式，即使用内部名称的不可读形式，类似于 `[ERROR][ASTVM] :error.datatype :error.value.type`。如果移动程序时没有一并移动 `res` 目录，则会产生同样的效果。

### 构建

请确认您安装了 CMake 和一种您喜欢的构建工具。

#### 创建输出目录

```shell
mkdir build
cd build
```

#### 运行 CMake

```shell
cmake -S ..
```

您也可以指定使用什么工具链，例如下例就使用了 Ninja。

```shell
cmake -GNinja -S ..
```

#### 编译

这一步取决于您之前选择了什么工具链。在 Unix-like 操作系统上，这一步通常需要 `make` 命令。在 Windows 操作系统上，您通常需要使用 Visual Studio 或 MSBuild 打开一个 `.sln` 项目文件。当然，它也可能是其他工具链对应的命令。

### 运行

要运行一个代码文件：

```shell
mpcc <fileName>
```

以 REPL (Read-Evaluate-Print Loop) 模式运行：

```shell
mpcc -r
```

要运行一个二进制文件 (`*.precf`)：

```shell
mpcc -b <fileName>
```

要创建一个二进制文件：

```shell
mpcc -c <inputFileName> <outputFileName>
```

**注意：** 您不能同时使用多种参数！

## 插件

要创建一个内置插件，您需要：

1. 插件一个文件位于 `include/registry` 目录，比如 `my_plugin.hpp`。

2. 写入下述内容。示例是对若干个整数求和，您也可以换成自己想要的。

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
       static NativeFunction::resulttype sum(NativeFunction::arglist args) {
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
           return std::make_pair(NativeFunction::Result::OK, std::make_shared<Integer>(_sum));
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

3. 修改 `include/registry/base/selection.hpp`:

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

4. 重新构建

## 功能

### 语言功能

MPC 提供多种类型。除 NaitveData、Instance 和 Class 具有不确定的类型字段，其他类型（Integer、String、Float、Boolean、Function、NativeFunction、Null、Error）都有固定了类型字段。

`type-utils` 库（`include/registry/typeutils.hpp`）提供的 `typestr` 函数可以获取一个值的类型。一个类和它的实例具有相同的编号（不是完整的类型字段）。

MPC 提供以下运算：

- 算数运算：加、减、乘、除、取模、正、负
- 位运算：按位与、按位或、按位取反、按位异或、左移、右移
- 逻辑运算：与、或、非
- 比较运算：等于、不等于、大于、小于、大于等于、小于等于

### 库

默认提供以下库：

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

- `<default>` 缺省语言，无法加载下述时自动加载，内嵌到代码
- `zh_CN` 简体中文（中国）
- `en_US` English (United States)
- `ja_JP` 日本語（日本）

注意：并不是所有语言的翻译都经过校验。若有不当，请予以反馈。

在 `include/program/util.hpp` 中可以设置默认值，运行时使用 `-l <Language>` 参数可以临时设置。所有语言均为 `UTF-8` 格式，请注意在 Windows 平台下运行会更改当前命令行上下文的代码页，正常退出时复原，则可能会造成一定的影响。

### 标准库

`mstdlib` 是 MPCC Project 使用的默认标准库，在 `res/standard.json` 中可以增减标准库以及设置默认。
