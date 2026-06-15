---
name: cpp-project-template
description: |
  从模板创建符合 cpp-cmake-coding-standards 规范的 C++ 项目，
  包含 CMake 构建系统、预设配置、预编译头、测试/示例目录结构、
  以及第三方库管理约定。
license: MIT
metadata:
  audience: C++ developers
  type: template
  requires:
    - cpp-cmake-coding-standards
    - project-scaffolding
---

# C++ 项目模板

基于 `cpp-cmake-coding-standards` 和 `project-scaffolding` 规范，
从零创建完整的 C++ 项目骨架。

## 模板变量

创建项目时，Agent 必须向用户确认以下变量：

| 变量 | 说明 | 示例 |
|------|------|------|
| `{{PROJECT_NAME}}` | 项目名（snake_case） | `render_engine` |
| `{{PROJECT_NAME_UPPER}}` | 项目名（ALL_CAPS，自动派生） | `RENDER_ENGINE` |
| `{{PROJECT_NAMESPACE}}` | 根命名空间（PascalCase） | `Engine`、`Gfx` |
| `{{PROJECT_DESCRIPTION}}` | 项目简述 | `A rendering engine` |

## 项目结构

```
{{PROJECT_NAME}}/
├── CMakeLists.txt
├── CMakePresets.json
├── .gitignore
├── include/
│   └── {{PROJECT_NAME}}/
│       └── {{PROJECT_NAME}}.h
├── src/
│   ├── pch.h
│   ├── pch.cpp
│   └── {{PROJECT_NAME}}.cpp
├── ext/
│   └── CMakeLists.txt
├── 3rd/
│   └── CMakeLists.txt
├── tests/
│   ├── CMakeLists.txt
│   └── test_core/
│       └── main.cpp
└── examples/
    ├── CMakeLists.txt
    └── hello_world/
        └── main.cpp
```

## 实例化流程

1. **确认变量** — 向用户确认 `PROJECT_NAME`、`PROJECT_NAMESPACE`、`PROJECT_DESCRIPTION`
2. **创建目录** — 在用户指定的路径创建完整目录结构
3. **生成文件** — 读取 `template/` 下的所有模板文件，进行变量替换后写入目标路径
4. **替换规则**：
   - 文件名中的 `__project_name__` → `{{PROJECT_NAME}}`
   - 文件内容中的 `{{PROJECT_NAME}}` → 项目名
   - 文件内容中的 `{{PROJECT_NAME_UPPER}}` → 项目名大写形式（如 `RENDER_ENGINE`）
   - 文件内容中的 `{{PROJECT_NAMESPACE}}` → 项目命名空间
   - 文件内容中的 `{{PROJECT_DESCRIPTION}}` → 项目描述
   - 文件内容中的 `{{YYYY}}`、`{{MM}}`、`{{DD}}` → 当前日期
5. **验证** — 检查生成的项目是否满足 cpp-cmake-coding-standards 所有规范

## 使用示例

用户执行 `/cpp-project-template` 或说"创建一个 C++ 项目"时触发。

Agent 应询问：
- 项目名称？（snake_case）
- 根命名空间？（PascalCase，默认取项目名的 PascalCase 形式）
- 项目简介？（一句话）
- 创建位置？（默认当前工作目录）
