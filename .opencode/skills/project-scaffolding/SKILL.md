---
name: project-scaffolding
description: |
  通用项目创建与文件布局规范。
  采用"检测-推理-适配"模式，不绑定具体语言或框架。
  任何项目类型都能通过文件探测定性，并自动适配对应约定。
license: MIT
compatibility: opencode
metadata:
  audience: developers
  type: scaffolding
---

# 项目脚手架与文件布局规范

## 核心原则

1. **检测优于预设** — 不假定语言/框架，通过探测项目文件来定性
2. **约定可推断** — 从已检测到的工具链推导出项目布局约定
3. **适配优先于改造** — 尊重项目现有的结构，在此基础上扩展
4. **渐进式组织** — 从最简单的单文件开始，随复杂度增加自然演化

---

## 一、项目检测方法论（Project Detection）

这是本 skill 的核心 —— **不记忆具体语言的布局，而是教 agent 如何发现任何项目的布局。**

### 1.1 探测清单

面对一个未知项目，按顺序执行以下探测：

```
Phase 1: 根目录文件扫描
  → 用 Glob 检查已知的配置文件

Phase 2: 语言/框架指纹匹配
  → 根据检测到的文件推断语言和工具链

Phase 3: 现有源码结构分析
  → 读取关键配置，理解源文件组织方式

Phase 4: 约定确认
  → 对比社区标准，确认当前项目是否遵循约定
```

### 1.2 指纹匹配表（可扩展）

检测到以下文件时，可推断出对应的语言/工具链：

| 检测到文件 | 推断结论 |
|-----------|---------|
| `pyproject.toml` 或 `setup.py` 或 `requirements.txt` | Python 项目 |
| `package.json` | Node.js 项目（JS/TS） |
| `Cargo.toml` | Rust 项目 |
| `go.mod` | Go 项目 |
| `build.gradle*` 或 `pom.xml` | JVM 项目（Java/Kotlin） |
| `CMakeLists.txt` | C/C++ 项目（CMake） |
| `Cargo.toml` + `pyproject.toml` | 多语言（Rust + Python） |
| `.csproj` 或 `*.sln` | .NET 项目 |
| `mix.exs` | Elixir 项目 |
| `rebar.config` 或 `erlang.mk` | Erlang 项目 |
| `*.cabal` 或 `stack.yaml` | Haskell 项目 |
| `Pubspec.yaml` | Dart/Flutter 项目 |
| `Gemfile` | Ruby 项目 |
| `swift` 后缀 + `Package.swift` | Swift 项目 |
| `Dockerfile` 或 `docker-compose.yml` | 容器化项目 |
| `Makefile` | 任意项目（需要进一步检测） |

**关键规则**：没有匹配到任何已知指纹时，不猜测 —— 进入"未知项目处理"流程。

### 1.3 项目状态判断

```
是已有项目还是新项目？
  ├── 已有项目 → 分析现有结构，尊重已有约定
  │   ├── 有 Git 历史 → 读取 git log，了解演变
  │   └── 有 CI 配置 → 从 CI 脚本中理解构建流程
  │
  └── 新项目 → 根据选择的语言/框架，创建标准结构
      ├── 已知语言 → 用标准初始化命令（见附录参考）
      └── 不确定语言 → 问需求方（不要替用户做决定）
```

---

## 二、推理与适配（Inference & Adaptation）

### 2.1 从工具链推导目录结构

检测到工具链后，按以下规则推理源文件存放位置：

```
检测到 package.json（有 "type": "module"）
  → 推断：Node.js ESM 项目
  → 源文件可能在: src/ 或 lib/ 或根目录
  → 测试文件可能在: test/ 或 __tests__/ 或 *.test.ts
  → 构建产物: dist/ 或 build/

检测到 pyproject.toml（有 [tool.hatch] 或 [build-system]）
  → 推断：现代 Python 项目
  → 源文件可能在: src/<pkg>/ 或直接 <pkg>/
  → 测试文件: tests/
  → 虚拟环境: .venv/

检测到 Cargo.toml
  → 推断：Rust 项目
  → 源文件: src/
  → 测试文件: tests/（集成测试）+ src/ 内 #[cfg(test)]（单元测试）
  → 构建产物: target/

检测到 go.mod
  → 推断：Go 项目
  → 源文件: 根目录 + cmd/ + internal/ + pkg/
  → 测试文件: *_test.go 和源码同目录（Go 惯例）

检测到 CMakeLists.txt
  → 推断：C/C++ 项目
  → 源文件可能在: src/ + include/
  → 构建产物: build/
```

