```markdown
# AGENTS.md

不要为可逆的、影响较小的变更编写测试，尤其是对于实现过程完全镜像的测试。如果您确实选择使用测试来验证您的工作，请确保这些测试有意义且对于验证实现过程是必要的。运行与变更相符的测试并完成必要的检查。一旦这些检查通过，仅当出现新的变更、故障或未解决的问题时，才需要扩展或重复测试；否则，继续完成任务。

## Avoid Over-Defensive Programming

- **Complexity:** Don't add complexity for speculative edge cases. For significant additional complexity, explain the failure scenario, likelihood, costs, and benefits, and get confirmation before proceeding.
- **Defensive programming:** Avoid speculative checks, redundant validation, unnecessary try/catch blocks, and fallback paths for impossible or unlikely states. Trust established invariants and internal contracts. Add defensive handling only for realistic failure scenarios.
- **Boundary validation:** Validate untrusted input once at trust boundaries. Trust already-validated internal data unless it crosses a new boundary.
- **Abstraction:** Avoid unnecessary abstractions, especially for single-use logic. Extract helpers when they improve clarity, encapsulate complexity, or represent meaningful concepts.
- **Error handling:** Handle errors where they can be meaningfully recovered from or contextualized. Don't silently swallow failures or add speculative fallback paths.

## 1. 项目与设备

本项目在 ArchLinux 本地环境中，编译 Mate 60 Pro 内核。

设备：

- 型号：`ALN-AL00`
- 代号：`Allen`
- SoC：海思麒麟9000S
- 原厂内核：5.10.43
- 设备序列号：FMR0223C20002621

目标：

1. 基于原厂 5.10.43 源码，先编译能开机进入系统正常工作的内核，再为后续移植更高版本和主线化做准备。
2. 优先采用 Linux 上游和标准 Android 内核写法。
3. 减少 OEM 定制、平台硬编码和不可移植代码。
4. 每次重要迭代记录修改、结论和后续工作。

---

## 2. 环境与命令

工作目录：

```text
/home/data/Allen/
```

实际参数以README 或项目已有脚本为准，不得猜测。所有命令都必须进入工作目录后执行，并检查：

- 当前路径；
- Git 状态；
- Python、gcc、clang、make；
- 交叉编译工具链；
- 磁盘空间。

---

## 3. 设备连接与序列号

第一次连接设备时，必须先确认设备身份，禁止未经确认直接刷写：

```cmd
adb devices
adb get-serialno
adb shell getprop ro.product.model
adb shell getprop ro.product.device
adb shell getprop ro.product.board
adb shell getprop ro.hardware
adb shell getprop ro.soc.model
adb shell uname -a
```

确认结果应符合：

```text
型号：ALN-AL00
代号：allen
SoC：9000s 或 8425L 等字样
```

同时记录：

- ADB 序列号；
- 当前内核版本；
- 当前启动模式；
- 设备型号和硬件平台。

如果型号、代号、SoC 或硬件平台不匹配，停止刷写，仅进行只读诊断。

首次确认后，所有 ADB 和 fastboot 命令都必须指定该序列号：

```cmd
adb -s <序列号> ...
fastboot -s <序列号> ...
```

禁止使用不带 `-s` 的设备操作命令。设备更换、序列号变化或记录丢失时，重新执行首次确认流程。

---

## 4. Root、Magisk 和 SELinux

当前 ramdisk 已使用 Magisk 修补，通常可通过以下命令获得 root：

```cmd
adb -s <序列号> shell su -c id
```

只有在诊断被 SELinux 拦截时，才临时设置宽容模式：

```cmd
adb -s <序列号> shell su -c setenforce 0
```

诊断结束后立即恢复
规则：

- 不把 `setenforce 0` 作为最终方案；
- 记录相关 AVC、域、类型和权限；
- 优先修复策略、标签、设备节点权限或驱动访问方式；
- 不以长期关闭 SELinux 替代问题修复。

---

## 5. boot、ramdisk 和镜像

本设备采用：

```text
boot 分区 + 独立 ramdisk 分区
```

因此：

1. 修改 ramdisk 时操作独立 ramdisk 镜像；
2. 不要默认将 ramdisk 重新打包进 boot；
3. 解包和重打包前确认镜像格式、头部版本、压缩方式和分区用途；
4. 原始镜像必须保留，不得覆盖唯一备份；
5. 刷写前确认目标分区真实存在且与镜像类型匹配；
6. 输出镜像名称应包含设备、用途和版本。

建议目录：

```text
images\original
images\extracted
images\modified
images\test
images\backup
scripts
tools
docs
.tmp
```

使用 `magiskboot` 等工具前，先确认输入文件格式和参数。

---

## 6. Fastboot 规则

刷写前确认：

```cmd
fastboot -s <序列号> devices
fastboot -s <序列号> getvar product
fastboot -s <序列号> getvar all
```

在设备、序列号、目标分区和镜像均确认无误后，以下命令无需额外批准：

```cmd
fastboot -s <序列号> flash boot <boot镜像>
fastboot -s <序列号> flash ramdisk <ramdisk镜像>
```

其他不擦除、格式化或覆盖用户数据的命令也可直接执行，例如读取信息、重启。

仍需明确批准的操作：

```cmd
fastboot erase <分区>
fastboot format <分区>
fastboot -w
fastboot flash userdata <镜像>
fastboot flash metadata <镜像>
fastboot flash cache <镜像>
```

以下高风险分区除非用户明确要求，否则不得刷写：

```text
xloader、fastboot、bl2、oeminfo、nvme、GPT、分区表
```

所有刷写仍必须：

- 使用 `-s <序列号>`；
- 保留原始镜像；
- 说明目标分区和镜像来源；
- 记录结果和失败恢复方式。

---

## 7. 大文件和日志

禁止直接把大型源码、日志或反编译结果完整输出到终端。

处理原则：

1. 优先按关键词搜索；
2. 必要时输出到 `.tmp` 临时文件；
3. 读取和筛选完成后立即删除；
4. 临时文件和完整日志不得提交 Git。

ADB 日志应使用已确认序列号：


## 8. 内核移植策略

移植顺序：

1. 配置通过；
2. 编译通过；
3. 内核启动；
4. 恢复存储、显示、触摸、USB、网络和电源管理；
5. 最后处理性能、功耗和 OEM 扩展功能。

遇到大问题时拆成小步骤，一次只处理一个子系统或一类错误。

优先参考顺序：

1. Linux 上游对应版本；
2. 标准 Android 公共内核；
3. 其他麒麟平台同大版本内核；
4. 其他相同 SoC或相同内核版本源码；
5. 官方 5.10.43 实现。

优先采用标准 driver model、device tree binding、时钟、电源、DMA、IOMMU 和中断接口。不要简单删除 OEM 代码；先判断其硬件功能，再进行标准化、兼容层改写或删除。

参考外部代码时记录来源、内核版本、适用 SoC、差异和采用原因。

---

## 9. 工作流程

### 环境确认

```cmd
cd
git status
git branch --show-current
git log -1 --oneline
```

源码树修改须基于git 以便后续查看修改记录以及回滚修改

### 设备基线

记录：

- 型号、SoC、序列号；
- 当前内核和启动模式；
- boot/ramdisk 分区布局；
- 原始镜像位置；
- 关键启动和运行日志；
- 当前源码分支和提交。

### 修改与编译

- 修改前先定位调用链；
- 使用独立输出目录；
- 保留用户已有未提交修改；
- 编译日志输出到临时文件；
- 先修复第一处真实错误，再处理后续错误；
- 不进行无关的大规模替换。

### 测试

优先顺序：

1. 静态检查；
2. 编译；
3. 设备读取和日志；
4. 可恢复测试；
5. 刷写测试。

测试时始终使用已确认的设备序列号。

---

## 10. 文档和 Git

每次记录：

- 当前状态；
- 修改文件和原因；
- 采用的实现及参考来源；
- 编译和测试结果；
- 已解决与未解决问题；
- 下一步计划；
- 重要风险和兼容性结论。

只记录长期有价值的信息。不要记录编译产物大小、临时文件名、临时校验值、完整重复日志或无关警告。

Git 规则：

- 不覆盖用户已有修改；
- 不执行未经确认的 `git reset --hard` 或批量删除；
- 保留原始镜像、设备树和配置；
- 编译产物、临时文件和完整日志不提交；
- 独立移植步骤尽量单独提交；
- 不修改与当前问题无关的文件。

建议 `.gitignore`：

```gitignore
.tmp/
.codex-tmp/
*.log
*.tmp
out/
build/
```

---

## 11. 故障分析和输出

故障分析顺序：

1. 明确现象；
2. 确认复现条件；
3. 获取最小必要日志；
4. 判断发生阶段；
5. 分类为配置、设备树、API/ABI、时钟、电源、中断、内存、DMA、SELinux 或镜像问题；
6. 只修改最相关的一处；
7. 重新测试并记录结果。

无法确定原因时，不要随机批量修改，应先增加诊断信息并缩小范围。

向用户汇报时只输出：

- 当前结论；
- 已执行操作；
- 修改文件；
- 测试结果；
- 关键错误和风险；
- 下一步最小操作。

不要直接粘贴超长终端输出。
```


---

## 12. 空间管理和额外说明

默认情况下，保持工作目录占用磁盘大小需小于20GB，超出时需要向用户汇报等待下一步指示
只保留最近一次可启动进入系统(若为移植早期版本则以启动流程走的最远为准)的版本和最近5个版本的内核编译产物打包boot镜像，超出此范围的诊断日志，编译缓存，产物在确认后续不再使用后均清理删除。
ALN_HarmonyOS4.2.0_Code_Opensource.tar.gz为当前设备官方开源文件
boot.img为官方固件内5.10.43内核boot镜像
ramdisk-magisk.img为已用magisk alpha 28102修补的ramdisk分区镜像
内核源码似乎指定bisheng编译器（只在arm64环境可用而当前架构为x86_64）但已有其他人使用clang12.0.1编译内核能开机
设备已刷入工程rec默认启动adb root以便无法启动时读取日志
除了内核日志外，麒麟设备还可以读取rrecord，bootfail_info等分区查看日志