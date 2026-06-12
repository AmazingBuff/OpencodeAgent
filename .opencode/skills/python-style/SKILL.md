---
name: python-style
description: |
  遵循标准 Python 代码规范（PEP 8、PEP 257、PEP 484）进行代码编写、审查和优化。
  涵盖命名约定、格式化、类型注解、文档字符串、导入组织、测试规范等。
license: MIT
compatibility: opencode
metadata:
  audience: developers
  language: python
---

# 标准 Python 代码规范

## 1. 命名约定

| 类型 | 规范 | 示例 |
|------|------|------|
| 包名 | 全小写，不用下划线 | `mypackage` |
| 模块名 | 全小写，可用下划线 | `my_module.py` |
| 类名 | CapWords（大驼峰） | `class MyClass:` |
| 异常类 | CapWords + `Error` 后缀 | `class MyError(Exception):` |
| 函数/方法 | 全小写，下划线分隔（snake_case） | `def my_function():` |
| 变量 | 全小写，下划线分隔 | `my_var` |
| 常量 | 全大写，下划线分隔 | `MY_CONSTANT` |
| 私有属性/方法 | 前置单下划线 | `_private` |
| 强私有（name mangling） | 前置双下划线 | `__private` |
| 特殊方法 | 双下划线前后缀 | `__init__`, `__str__` |
| 类型变量 | CapWords 或简短 | `T`, `ResponseType` |

## 2. 代码布局（PEP 8）

- **缩进**：4 个空格（禁止使用 Tab）
- **行长度**：最大 88 字符（Black 默认），不超过 99 字符
- **空行**：
  - 顶层函数/类之间 **2 个空行**
  - 类内部方法之间 **1 个空行**
  - 逻辑段落间可用 **1 个空行** 隔开
- **导入**：每组之间 1 个空行，顺序：
  1. 标准库
  2. 第三方库
  3. 本地模块
- **表达式/语句**：
  - 每行一条语句
  - 二元运算符前换行
  - 避免尾随空格

## 3. 导入规范

```python
# 正确
import os
import sys
from typing import Optional

import requests
from pydantic import BaseModel

from mypackage import mymodule

# 禁止：通配符导入
from os import *           # ❌

# 禁止：多模块一行导入
import os, sys              # ❌

# 推荐：显式导入所需内容
from collections.abc import Sequence  # ✅
```

## 4. 类型注解（PEP 484 / PEP 604）

```python
from __future__ import annotations  # 支持 PEP 604 语法（Python 3.7+）

from typing import Optional, Union, Literal

# ✅ 现代语法（Python 3.10+）
name: str
count: int | None = None
result: list[str] = []

# ✅ 传统语法
name: str
count: Optional[int] = None
result: List[str] = []

# 函数注解
def greet(name: str, age: int = 0) -> str:
    return f"{name} is {age} years old"

# 禁止：运行时类型检查（除非必要）
def add(a: int, b: int) -> int:  # 注解仅用于静态检查
    return a + b
```

## 5. 文档字符串（PEP 257）

```python
def fetch_data(url: str) -> dict[str, Any]:
    """获取远程数据并返回解析后的字典。

    Args:
        url: 请求的目标 URL。

    Returns:
        解析后的 JSON 字典数据。

    Raises:
        HTTPError: 当请求失败或状态码非 2xx 时抛出。
    """
    ...

class DataProcessor:
    """数据处理基类。

    Attributes:
        batch_size: 每批处理的数据量。
    """
    ...
```

使用 **Google 风格** 或 **NumPy 风格**，保持项目内一致。

## 6. 格式化工具（推荐）

| 工具 | 用途 | 配置 |
|------|------|------|
| **Ruff** | 格式化 + lint（首选，速度极快） | 替代 flake8 + isort + black |
| **Black** | 代码格式化（坚持己见） | `--line-length 88` |
| **isort** | 导入排序 | 与 black 兼容的配置 |
| **mypy** | 静态类型检查 | 严格模式 `--strict` |

推荐 `pyproject.toml` 配置：

```toml
[tool.ruff]
line-length = 88
target-version = "py311"

[tool.ruff.lint]
select = ["E", "F", "I", "N", "W", "D", "UP", "B", "SIM"]

[tool.ruff.format]
quote-style = "double"

[tool.mypy]
strict = true
python_version = "3.11"

[tool.pytest.ini_options]
testpaths = ["tests"]
python_files = ["test_*.py"]
```

## 7. 测试规范

- 测试框架：**pytest**（首选）
- 文件命名：`test_<模块名>.py`
- 函数命名：`test_<功能描述>`
- 类命名：`Test<组件名>`
- 目录结构：
  ```
  tests/
  ├── __init__.py
  ├── test_utils.py
  ├── test_api.py
  └── conftest.py
  ```
- 使用夹具（fixture）替代 `setUp`/`tearDown`
- 测试函数应简洁、聚焦、可独立运行
- 优先使用 `parametrize` 做多场景测试

```python
# ✅ 正确示例
import pytest

@pytest.fixture
def sample_data() -> dict:
    return {"key": "value"}

def test_process_data(sample_data: dict) -> None:
    result = process_data(sample_data)
    assert result["key"] == "value"

@pytest.mark.parametrize("input_val,expected", [
    (1, 2),
    (2, 4),
    (3, 6),
])
def test_double(input_val: int, expected: int) -> None:
    assert double(input_val) == expected
```

## 8. 项目结构（推荐）

```
project/
├── pyproject.toml          # 项目配置（替代 setup.py）
├── src/
│   └── mypackage/
│       ├── __init__.py
│       ├── core.py
│       ├── utils.py
│       └── models.py
├── tests/
│   ├── __init__.py
│   ├── test_core.py
│   └── test_utils.py
├── docs/
├── .gitignore
├── README.md
└── requirements.txt
```

## 9. 编码最佳实践

- 优先使用 `pathlib` 替代 `os.path`
- 优先使用 f-string 替代 `%` 或 `.format()`
- 使用上下文管理器（`with` 语句）处理资源
- 使用 `dataclasses` 或 `pydantic` 定义数据模型
- 优先使用 `typing` 和标准类型提示
- 异常捕获要具体，避免裸 `except:`
- 避免可变默认参数
- 使用 `if __name__ == "__main__":` 守卫入口代码

```python
# ✅ 推荐
from pathlib import Path

data_dir = Path("data")
output = data_dir / "result.json"

with open(output) as f:
    data = f.read()

# ❌ 不推荐
import os
output = os.path.join("data", "result.json")
```

## 10. 代码审查检查清单

- [ ] 所有函数/方法都有类型注解
- [ ] 公有函数/类有文档字符串（Google/NumPy 风格）
- [ ] 无通配符导入（`from x import *`）
- [ ] 无可变默认参数（`def f(x=[])`）
- [ ] 行长度 ≤ 88 字符
- [ ] 缩进使用 4 空格
- [ ] 测试覆盖新增/修改的代码
- [ ] `mypy --strict` 通过无错误
- [ ] Ruff lint 通过无错误
- [ ] 无调试打印（`print`）或断点（`breakpoint()`）