### 2.2 深度分析：读取关键配置

检测到配置后，读取关键字段以确定更具体的结构：

```python
# 伪代码逻辑
config = read_file("pyproject.toml")
if "[tool.hatch]" in config:
    layout = "src-based"     # src/<pkg>/
elif "[tool.poetry]" in config:
    layout = "src-based"     # src/<pkg>/
elif "scripts" in config:
    layout = "flat"          # <pkg>/ 直接在根目录
```

```json
// package.json 深度分析
{
  "scripts": { ... },          // 构建/测试/运行命令
  "workspaces": [...],         // monorepo 检测
  "type": "module"             // ESM vs CommonJS
}
```

### 2.3 适配规则

```
当前项目结构分析完成后，如需添加新文件：
  1. 参考社区标准布局
  2. 参考项目已有的文件组织模式
  3. 以最小侵入方式添加

例：一个 Python 项目
  → 现有文件都在根目录（main.py, utils.py）
  → 不应强制迁移到 src/ 布局
  → 应在根目录添加测试目录 tests/
  → 仅在项目明显需要扩展时，才建议做结构升级
```

---

## 三、项目类型通用结构（语言无关）

### 3.1 从简单到复杂

```
Level 0: 单文件
  project/
  ├── main.py / index.ts / main.go
  └── README.md

Level 1: 多模块
  project/
  ├── <module_a>.py
  ├── <module_b>.py
  ├── test_<module_a>.py
  └── README.md

Level 2: 有配置的项目
  project/
  ├── src/
  ├── tests/
  ├── <config_file>
  └── README.md

Level 3: 完整项目
  project/
  ├── src/
  ├── tests/
  ├── scripts/        # 构建/运维脚本
  ├── docs/           # 文档
  ├── examples/       # 使用示例
  ├── <config>        # 语言特定配置文件
  ├── .gitignore
  └── README.md

Level 4: 多服务/多语言项目
  project/
  ├── services/
  │   ├── <service-a>/    # 每个服务独立项目
  │   └── <service-b>/
  ├── shared/             # 共享 protobuf/schema
  ├── deploy/             # Docker/K8s 配置
  ├── docs/
  └── README.md
```

**原则**：不必一开始就到 Level 4。从项目当前复杂度选择合适层级，随需求自然演进。

### 3.2 目录命名约定

| 用途 | 命名方式 | 说明 |
|------|----------|------|
| 源代码 | `src/` 或语言惯例 | 首选 `src/` |
| 测试 | `tests/` 或语言惯例 | 首选 `tests/` |
| 脚本 | `scripts/` | 构建、部署、维护脚本 |
| 文档 | `docs/` | 项目文档 |
| 示例 | `examples/` | 使用示例代码 |
| 部署 | `deploy/` | Docker、K8s、CI 配置 |
| 共享接口定义 | `shared/` | protobuf、OpenAPI、JSON Schema |
| 配置文件 | `config/` | 环境配置 |

---

## 四、多语言项目架构（Multi-Language Projects）

### 4.1 何时应考虑多语言

```
一个服务的不同部分有不同性能/生态需求时：
  ├── API 层 → Node.js/Python（快速开发）
  ├── 计算密集 → Rust/Go（高性能）
  └── 数据处理 → Python（生态丰富）

纯技术原因引入新语言前，先问：
  - 现有语言是否真的无法满足需求？
  - 维护多语言栈的团队成本是否可接受？
```

### 4.2 推荐布局

```
project/
├── services/                  # 每个服务独立子项目
│   ├── api-gateway/
│   │   ├── src/
│   │   ├── tests/
│   │   └── package.json
│   ├── user-service/
│   │   ├── src/
│   │   ├── tests/
│   │   └── pyproject.toml
│   └── data-processor/
│       ├── src/
│       ├── tests/
│       └── Cargo.toml
├── shared/                    # 共享接口定义
│   ├── protos/                # Protobuf 定义
│   └── schemas/               # JSON Schema
├── deploy/                    # 全局部署配置
│   ├── docker/
│   └── k8s/
├── .editorconfig             # 统一编辑器设置
├── .gitignore
└── README.md
```

### 4.3 服务间通信规则

