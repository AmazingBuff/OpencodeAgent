---
name: testing-strategy
description: |
  通用测试策略与单元测试隔离规范。
  采用"检测-适配"模式，不绑定具体语言或测试框架。
  任何项目的测试体系都能通过文件探测定性并适配。
license: MIT
compatibility: opencode
metadata:
  audience: developers
  type: testing
---

# 测试策略与模块隔离规范

## 核心原则

1. **隔离优先** — 每个测试独立运行，不依赖外部状态
2. **行为验证** — 测试行为而非实现细节
3. **测试即文档** — 测试代码应清晰表达被测单元的预期行为
4. **红-绿-重构** — TDD 循环：失败 → 通过 → 优化

---

## 一、测试框架检测（Framework Detection）

**核心方法**：不假定测试框架，通过探测项目文件来发现已使用的框架。

### 1.1 探测流程

```
Step 1: 检查配置文件
  → 搜索已知测试框架的配置文件

Step 2: 检查测试目录
  → 测试文件一般在 tests/、test/、__tests__/ 或源码同目录

Step 3: 读取一个测试文件
  → 分析 import/require 语句，确定实际使用的框架

Step 4: 检查构建/运行命令
  → 从 package.json scripts、Makefile、justfile 等读取测试命令
```

### 1.2 框架指纹匹配表

当检测到以下配置或依赖时，可推断出测试框架：

| 检测指纹 | 推断框架 | 典型命令 |
|---------|---------|---------|
| `pyproject.toml` 含 `[tool.pytest]` | pytest | `pytest` |
| `pyproject.toml` 含 `[tool.unittest]` | unittest | `python -m unittest` |
| `package.json` 含 `"jest"` 依赖 | Jest | `npx jest` |
| `package.json` 含 `"vitest"` 依赖 | Vitest | `npx vitest run` |
| `package.json` 含 `"mocha"` 依赖 | Mocha | `npx mocha` |
| `Cargo.toml` 含 `[dev-dependencies]` 且含测试属性 | built-in (`#[cfg(test)]`) | `cargo test` |
| `go.mod` | go test (built-in) | `go test` |
| `build.gradle*` 含 `test` 块 | JUnit / Gradle test | `gradle test` |
| `pom.xml` 含 `junit` | JUnit / Maven surefire | `mvn test` |
| `CMakeLists.txt` 含 `enable_testing()` | CTest / GTest | `ctest` |
| `.csproj` 含 `xunit` 或 `nunit` | xUnit / NUnit / MSTest | `dotnet test` |
| `Gemfile` 含 `rspec` | RSpec | `rspec` |
| `mix.exs` 含 `ex_unit` | ExUnit (built-in) | `mix test` |

### 1.3 未检测到测试框架时的行为

```
1. 如果项目是新建的 → 根据项目语言选择推荐的测试框架（参考附录）
2. 如果是有历史的项目 → 报告未找到测试框架，询问是否需要创建
3. 推荐优先级（语言无关）：
   - 内置测试工具（如 `go test`、`cargo test`）> 第三方框架
   - 社区主流框架 > 小众框架
   - 无需额外运行时的框架 > 需要独立运行时的框架
```

---

## 二、测试文件结构（语言无关）

### 2.1 通用目录布局

```
tests/                        # 首选的测试目录
├── unit/                     # 单元测试
│   └── test_<module>.py      # 命名规则由语言惯例决定
├── integration/              # 集成测试
│   └── test_<feature>.py
├── fixtures/                 # 测试数据（JSON/YAML/CSV 等）
│   └── sample_response.json
└── mocks/                    # 模拟服务/对象
    └── mock_server.py
```

### 2.2 文件命名检测与适配

测试文件的命名方式取决于已检测到的框架。在添加新测试文件时：

```
Step 1: 查看已有的测试文件命名方式
  → 例：存在 test_user.py → 新文件也应命名 test_*.py
  → 例：存在 user.test.ts → 新文件也应命名 *.test.ts

Step 2: 如果没有现有测试 → 按语言惯例
  → 参考下面的通用规则

Step 3: 混合语言项目 → 每个子项目遵循各自语言的惯例
```

#### 通用命名规则（框架无关）

| 场景 | 模式 | 示例 |
|------|------|------|
| 单文件模块 | `test_<module>.<ext>` | `test_utils.py` |
| 测试类 | `Test<Component>` | `TestUserService` |
| 测试函数 | `test_<func>_<scenario>` | `test_login_with_invalid_email` |
| 集成测试 | `test_<feature>_integration.<ext>` | `test_auth_integration.py` |
| 测试数据夹具 | `fixtures/<name>.json` | `fixtures/user_data.json` |

### 2.3 测试函数命名规范

```
<test>_<被测单元>_<场景>_<预期>
```

- Python: `def test_calculate_total_with_empty_cart_returns_zero():`
- TypeScript: `it('returns zero when cart is empty', ...)`
- Go: `func TestCalculateTotal_WithEmptyCart_ReturnsZero(t *testing.T)`
- Rust: `fn test_calculate_total_with_empty_cart_returns_zero()`

