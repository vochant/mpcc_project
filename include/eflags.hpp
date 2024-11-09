#include <memory>

struct EFlags {
    size_t maxStackDepth;       // 最大栈深
    bool autoClearArgs;         // 自动清理参数
    bool strictAccessControl;   // 严格访问控制（禁用 FACS 指令）
    bool disableGlobal;         // 禁止使用 global
    bool acceptDebugger;        // 启用调试特性
    static constexpr bool undefinedAsError = false;
    bool noConstantsCache;      // 不缓存常用常量
    unsigned int build;         // MPCC Binary 版本
    unsigned short binsig;      // 二进制签名版本
    bool shiftOnFloat;          // 允许浮点数使用 SHL 和 SHR 充当乘除
    bool noRuntimeInit;         // 不初始化运行时环境
    bool windowsNoUTF8;         // 不为 Windows 切换到 UTF-8 代码页
    bool breakNullCall;         // 忽略对 Null 的调用
    bool privAsError;           // 尝试外部访问私有成员触发错误
    EFlags();
}; // 环境选项

extern EFlags _eflags;