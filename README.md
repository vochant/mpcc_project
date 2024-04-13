# MPCC Project

## Introduction

**MPCC Project** is a cross-platform integration tool for MPC programming language based on C++. MPC means Mirekintoc's "Portable & Customizable" programming language. It has a very small source code, which does not take too much time to understand, and is easy to modify, and also has a convenient plug-in loading scheme.

```mpc
// Hello World in MPC Programming language
print("Hello World");
```

MPC is a scripted, explanatory, strongly typed dynamic programming language that stores code logic in the from of AST. The code can be easily converted to a binary format to save space, and this process is completely reversible (but it will lose code annotations, format and partial grammar! For example, `public` will become `var`, `constructor(a, b) {}` will become `var __spec_constructor2 = function(a, b) {}`)

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

MPC is object-oriented, supports classes, encapsulation, constructors, destructors, but not inheritance, polymorphism, etc.

## Building & Running

### Operating System

MPCC is currently compatible with common systems such as Windows, macOS, Linux, etc., and relies on the Windows API or POSIX API to load external plug-ins. This type of plug-in does not depend on modifying the source code of MPCC, but can be loaded by the program from local dynamic libraries. This is not a necessary function, but a plugin that is loaded by default. You can remove it. Also, please note whether your program is dependent on it or other plugins that are not cross-platform. Theoretically, MPCC that do not contain plugins are compatible with any system (unless it doesn't even have CMake and a suitable C/C++ compiler!).

### Building

Confirm that you have installed CMake and a compilation tool that you are familiar with.

#### Create a build directory

```shell
mkdir build
cd build
```

#### Run CMake

```shell
cmake ..
```

If you want to specify what toolchain to use, you can also do this (using Ninja Build as an example):

```shell
cmake -GNinja ..
```

#### Compile

The method used to complete this step depends on your operating system and the settings from the previous step. On UNIX like systems, the `make` command is usually used to complete this step, while on Windows systems, you will be required to open the generated `. sln` file using Microsoft Visual Studio and build it. Of course, if you have already specified the toolchain to use in advance, you may also need to use tools such as Ninja Build.

### Running

To evaluate source codes from a file:

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

To change stdin/stdout:

```shell
mpcc -i <inputFile> -o <outputFile> ...
```

To get additional information:

```shell
mpcc -v # Version
mpcc -a # About
```

**Attention:** You cannot merge two command-line parameters!!!

## Plug-ins

To create a plugin, you can follow the following steps:

1. Create a new file in the `include/registry` directory, such as `my_plugin.hpp`.

2. Write the following content in the file. The following example demonstrates the functionality of implementing an integer summation.

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
               return std::make_pair(NativeFunction::Result::FORMAT_ERR, std::make_shared<Error>());
           }
           long long _sum = 0;
           for (auto i : args) {
               if (i->type != Object::Type::Integer) {
                   return std::make_pair(NativeFunction::Result::FORMAT_ERR, std::make_shared<Error>());
               }
               _sum += std::dynamic_pointer_cast<Integer>(i)->value;
           }
           return std::make_pair(NativeFunction::Result::OK, std::make_shared<Integer>(_sum));
       }
   };
   ```

   `NativeFunction::resulttype` represents `std::pair<NativeFunctions::Result, std::shared_ptr<Object>>`.

   `NativeFunction::arglist` represents `std::vector<std::shared_ptr<Object>>`.

   Enumerate `NativeFunction::Result` includes 4 values: `OK`, `FORMAT_ERR`,  `DATA_ERR` and `UNHANDLED_ERR`.They respectively represent:

   - `OK` : No error(s).
   - `FORMAT_ERR` : Called in the wrong format
   - `DATA_ERR` : Called with incorrect data (not data type).
   - `UNHANDLED_ERR` : An error occurred due to unknown reason.

3. Modify `include/registry/base/selection.hpp`:

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

4. Re-build the project.

## Features

### Language features

MPC provides the following data types by default. Except for Class, Native Data and Instance, which have non fixed type strings, they all have fixed type strings. The provided data types are as follows:

```plaintext
Integer Float Boolean Array Class Instance Enumerate Object String Function NativeData NativeFunction Error Null
```

The `typestr` function provided in the `type-utils` library located in the file `include/registry/typeutils.hpp` can retrieve the type of a value. The type of a class is the same as the type of its instance.

MPC provides arithmetic, modulo, and bitwise operations for integer types, arithmetic and modulo operations for floating-point types, concatenation operations for string types, and AND, OR, and NOT operations for Boolean types. Provide 6 common comparison operations. Provides assignment operations and can be continuously assigned, compatible with post operation assignments (excluding logical operations), and compatible with prefix or suffix forms of self increasing and self decreasing.

### Libraries

The following is the default provided libraries:

| Name | File | Features | System |
| :--: | :--: | :--: | :--: |
| dynamic-load | include/registry/dynamic_load.hpp | Provide the ability to load external plugins | Windows/POSIX |
| io | include/registry/io.hpp | Provide standard input and output | All |
| constants | include/registry/constants.hpp | Provide information about the operating system and MPCC | All |
| fileio | include/registry/fileio.hpp | Provide the ability to read and write from files | All |
| type-utils | include/registry/type_utils.hpp | Provide functions such as type conversion and type string | All |
| system | include/registry/system.hpp | Provide the ability to execute system commands | All |
| math | include/registry/math.hpp | Provide simple mathematical functions | All |

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