**关键**：命名应能直接读出一个完整句子。

---

## 三、模块隔离策略（语言无关）

### 3.1 隔离层级

| 层级 | 依赖类型 | 隔离手段 | 测试速度 |
|------|---------|---------|---------|
| L0 | 纯函数 | 无需隔离 | 微秒 |
| L1 | 有外部依赖接口 | 依赖注入 + 接口 Mock | 毫秒 |
| L2 | 有 IO 操作 | Mock IO 层 | 毫秒 |
| L3 | 有外部服务 | Mock Server / Test Double | 毫秒~秒 |
| L4 | 有数据库 | 内存数据库 / Testcontainers | 秒 |

**通用原则**：尽量将代码推到更高的层级（L0 > L1 > ...），使测试更快更稳定。

### 3.2 依赖注入模式（框架无关）

为了可测试，代码必须支持依赖替换：

```
✅ 可测试的模式：
  - 构造器注入（constructor injection）
  - 参数注入（parameter injection）
  - 工厂方法 + 接口抽象

❌ 不可测试的模式：
  - 在方法内部直接 new/创建依赖对象
  - 全局单例（global/static）
  - 硬编码连接字符串/配置
```

```python
# ✅ 可测试
class Service:
    def __init__(self, db: Database, cache: CacheClient):
        self._db = db
        self._cache = cache

# ❌ 不可测试
class Service:
    def get(self, id: str):
        db = Database()  # 无法替换
```

```typescript
// ✅ 可测试
class Service {
  constructor(
    private db: Database,
    private cache: CacheClient
  ) {}
}
```

### 3.3 Mock 策略

**何时使用 Mock**：
- 调用外部服务（API/第三方/数据库/文件系统）
- 依赖非确定性行为（时间/随机数）
- 昂贵或难以搭建的环境

**何时避免 Mock**：
- 纯函数/数据结构（直接测试即可）
- 同一模块内部调用（应通过公共接口测试）
- 过度细粒度的 Mock（导致测试与实现耦合）

**如何 Mock（框架无关的通用技巧）**：
```
1. 创建接口/抽象类（或使用 duck typing）
2. 在测试中创建实现了该接口的假对象
3. 通过依赖注入传入假对象
4. 验证假对象是否被正确调用

并不是所有语言都需要显式接口——鸭子类型和代理模式同样有效。
```

---

## 四、测试命令检测

### 4.1 检测已有命令

```
1. 读取配置文件中的 scripts/test 字段
   → package.json: "scripts": {"test": "jest"}
   → pyproject.toml: [tool.pytest.ini_options]
   → Makefile: test: ...

2. 如果配置中没有测试命令
   → 按框架指纹中的"典型命令"运行
   → 运行前确认该工具已安装

3. monorepo 注意事项
   → 检查子项目各自的配置文件
   → 或根目录是否有统一的测试命令
```

### 4.2 运行特定测试

| 需求 | 通用方法 |
|------|---------|
| 运行所有测试 | 框架默认命令 |
| 运行单个文件 | `<command> <file_path>` |
| 按关键字过滤 | `<command> -k <keyword>` / `<command> -- --test-threads=1` |
| 带覆盖率 | `<command> --coverage` / `<command> --cov` |
| 失败时停止 | `<command> --bail` / `<command> -x` / `<command> --fail-fast` |
| 调试模式 | `<command> --verbose` / `<command> -v` |

**关键**：不确定具体参数时，先查看测试配置文件（pyproject.toml / package.json / Cargo.toml 等）中的已有设置，或运行 `<command> --help`。

---

## 五、测试数据管理

### 5.1 数据提供方式选择

| 方式 | 适用场景 | 优点 | 缺点 |
|------|---------|------|------|
| 内联数据 | 简单、少量测试数据 | 直观、自包含 | 复杂数据时混乱 |
| 工厂函数 | 需要灵活构造对象 | 可复用、灵活 | 需要维护工厂代码 |
| Fixture 文件 | 复杂且固定的数据结构 | 清晰、可独立维护 | 文件分散 |
| 数据库种子 | 集成测试 | 真实 | 慢、环境依赖 |

**选择依据**：优先内联 → 数据复杂时用工厂 → 大型固定数据用文件 → 集成测试用种子。

### 5.2 内联数据（首选）

```python
@pytest.mark.parametrize("input_val,expected", [
    (1, 2),
    (2, 4),
])
def test_double(input_val: int, expected: int) -> None:
    assert double(input_val) == expected
```

### 5.3 工厂函数

```python
class UserFactory:
    @staticmethod
    def create(
        user_id: str = "default-id",
        name: str = "Test User",
        **kwargs,
    ) -> User:
        return User(id=user_id, name=name, **kwargs)

def test_user_email_validation():
    user = UserFactory.create(email="invalid")
    with pytest.raises(ValidationError):
        validate_user(user)
```