| 方式 | 适用场景 | 要求 |
|------|----------|------|
| HTTP/REST | 同步请求-响应 | 显式 API 文档（OpenAPI） |
| gRPC | 高性能内部通信 | Protobuf 定义在 shared/ |
| 消息队列 | 异步事件驱动 | 明确的事件 Schema |
| 共享库 | 仅限同语言 | 独立版本管理 |

**核心约束**：
- 不同语言的服务**禁止直接导入**对方代码
- 服务间通信必须通过 **显式契约**（API/Proto/Schema）
- 每个服务**独立数据库**（或至少独立 Schema）

---

## 五、环境与配置管理

### 5.1 配置文件层级

```
project/
├── .env.example          # 模板，提交到 Git
├── .env                  # 本地配置，加入 .gitignore
├── .env.development      # 开发环境覆盖
├── .env.production       # 生产环境覆盖
└── config/
    ├── default.yaml      # 默认配置
    ├── development.yaml
    └── production.yaml
```

### 5.2 通用配置文件

| 文件 | 用途 | 建议 |
|------|------|------|
| `.gitignore` | 忽略规则 | 从 gitignore.io 生成 |
| `.editorconfig` | 跨编辑器格式 | 缩进、编码、行尾统一 |
| `README.md` | 项目概览 | 必须：简介、快速开始 |
| `LICENSE` | 开源协议 | 按需 |

---

## 六、遇到未知项目类型的处理流程

当指纹匹配失败时（无法识别项目类型），按以下流程操作：

```
1. 文件系统探测
   → 列出根目录文件
   → 检查 src/、lib/、app/、cmd/、main.* 等常见入口

2. 源码分析
   → 读取 main 文件，分析语法特征
   → 检测 import/require/use/mod 等关键字识别语言

3. 网络参考
   → 如果无法判断，使用 WebFetch 搜索"<检测到的特征> project structure conventions"
   → 或搜索"how to structure a <特征> project"

4. 主动询问
   → 向需求方确认：
   - 这是什么语言/框架？
   - 项目预期的目录结构是怎样的？
   - 是否有内部规范文档？

5. 建立本地约定
   → 一旦确认，将这个新类型的指纹记录在本 project 的 AGENTS.md 中
   → 下次遇到同类型项目时可直接使用
```

---

## 七、快速启动参考（附录）

以下为已知语言的标准初始化方式，仅作参考。**实际使用时必须先检测项目是否存在。**

| 语言 | 检测指纹 | 初始化命令 | 构建运行 |
|------|---------|-----------|---------|
| Python | `pyproject.toml` / `requirements.txt` | `uv init` / `pip install` | `uv run python main.py` |
| Node.js | `package.json` | `pnpm init` / `npm init` | `pnpm start` / `node index.js` |
| TypeScript | `package.json` + `tsconfig.json` | `pnpm init` + `tsc --init` | `pnpm build && pnpm start` |
| Rust | `Cargo.toml` | `cargo new` / `cargo init` | `cargo run` |
| Go | `go.mod` | `go mod init` | `go run .` |
| Java | `build.gradle*` / `pom.xml` | `gradle init` / `mvn archetype` | `gradle run` / `mvn exec` |
| C/C++ | `CMakeLists.txt` | `cmake -B build` | `cmake --build build` |
| .NET | `.csproj` / `*.sln` | `dotnet new` | `dotnet run` |
| Ruby | `Gemfile` | `bundle init` | `ruby main.rb` |
| Dart/Flutter | `pubspec.yaml` | `dart create` | `dart run` |

### 实际项目适配检查清单

面对一个已有项目时，按顺序检查：

```
□ 1. 根目录有哪些配置文件？
     → 运行 ls/glob 检查

□ 2. 检测到的配置对应什么语言/框架？
     → 参考指纹匹配表

□ 3. 源文件在哪里？（src/、lib/、根目录？）
     → 读取关键配置中的目录设置

□ 4. 测试文件在哪里？
     → 查找 tests/、test/、__tests__/、*.test.* 等

□ 5. 构建/运行命令是什么？
     → 从配置中读取 scripts/build/test 字段

□ 6. 项目是 monorepo 还是单包？
     → 检查 workspaces、subprojects、多 Cargo.toml

□ 7. 现有代码的组织模式是怎样的？
     → 按功能/按类型/按层级？
     → 新文件应遵循同样的模式
```
