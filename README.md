# MPCC Project

## Translations

English | [中文](README-zh_CN.md)

## Introduction

> [!CAUTION]
> The project is not released yet. Before the first release, you cannot compile or run it.

### MPC

MPC is a scripted, explanatory, strongly typed dynamic programming language that stores code logic in the from of AST. The code can be easily converted to a binary format to save space! It is object-oriented, supports classes, encapsulation, constructors, destructors, inheritance, polymorphism, etc.

MPC means Mirekintoc's "Portable & Customizable" programming language. It has a very small source code, which does not take too much time to understand. It is easy to modify and has a convenient plug-in loading scheme. The following is a sample code:

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

### MPCC Project

MPCC Project is a cross-platform integration tool for MPC programming language based on C++.

## Building & Running

### Supporting Systems

MPCC is currently compatible with common systems such as Windows, macOS, Linux, etc., and relies on the Windows API or POSIX API to load external plug-ins. This type of plug-in does not depend on modifying the source code of MPCC, but can be loaded by the program from local dynamic libraries. This is not a necessary function, but a plugin that is loaded by default. You can remove it. Also, please note whether your program is dependent on it or other plugins that are not cross-platform. Theoretically, MPCC that do not contain plugins are compatible with any system (unless it doesn't even have CMake and a suitable C/C++ compiler! ).

The MPCC Project does not depend on any specific instruction set. Common architectures such as x86, arm, RISC-V, PowerPC, and Loongarch can all run it, as long as they meet the previous requirement.

Due to functionalities like I18N and ASTDLIB relying on the Module class for parsing, which depends on the `nlohmann_json` library, if your system is incompatible with this library, try removing it from `CMakeList.txt` and change `NO_MODULE` to `true`. Note that this will degrade error messages to default mode, displaying internal names in an unreadable form, similar to `[ERROR][ASTVM] :error.datatype :error.value.type`. Failing to move the `res` directory when relocating the program will result in the same effect.

### Building

Before we get started, you need to install CMake and a compilation tool that you are familiar with it. Run the following commands:

```shell
cmake -B build -S . -D BUILD_TYPE=Release
cmake --build build --config Release
```

### Running

To evaluate source code from a file:

```shell
mpcc <fileName>
```

To run REPL(Read-Evaluate-Print-Loop) mode:

```shell
mpcc -r
```

To run a binary file (`.precf`):

```shell
mpcc -b <fileName>
```

To generate a binary file:

```shell
mpcc -c <inputFileName> <outputFileName>
```

To get additional information:

```shell
mpcc -v # Version
mpcc -a # About
```

**Note:** You cannot merge two command-line parameters.

## Plugins

To create a plugin, follow these steps:

### Folder

Create a new file in the `include/registry` directory, such as `my_plugin.hpp`.

### Coding

Write the following content in the file. The example demonstrates the functionality of implementing an integer summation:

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

- `NativeFunction::resulttype` represents `std::pair<NativeFunctions::Result, std::shared_ptr<Object>>`.
- `NativeFunction::arglist` represents `std::vector<std::shared_ptr<Object>>`.

Enumerate `NativeFunction::Result` includes 4 values: `OK`, `FORMAT_ERR`,  `DATA_ERR` and `UNHANDLED_ERR`.They respectively represent:

- `OK`: No error(s).
- `FORMAT_ERR`: Called in the wrong format
- `DATA_ERR`: Called with incorrect data (not data type).
- `UNHANDLED_ERR`: An error occurred due to unknown reason.

### Editing

Modify `include/registry/base/selection.hpp`:

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

### Rebuilding

Rebuild the project.

## Features

### Language

MPC provides the following data types by default. Except for Class, Native Data and Instance, which have non fixed type strings, they all have fixed type strings.

The `typestr` function provided in the `type-utils` library located in the file `include/registry/typeutils.hpp` can retrieve the type of a value. The type of a class is the same as the type of its instance.

Supported operations:

- Arithmetic: Addition, subtraction, multiplication, division, modulo, positive, negative

- Bitwise: AND, OR, NOT, XOR, left shift, right shift

- Logic: AND, OR, NOT

- Comparison: Equal to, not equal to, greater than, less than, greater than or equal to, less than or equal to

### Libraries

| Name | File | Features | System |
| :--: | :--: | :--: | :--: |
| dynamic-load | include/registry/dynamic_load.hpp | Provide the ability to load external plugins | Windows/POSIX |
| io | include/registry/io.hpp | Provide standard input and output | All |
| constants | include/registry/constants.hpp | Provide information about the operating system and MPCC | All |
| fileio | include/registry/fileio.hpp | Provide the ability to read and write from files | All |
| type-utils | include/registry/type_utils.hpp | Provide functions such as type conversion and type string | All |
| system | include/registry/system.hpp | Provide the ability to execute system commands | All |
| math | include/registry/math.hpp | Provide simple mathematical functions | All |
| algorithm | include/registry/algorithm.hpp | Provide common algorithms and methods | All |
| base | include/registry/base.hpp | Provide some basic language features | All |
| config | include/registry/config.hpp | Change evaluator config | All |

Users can also load the following built-in libraries according to their own needs:

| Name | File | Features | System |
| :--: | :--: | :--: | :--: |
| cpython | include/registry/more/cpython.hpp | Provide the ability to run Python code with CPython, but require the corresponding include file. | All |
| eval | include/registry/more/eval.hpp | Provide the ability to run code in strings | All |
| win32-windowing | include/registry/win32w.hpp | Provide the ability to create window applications on Windows | Windows |

### Multifile

The `import` keyword provided by MPC can import code from files. Please note that if there is a binary version of the file, MPCC will prioritize importing its binary version over its source code version. MPCC does not recognize file formats and will only attempt to add the `.precf` suffix at the end of the file name to find its binary version.

The import operation will be completed by default at the evaluation step. If you have built-in imported content when generating binary, you can also add the `-x` command-line parameter to accomplish this. Reminder again, you cannot concatenate command line parameters, which means you cannot use syntax like `mpcc -cx in.mpc out.precf`, but only `mpcc -c -x in.mpc out.precf`.

The imported path can only be a path equivalent to the running directory or an absolute path, written as a string after the keyword `import`.

### Internationalization

MPCC supports a lot of languages. Completed languages include:

- `<default>`: Default language, which will be loaded unless the following cannot be loaded
- `zh_CN`: 简体中文（中国）
- `en_US`: English (United States)
- `ja_JP`: 日本語（日本）

Maybe there are some mistakes in translations. Please give me feedback if you find problems.

To set the default value, you could edit `include/program/util.hpp` or specify parameter `-l <Language>` to set temporarily. All languages are in UTF-8 format. Please note that running on the Windows platform may change the code page of the current command line context. Restoring it to normal upon regular exit may cause certain impacts.

## Development Progress

- [x] Inheritance & Polymorphism
- [ ] break, continue, else-extra
- [ ] const for function arguments
- [ ] More Arguments Anywhere for calling & array
- [ ] Object(Map)
- [ ] Function Binding
- [ ] Syndax Extra
- [ ] Lazy Mode (negative width, lambda or native constructors, etc.)

## Contributing

Welcome to create issues and pull requests for the project. I am sooo happy to see it!

## License

The project is licensed under MIT license. See also [LICENSE](LICENSE).