**工厂函数放在**：
- 单项目：`tests/factories.py` 或 test 文件内
- 复杂项目：`tests/factories/<module>_factory.py`

### 5.4 Fixture 文件

```
tests/
└── fixtures/
    ├── user_response.json
    └── order_list.json
```

```python
@pytest.fixture
def sample_user():
    with open("tests/fixtures/user_response.json") as f:
        return json.load(f)
```

---

## 六、TDD 工作流（红-绿-重构）

### 标准循环

```
Step 1: [红] 写一个会失败的测试
  → 明确表达需求

Step 2: [绿] 用最简方式让测试通过
  → 最小改动

Step 3: [重构] 优化代码，行为不变
  → 整洁实现

Step 4: 重复
```

### 适用规则

| 场景 | 规则 |
|------|------|
| 修复 Bug | **必须** 先写重现 Bug 的测试 |
| 新功能 | **建议** 先写测试（至少核心路径） |
| 简单工具函数 | 可选，先实现后补测试 |

---

## 七、测试质量指标

### 7.1 覆盖率参考

| 代码层级 | 建议最低 | 建议理想 |
|---------|---------|---------|
| 核心业务逻辑 | 90% | 100% |
| 服务/用例层 | 80% | 90% |
| 接口层 | 70% | 85% |
| 基础设施 | 60% | 80% |

### 7.2 测试质量自检

- [ ] 每个测试独立运行，不依赖其他测试
- [ ] 不依赖外部服务可用性（外部依赖已 Mock）
- [ ] 不依赖执行顺序
- [ ] 没有硬编码的环境路径/凭据
- [ ] 测试命名清晰表达意图
- [ ] 每个测试只验证一个行为
- [ ] 测试数据在 setUp 或 fixture 中清理
- [ ] 没有跳过（skip/pending）的测试
- [ ] 没有注释掉的测试代码

---

## 八、常见反模式

| 反模式 | 问题 | 改进 |
|--------|------|------|
| 测试共享状态 | 测试间相互干扰 | 为每个测试创建独立实例 |
| 过度 Mock | 测试与实现强耦合 | 只 Mock 外部边界 |
| 测试实现细节 | 重构时大量失效 | 测试行为/输出而非内部调用 |
| 断言不足 | 测试通过但未验证 | 检查完整结果和副作用 |
| 睡眠等待 | 慢且不稳定 | 使用轮询、回调或信号量 |
| 测试过重 | 维护成本高 | 简化测试，减少集成测试 |

---

## 九、CI 集成

### 9.1 CI 验证流水线

```
[Lint] → [Type Check] → [Unit Tests] → [Integration Tests] → [Coverage]
```

### 9.2 CI 规则

```
单元测试失败 → 阻止合并
Lint 失败     → 阻止合并
覆盖率下降   → 警告（可配置是否阻止）
集成测试失败 → 警告（依赖环境可能不可用）
```

### 9.3 CI 配置检测

在添加 CI 配置前，先检查项目是否已有：
- `.github/workflows/` — GitHub Actions
- `.gitlab-ci.yml` — GitLab CI
- `Jenkinsfile` — Jenkins
- `.circleci/config.yml` — CircleCI
- `.drone.yml` — Drone CI

**已有 CI → 在现有配置中添加测试步骤**
**无 CI → 根据项目托管平台创建初始配置**

---

## 附录：已知测试框架快速参考

以下为常见框架的配置和命令参考。**实际使用前应先检测项目是否已配置。**

| 语言 | 框架 | 检测指纹 | 常用命令 |
|------|------|---------|---------|
| Python | pytest | `[tool.pytest]` in `pyproject.toml` | `pytest -v --cov=src` |
| Python | unittest | `unittest` in imports | `python -m unittest discover` |
| TypeScript | Vitest | `vitest` in `package.json` | `npx vitest run --coverage` |
| TypeScript | Jest | `jest` in `package.json` | `npx jest --coverage` |
| TypeScript | Mocha | `mocha` in `package.json` | `npx mocha 'tests/**/*.test.ts'` |
| Rust | built-in | `Cargo.toml` (默认) | `cargo test -- --nocapture` |
| Go | built-in | `go.mod` (默认) | `go test -v -cover ./...` |
| Java/Gradle | JUnit | `build.gradle*` 含 `test { }` | `gradle test --info` |
| Java/Maven | JUnit | `pom.xml` 含 `junit` | `mvn test -Dtest=MyTest` |
| .NET | xUnit/NUnit | `.csproj` 含包引用 | `dotnet test --verbosity normal` |
| Ruby | RSpec | `rspec` in `Gemfile` | `bundle exec rspec` |
| Elixir | ExUnit | `mix.exs` (默认) | `mix test` |

**如果项目使用的框架不在上表**：
1. 读取已有测试文件中的 import/require 语句
2. 使用 WebFetch 搜索该框架的使用方式
3. 参考该项目已有测试的写法
