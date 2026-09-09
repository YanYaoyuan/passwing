# PassWing 代码审计、架构说明与重构建议

> 审计日期：2026-09-04  
> 审计对象：当前工作区快照（该目录不是一个可用的 Git 工作树，无法给出提交号）  
> 结论性质：静态代码审阅 + 构建配置验证；未能完成 GUI 运行与气动结果对标

## 1. 结论先行

PassWing 是一个功能野心很大的单体 Qt/C++ 桌面工程：它把翼型设计与 XFoil 分析、机翼/飞机/螺旋桨 VLM、BEMT、遗传优化、VTK 可视化、项目文件、远程网格/FUN3D 服务等放进了同一个程序。作者具备明显的空气动力学知识，也能把 Qt、Eigen、VTK、HDF5、网络与第三方求解器组合成可操作的原型。

但当前代码仍属于“研究原型/个人工具”，不能按生产级工程发布。主要原因不是界面风格或命名，而是以下基础质量门槛尚未建立：

- 当前源码快照不可重复构建，且仓库缺少运行所依赖的大量 `libaries/` 数据和 `help/` 源文件。
- 存在已确认的数据损坏、数组越界、空指针、双重释放、未初始化值和除零路径。
- 登录密码、Bearer token 和任务数据通过固定 IP 的明文 HTTP 传输，属于必须立即处理的安全问题。
- 求解器输入缺少统一的不变量检查，多个数值算法在退化输入下返回错误结果、NaN 或崩溃。
- UI、业务流程、求解算法、持久化和基础设施高度耦合；五个界面类合计超过 2.2 万行。
- 没有项目测试、持续集成、格式化/静态检查门禁，也没有可验证的数值基准。

综合评价：作为单人完成的跨学科原型，功能覆盖和探索能力值得肯定；作为需要长期维护、多人协作或向用户交付的工程，当前成熟度偏低。建议不要直接“大重写”，而应先建立可构建、可测试、可回归的安全网，再按边界逐步替换。

## 2. 审计范围和方法

### 2.1 规模

- 仓库共有 397 个文件。
- C/C++ 与 CMake 共约 217,643 行。
- 排除三份 QCustomPlot、两份 XFoil 后，自研/集成层约 60,940 行。
- 最大的自研实现文件：
  - `src/wing/wingdisplay.cpp`：5,966 行
  - `src/propeller/propellerdisplay.cpp`：5,923 行
  - `src/airfoil/airfoildisplay.cpp`：4,630 行
  - `src/aircraft/airplanedisplay.cpp`：3,717 行
  - `src/wing/wingvlm.cpp`：3,654 行
  - `src/propeller/propellervlm.cpp`：3,025 行
  - `saas/funClass/structwidget.cpp`：2,905 行

### 2.2 做了什么

1. 盘点全部源码、头文件、资源、配置、理论资料和第三方副本。
2. 逐文件审阅自研代码；对超大文件同时按类、函数、线程、I/O、容器索引、所有权和数值运算进行交叉检索。
3. 区分自研代码、移植/第三方代码和未进入构建的残留代码。
4. 执行 CMake 配置验证。
5. 对 55 个实现文件执行 Cppcheck（warning/style/performance/portability）。
6. 对关键告警回到源码上下文人工确认，未把静态检查结果直接当作事实。
7. 核对 QRC 资源清单、运行时路径、重复文件哈希和测试文件。

### 2.3 验证边界

- CMake 在生成阶段失败，无法继续编译、链接和启动 GUI。
- 仓库没有自动化测试；因此气动算法只能检查实现一致性和边界条件，不能证明物理结果正确。
- `libaries/` 整个目录缺失，翼型库、螺旋桨库和机场库无法做端到端验证。
- 远程服务是固定公网 IP，本次审计未向它发送请求。
- Cppcheck 总计报告 164 项未初始化成员、15 项潜在空指针等；部分是第三方代码或延迟初始化成员，本文只列出能从调用路径确认或风险很高的项目。

## 3. 总体架构

### 3.1 当前结构

```text
main.cpp
  └─ mainWindow（应用装配、导航、全局信号总线、项目读写入口）
      ├─ airfoilDisplay
      │   ├─ airfoilLibary / airfoilExplorer
      │   ├─ airfoilDesign（CST/几何变形）
      │   ├─ airfoilSolve → XFoil
      │   ├─ airfoilAnalyse / airfoilOptimization / airfoilOutput
      │   └─ cfdDataReader → HDF5
      ├─ wingDisplay
      │   ├─ wingDefinition
      │   ├─ wingVLM
      │   └─ wingOptimization
      ├─ propellerDisplay
      │   ├─ propellerLibary
      │   ├─ propellerDesign
      │   ├─ propellerVLM
      │   └─ propellerBemt
      ├─ airplaneDisplay
      │   ├─ airplaneVLM（组合 wingVLM）
      │   ├─ airplaneStability（未完成）
      │   └─ airplaneDesign（空壳）
      ├─ cfdDisplay（SaaS/FUN3D 工作台）
      │   ├─ structWidget / structCFDDefinition
      │   ├─ LoginWidget → jobManager → HTTP 服务
      │   ├─ fun3DRun → QProcess/mpiexec
      │   └─ flowViewer / pltReader / ugridViewer → VTK
      ├─ airplaneLibrary / STLReader
      ├─ chatWidget（占位实现）
      └─ PublicClass + publicWidgetClass（数学、文件、图表和控件）
```

### 3.2 关键数据流

1. `mainWindow` 在构造函数中一次性创建所有大页面，并用 Qt 信号直接互连。
2. 翼型页面产生 `QVector<QVector<QVector<double>>>`，直接传给机翼、螺旋桨和 CFD 页面。
3. 机翼页面再把 `wingDefinition` 数组直接传给飞机页面。
4. 各 Display 类从控件读取字符串/数值，直接构造求解器、启动并发任务、保存结果、更新图表。
5. 项目保存由 `myFile` 把多个页面的公开数组拷贝到自定义文本格式；读入后再通过主窗口信号覆盖各页面。
6. SaaS 页面把翼型和参数交给 `LoginWidget/jobManager`，通过 HTTP 提交任务、轮询/SSE 获取状态，并把网格写到当前目录。

这不是严格的分层架构，而是“页面对象互相传公开数据 + 页面直接调用基础设施”的事件驱动单体。它能快速做功能，但任何数据结构变化都会穿透 UI、算法、保存格式和网络层。

### 3.3 模块评价

| 模块 | 当前职责 | 主要问题 |
|---|---|---|
| `mainwindow.*` | 组装、导航、总线、项目入口 | 全局状态 `ANALYSE_TYPE` 驱动；页面强耦合；所有页面启动即创建 |
| `AirfoilClass` | 翼型库、CST、XFoil、优化、图表 | 业务与 UI 混杂；固定 600 数组；库数据缺失；异常与边界策略不一致 |
| `WingClass` | 几何、VLM、优化、可视化 | God Object；模型字段未初始化；优化数据编码脆弱 |
| `propellersClass` | 桨库、设计、BEMT/VLM、可视化 | 多处除零/越界前置条件缺失；两套求解路径重复 |
| `AirPlaneClass` | 多部件组合 VLM、稳定性、显示 | 稳定性尚未完成；多个 solver 用裸指针持有；结果可信度无基准 |
| `saas` | CFD 参数、任务、登录、结果可视化 | 明文 HTTP；固定服务地址；状态机和错误处理脆弱；仅 Windows |
| `PublicClass` | 数学、文件、结构、QCustomPlot | 工具函数契约不清；项目格式无版本/校验；第三方代码重复 |
| `publicWidgetClass` | 自定义 Qt 控件 | 命名/所有权不统一；部分来自 Qt 示例，需要许可证清单 |

## 4. 缺陷与 Bug 清单

优先级定义：P0 = 安全、数据损坏、稳定崩溃或阻止交付；P1 = 结果错误、高概率崩溃、核心功能不可用；P2 = 可维护性、性能或局部体验问题。

### 4.1 P0：应在任何重构前先修

#### P0-01 当前快照不可构建

证据：

- `CMakeLists.txt:2` 只启用 `CXX`，实际 CMake 配置在 VTK 的 MPI C 依赖处失败。
- `CMakeLists.txt:4-5,10` 写死个人电脑上的 Qt/HDF5 Windows 路径。
- `CMakeLists.txt:100-101` 与 `mainwindow.cpp:2` 引用了不存在的 `help/htmlviewer.h/.cpp`。
- CMake 中 15 个源路径只在大小写不敏感文件系统上成立，例如 `AirplaneClass` 实际为 `AirPlaneClass`、`publicClass` 实际为 `PublicClass`。
- `CMakeLists.txt:102` 的 `customscrollArea.h` 与实际 `customscrollarea.h` 不一致。
- `CMakeLists.txt:30` 引用不存在的 `3rdLib/Eigen3/...`。
- `saas/publicClass/fun3drun.cpp:2` 无条件包含 `windows.h`。

影响：无法建立 CI，无法可靠复现作者机器之外的构建。Windows 上仍会因缺少 `help` 文件失败。

修复：使用 `project(... LANGUAGES C CXX)`；现代化 `find_package` 和 imported target；删除所有机器绝对路径；统一真实文件名大小写；明确 Windows-only 源的条件编译；补齐或移除 help 模块；提供锁定版本的依赖说明与干净构建脚本。

#### P0-02 账号密码和令牌通过明文 HTTP

证据：`saas/login/loginwidget.cpp:77-89,125-133` 把账号密码 POST 到 `http://112.46.144.60:8000`；`saas/manager/jobmanager.cpp:30-33,97-100,270-273,349-352,420-423,554-557` 在 HTTP 上发送 Bearer token。

影响：同网段、代理链路或被劫持路由上的攻击者可以窃取密码、token 和计算数据，也可以篡改响应。

修复：立即停用 HTTP 入口，启用 HTTPS、有效证书和主机名；服务地址从受控配置读取；设置超时、重试上限和证书错误策略；token 仅存内存或系统凭据库；服务端吊销已经暴露链路上的 token。

#### P0-03 项目保存会写错数据并可能越界崩溃

证据：`src/common/myfile.cpp:344-395` 保存螺旋桨，`390` 行却写 `tailArray[i].num`，而不是 `propellerArray[i].num`。

影响：桨叶数被尾翼数组污染；当螺旋桨数量大于尾翼数量时直接越界。保存成功对话框仍无条件显示（`mainwindow.cpp:723-728`）。

修复：先做最小修补并添加 round-trip 测试；随后淘汰该行协议，使用有 schema/version 的 JSON、CBOR 或真正 XML，并通过 `QSaveFile` 原子提交。

#### P0-04 项目读取器对损坏/空文件不设防，并保留旧数据

证据：

- `src/common/myfile.cpp:23-49` 未检查行数和 `QStringList` 长度就访问 `[0]`、`[1]`。
- `src/common/myfile.cpp:64` 在 EOF/空行时执行 `line.at(0)`。
- 后续各数组按一条数组的长度索引其他数组，没有一致性校验。
- `src/common/myfile.cpp:454-459` 的 `clearData()` 只清翼型、机翼和尾翼，漏掉螺旋桨与飞机；再次打开项目会混入旧对象。
- 文件对话框称其为 `.xml`，内容并不是 XML，也没有格式版本。

影响：用户选择截断文件即可崩溃；失败读取可能先清掉当前内存数据；多次打开项目产生幽灵对象。

修复：先解析到临时 `Project`，完整校验成功后一次性交换；所有字段使用带错误信息的解析；校验数量、维度、有限数、索引和 EOF；格式必须带版本并可迁移。

#### P0-05 `airfoilSolve` 可写爆固定栈数组且早退泄漏

证据：`src/airfoil/airfoilsolve.cpp:62-76,136-160,301-321` 用 `double x[600]` 等固定数组，却按未限制的 `airfoilData.size()` 写入；每个点也默认至少两列。`XFoil` 用 `new` 创建，多个初始化/`specal()` 失败分支在 `delete` 前返回（如 `151-160,204-207,312-321,342-345`）。

影响：超过 600 点会破坏栈；畸形点会越界；重复失败会泄漏大对象。

修复：入口强制 `2 <= n <= XFoilLimit` 且每点两列、值有限；使用 `std::unique_ptr<XFoil>` 或栈对象；将错误作为结构化结果返回。

#### P0-06 QCustomPlot 图对象被双重删除

证据：`src/airfoil/airfoilexplorer.cpp:1847-1854` 先调用 `plot->removeGraph(graphPtr)`，随后又 `delete graphPtr`。QCustomPlot 自身在 `src/common/qcustomplot.cpp:14446-14458` 明确实现为 remove 并 delete。

影响：清图时 double free / heap corruption，崩溃位置可能远离根因。

修复：只调用 `removeGraph`，随后把非拥有指针设空；审计所有第三方容器的所有权语义。

#### P0-07 通用样条函数在上界外访问越界

证据：`src/common/mymath.cpp:397-452` 在 `xq > x.last()` 时把区间索引递增到 `n-1`，然后读取只有 `n-1` 个元素的 `b[i]`、`d[i]`。

影响：任何高于采样上界的查询都可能崩溃或返回随机值；该函数被推力插值等路径复用。

修复：明确采用 clamp、线性外推或最后区间外推，并把索引限制在 `[0,n-2]`；验证 x 严格递增、无重复、值有限。

#### P0-08 螺旋桨设计可能返回未初始化的 `K`

证据：`src/propeller/propellerdesign.cpp:712-725` 中 `K` 只在 `while (fabs(K2-K1) > tolerance)` 内赋值；当 `tolerance >= 1` 或 NaN 时直接返回未初始化值。

影响：后续几何/性能计算被随机值污染，且结果可能看似正常。

修复：初始化中点；验证 tolerance 为有限正数且小于区间；设最大迭代数；验证目标被上下界夹住并返回收敛状态。

#### P0-09 VTK 显示模式持有空/失效 Actor

证据：

- `saas/resultClass/flowviewer.cpp:36-41` 把 `airfoilActor`、`meshActor` 设空，但状态设为 `ShowAirfoil`。
- `switchToAirfoil/switchToMesh`（`938-962`）直接解引用两个 Actor。
- `loadFlattenedGridAuto` 在 `905-923` 创建局部 `actor` 并加入 renderer，却从未赋给成员 `meshActor`。
- `setAirfoil` 没有同步 `currentMode`。

影响：常见的翼型/网格切换顺序可空指针崩溃；状态与画面不一致。

修复：构造时创建两个成员 Actor，或在切换前完整判空；加载函数更新成员与状态；用一个显式状态机集中管理可见性。

#### P0-10 CFD 项目目录校验逻辑反了

证据：`saas/cfd/cfddisplay.cpp:1162-1177` 使用 `runDir.isEmpty() && QFile::exists(runDir + "/nodet_mpi.exe")`。空目录下通常不存在文件，非空但缺文件时左侧又为 false，因此警告几乎永远不会出现。

影响：取消选择或选择无求解器目录仍创建项目，失败被推迟到更深层。

修复：分别处理取消；使用 `if (!QFileInfo::exists(QDir(runDir).filePath(...)))`；验证可执行性和平台对应名称。

#### P0-11 BEMT 多个输入组合会立即越界/除零

证据：

- `propellerBemt::readInterDrag` 在列表少于 3 项的分支仍访问 `list.at(1)`（`src/propeller/propellerbemt.cpp:36-64`），空列表还在 `41` 行访问 0。
- `getReIndex` 对空数组返回 -1，对单元素数组访问 `len-2`（`414-423`）；调用方立即用返回值索引（`259-266`）。
- `initialAnalyse` 对空 `spanW` 调用 `last()`（`112`）。
- `computeForce/iterCompute` 对零 RPM、零来流、零半径、零桨叶数、空极曲线有多处除法（`183-207,323-328`）。

影响：UI 空输入或缺失 `libaries` 数据即可触发崩溃/NaN。

修复：建立 `validate(BemtInput)`；极曲线至少两组 Re 且各列同长；几何至少两个站位；所有物理量范围明确；迭代遇到非有限值立即失败并给 UI 可读错误。

### 4.2 P1：影响结果正确性或核心功能

#### P1-01 大量成员未初始化

Cppcheck 在全工程报告 164 个 `uninitMemberVar`。高风险例子包括：

- `airplaneStability` 的全部导数、速度、质量（构造函数为空，`src/aircraft/airplanestability.cpp:4-8`）。
- `wingDefinition` 的 `realChord/vMeshType/uMeshType/span/area/aspectRatio/...`（`src/wing/wingdefinition.h:45-105`）。
- `wingVLM`、`propellerVLM` 的网格数、密度、模式和固定系数。
- `propellerBemt` 的诱导因子、温度、粘度和半径。
- 两个遗传优化器的 `bestIndex`、染色体长度等。

影响：字段只要在预期初始化路径之前被读取，就产生未定义行为或随机物理结果。

修复：所有值类型在声明处初始化；构造函数建立完整不变量；不可缺省的参数放入构造参数；删除“两阶段初始化”或让对象在未初始化态不可求解。

#### P1-02 稳定性分析实际上尚未实现

证据：`src/aircraft/airplanestability.cpp:10-85` 只生成 12 个扰动速度并打印；`72-74` 行明确把力和导数计算留作注释。旋转扰动 `46-51` 使用 `point3d` 的逐分量乘法（`src/common/structDefinition.h:79-81`），而刚体旋转速度应涉及叉积。

影响：界面入口 `mainwindow.cpp:659-660` 调用“飞机稳定性分析”，但无法得到可信的稳定导数或模态。

修复：在 UI 标为未实现或禁用；先写出坐标系、符号、单位和有限差分定义；实现叉积与基准工况；用公开算例对 Xu、Mq、短周期/荷兰滚等结果做回归。

#### P1-03 遗传算法状态在翼段之间串值

证据：`src/wing/wingoptimization.cpp:25-53` 的 `newSymbolTmp` 在外层翼段循环之外创建，循环中不清空，却每次把整个累积数组 append 到 `symbolCSTArray`。

影响：第二个及以后翼段使用错误的 CST 符号索引，搜索空间和生成几何与设置不一致。

修复：每个翼段创建独立局部 vector；更进一步用 `GeneSpec{min,max,resolution}` 代替平行数组和符号推断。

#### P1-04 机翼分段位置不是累计和

证据：`src/wing/wingoptimization.cpp:1252-1255` 构造站位时写成相邻两段长度之和；三段以上时第三个站位缺少第一段累计量。

影响：优化出的多段机翼几何站位错误，可能自交或翼展不符目标。

修复：`spanTmp.push_back(spanTmp.back() + segmentLength)`；增加单调递增、末端翼展一致性测试。

#### P1-05 翼型优化参数校验存在恒假条件和除零

证据：`src/airfoil/airfoildisplay.cpp:3544-3559` 先用 `threadNum` 做取模而不检查 0；CST 比例判断写成 `>=1 && <=0`，永远为假。

影响：线程数为 0 时崩溃；非法比例进入优化。

修复：先验证 threadNum > 0，再做整除；条件改为 `<=0 || >=1`（是否包含端点需产品定义）；所有表单统一由 validator + domain validation 双层保护。

#### P1-06 数学工具 `minIndex` 返回错误索引

证据：`src/common/mymath.cpp:343-355` 找到更小误差时执行 `j++`，而不是 `j=i`。

影响：返回的是“刷新最小值的次数”，不是最近元素位置，调用方会选错数据。

修复：使用 `std::min_element` 或保存真实索引；为空返回 `optional`/错误。

#### P1-07 其他数学函数缺少基本契约

例子：

- `linearInterpolation` 只拒绝 size 0，size 1 在外推分支访问 `[1]`（`src/common/mymath.cpp:169-205`）。
- `getMaxIndex/minV/maxV` 等对空数组访问 `[0]`（`320-373`）。
- `getInterpolateThrust` 固定读取前三条曲线且不验证 RPM/曲线长度（`16-23`）。
- 插值没有统一检查 x 是否有序、重复或有限；同类函数有的 throw、有的返回 0、有的 `qFatal` 结束进程。
- 大气模型超出对流层适用高度后仍直接套公式（`260-279`），可能产生负温度/NaN。

修复：给每个公共算法写清输入域、单位、排序、外推和错误策略；将数据预验证与纯算法分离。

#### P1-08 CST 拟合显式求逆且未检查秩

证据：`src/airfoil/airfoildesign.cpp:161-175` 使用 `(SᵀS).inverse()SᵀY`，仅用首个系数是否 NaN 判断成功。

影响：正规方程放大条件数；重复点、点数不足或退化几何产生不稳定系数/Inf，后续仍可能标记成功。

修复：使用带秩判定的 QR/SVD；检查点数、矩阵秩、全部系数有限和拟合残差；保留失败原因。

#### P1-09 `wingDefinition::computeWingMessage` 的模型不变量不完整

证据：`src/wing/wingdefinition.h:107-142` 未检查数组同长、至少两站、弦长/面积非零、grid 非空；`meshNum` 无论 `isSymmetry` 都乘 2；`realArea` 从未赋值；`gridV[0]` 直接访问。

影响：面积、展弦比、网格数可能错误或越界，且 getter 返回未定义字段。

修复：把 `WingGeometry` 做成经校验的值对象；派生量一次计算且不可被任意修改；对对称与非对称网格分别定义。

#### P1-10 “并发”路径仍阻塞 UI，并允许重入

证据：

- `src/aircraft/airplanedisplay.cpp:152-156,212-218` 启动 `QtConcurrent` 后立即 `waitForFinished()`，GUI 线程仍冻结。
- `propellerDesign::startXfoil` 与 `wingDisplay::startVLMInThread` 用忙轮询 + `QCoreApplication::processEvents()`（`src/propeller/propellerdesign.cpp:503-513`、`src/wing/wingdisplay.cpp:5247-5257`）。
- 求解器 QObject 仍属于创建它的 GUI 线程，却被线程池直接调用；取消、窗口销毁和重复点击没有统一生命周期协议。

影响：长算例卡界面；`processEvents` 允许同一操作重入，可能同时修改数组或删除对象；退出时存在 use-after-free 风险。

修复：使用 `QFutureWatcher`/专用 worker；任务拥有不可变输入副本；结果一次性回传；按钮状态机阻止重复提交；支持 cooperative cancellation；析构时等待或取消任务。

#### P1-11 运行时依赖数据完全缺失

代码大量读取 `QDir::currentPath()/libaries/...`，但快照中没有 `libaries` 目录。受影响的包括机场模型、Profili/UIUC/WindAI 翼型、极曲线、APC 螺旋桨几何/性能。帮助代码与 `help/help.html` 也缺失。

影响：库窗口为空，BEMT 没有极曲线，主页机场与帮助不可用；部分路径会静默失败，部分会崩溃或 `exit(1)`（`src/airfoil/airfoillibary.cpp:257-259`）。

修复：明确哪些数据可分发；放入资源包/安装目录并在 CMake install 阶段安装；启动时做依赖自检；禁止依赖当前工作目录。

#### P1-12 Tecplot 解析器会吞掉 ZONE 行

证据：`saas/resultClass/pltreader.cpp:348-360` 解析带引号变量列表时，每轮末尾无条件再读一行；当下一行是 `ZONE` 时它被读入 `rest` 后因不以引号开头退出，但没有回放。随后 `readZone` 从下一行开始，找不到该 zone。

影响：常见 Tecplot ASCII 文件无法解析或得到 0 节点空网格。

修复：实现带 lookahead 的词法解析器，或一次读取头部再按 token 解析；用 POINT/BLOCK、单/多行 VARIABLES、多 Zone 样例测试。

#### P1-13 本地 FUN3D 进程状态和输出解析不可靠

证据：

- `saas/publicClass/fun3drun.cpp:21-38` 启动进程时不验证进程数、工作目录/可执行文件，不监听 `errorOccurred`，也不把 `isRun` 设 true。
- `138-145` 收到任何 stderr 数据就把 `isRun=false`，即使进程仍在运行。
- `102-131` 假定每次 `readyRead` 都按完整行边界到达，没有把末尾半行保留到下一 chunk。
- `82-89` 遇到含 Lift 或 Drag 的第一行就 break，若 Lift 和 Drag 分行可能只解析一个。

影响：UI 显示错误状态，迭代数据随机丢失或置零。

修复：建立 QProcess 状态机；持久化字节/行缓冲；分别解析字段；把 stderr 当日志，退出码/信号才决定完成；增加录制输出的解析测试。

#### P1-14 下载任务缺少认证和安全文件名处理

证据：`saas/manager/jobmanager.cpp:139-144` 下载网格的 GET 没带 Authorization；`165-180` 用当前目录和 `meshName` 拼本地路径，未清理路径分隔符，也未检查 `mkpath/remove/write` 的所有结果。

影响：服务端若未做额外保护可能越权下载；恶意/异常名称可能越出预期目录；旧文件先删除，写失败会丢数据。

修复：所有受保护请求统一走 API client 注入 token；仅允许安全 basename；用应用数据目录和 `QSaveFile`；校验 HTTP 状态、Content-Type、大小和数据格式。

#### P1-15 网络层不是可靠状态机

登录路径不先判断 `reply->error()`/HTTP 状态和 JSON parse error（`saas/login/loginwidget.cpp:91-116`）；多个请求没有超时；固定计时器、轮询和 SSE 可能并行；错误处理有的只注释掉日志；`jobManager` 在 LoginWidget 中无父对象（`loginwidget.cpp:18`），产生泄漏。

修复：抽出单一 `CfdApiClient`；统一请求超时、错误模型、认证、JSON schema、取消；任务由明确状态枚举驱动。

### 4.3 P2：结构、性能和可维护性问题

#### P2-01 God Object 与公开可变状态

五个 Display 类既创建数百个控件，又做文件 I/O、数据转换、线程调度、算法调用、图表和 VTK 更新。头文件暴露大量公开数组和裸指针，调用者可以绕过校验修改状态。

建议：每个页面拆为 `View + ViewModel/Controller + UseCase + Domain Service`；求解器不包含 QWidget；结果以不可变 DTO 传递。

#### P2-02 第三方代码重复且来源边界不清

- 三份 QCustomPlot 约 13 万行；其中两份 `.cpp` 完全相同，三份 `.h` 完全相同，另一份 `.cpp` 仅有分叉。
- 两份 XFoil 各约 1.2 万行，已经发生分叉。
- `src/widgets/callout.*` 带 Qt 示例版权声明；QCustomPlot 文件声明商业/GPL 许可。

影响：修复无法同步、编译变慢、许可证义务不清。静态检查还在 XFoil 中报告多个可疑 buffer size 告警，需要结合上游版本核查，不能简单忽略。

建议：建立 `third_party/`，每个依赖一份、记录版本/来源/本地补丁和 LICENSE；由独立 CMake target 构建；发布前由负责人确认 Qt/QCustomPlot/XFoil/HDF5/VTK 等许可兼容性。

#### P2-03 大量裸指针和模糊所有权

自研代码中检索到约 1,820 处 `new`、仅约 106 处显式 `delete`。Qt parent 和 VTK smart pointer 会接管其中不少对象，但仍有大量无 parent 的 dialog、solver、model 和跨线程对象，所有权只能靠读调用顺序猜测。

建议：QObject 必须立即给 parent，非 QObject 使用值语义/`unique_ptr`；头文件用 `QPointer` 表示可能被 Qt 删除的观察指针；不得手动删除由容器/scene/chart 接管的对象。

#### P2-04 错误处理策略分裂

相似错误分别采用返回 0、空数组、throw、`qFatal`、`exit(1)`、qDebug 或静默忽略。UI 调用方普遍不捕获异常，后台线程抛出异常可能终止进程。

建议：domain 层统一 `Result<T, Error>`（C++17 可自建/使用 expected 库），错误包含 code、message、context；只有不可恢复的编程错误才 assert，用户输入错误不得终止进程。

#### P2-05 命名和目录规范不足

`libary/libaries`、`Soluation`、`Angele`、`compoent` 等拼写散落在公开 API；类名有 `mainWindow`/`wingVLM`/`LoginWidget` 多种风格；目录大小写混用；`PublicClass` 与 `publicWidgetClass` 意义重叠。

建议：目录和文件一律小写 snake_case，类型 PascalCase，函数/变量 lowerCamelCase 或项目统一 snake_case；先用兼容适配器渐进改名，避免一次性全仓 rename 造成不可审查变更。

#### P2-06 死代码、空壳和注释噪声

`airplaneDesign`、`airfoilMesh`、`glWidget` 基本为空；`airfoilcompute.h` 复制了冲突的 include guard 和另一个 `airfoilAnalyse` 声明；`src/wing/wingdefinition.cpp` 定义了头文件已内联定义的默认构造，并定义未声明的五参数构造，但该 `.cpp` 又没有进入 CMake。大量被注释实现长期保留。

建议：删除无用途代码或明确放入实验分支；编译 target 必须覆盖所有声称属于产品的源；启用 `-Wall -Wextra -Wpedantic` 和 warnings-as-errors（第三方例外）。

#### P2-07 UI 和资源路径依赖当前工作目录

代码混用 `QDir::currentPath()`、`QCoreApplication::applicationDirPath()` 和 QRC。用户从不同快捷方式启动时，current path 不稳定；下载还写入工作目录。

建议：只读内置资源放 QRC；可安装数据使用明确的 install data dir；用户数据使用 `QStandardPaths::AppDataLocation`；项目路径由 ProjectContext 持有。

#### P2-08 重复算法与复制粘贴

四图表菜单、数据点、颜色、导出逻辑在 wing/airplane/propeller 页面高度重复；VLM 实现也存在大段同构代码；数组索引与魔法数字（5 个部件、15 个图表类型等）到处传播。

建议：抽取 `ChartPanel`、`ResultSeriesModel`、`GeometryRenderer`；用 enum class 和结构体替代平行数组；先通过 characterization tests 固化行为再消除重复。

#### P2-09 性能与数值可观测性不足

多个地方用 O(n²) 冒泡排序；样条每次查询都重建并求解完整矩阵；大量值按 QVector 深拷贝；求解只输出零散 qDebug，没有残差、条件数、收敛原因和输入摘要。

建议：排序使用 `std::sort`；样条预计算系数；大对象按 const reference/move；结果携带迭代数、残差、condition estimate、warnings 和版本信息。

#### P2-10 产品完整性细节

- `chatWidget` 只是把输入原样前缀返回，却显示为 “ChatGPT”（`src/chat/chatwidget.cpp:36-50`）。
- `mainWindow` 关于页写版本 1.4，而 CMake 项目版本为 0.1。
- 主窗口最小尺寸固定 1500×900，对小屏/缩放不友好。
- 很多保存函数不把打开/写入失败反馈给用户，却显示“保存成功”。

建议：未完成能力明确标注 Preview/占位或移除；版本由构建系统生成；所有成功提示必须来自真实成功结果。

## 5. 测试与构建结果

### 5.1 CMake

执行：

```bash
cmake -S . -B /tmp/passwing-cmake-audit -Wdev
```

结果：失败。首个错误是 VTK 请求 MPI C，而项目未启用 C。即使修复这一点，缺失 help 文件、Linux 大小写路径、`windows.h` 和缺失 Eigen 路径仍会继续阻止构建。

### 5.2 Cppcheck

对主要实现目录执行 C++17 静态检查。全量输出约 2,978 行，主要类别：

| 类别 | 数量 | 说明 |
|---|---:|---|
| `uninitMemberVar` | 164 | 部分为延迟初始化，求解器/模型中的值类型风险真实 |
| `nullPointerRedundantCheck` | 15 | 多个槽函数在判空前解引用 sender |
| `knownConditionTrueFalse` | 5 | 含固定分支与逻辑错误 |
| `argumentSize` | 6 | 位于 XFoil 分叉，需对照上游签名核查 |
| `uninitvar` | 1 | `propellerDesign::findK` 已人工确认 |
| `incorrectLogicOperator` | 1 | CST 比例恒假条件已人工确认 |
| `selfAssignment` | 1 | `double dt = dt = ...`，虽通常得到右值但表达错误 |

静态检查不是证明。比如项目保存错数组、双重删除、Tecplot 吞行、CFD 目录条件等都是人工数据流审阅发现的。

### 5.3 测试现状

仓库没有发现单元测试、集成测试或端到端测试，也没有 CTest 配置。`xfoil/xfoil-cpp-master/main.cpp` 是上游 sample，不是 PassWing 回归测试。

## 6. 建议的目标架构

```text
apps/passwing_qt
  ├─ views                 只负责控件和渲染
  ├─ view_models           页面状态、命令、异步进度
  └─ composition_root      对象装配和依赖注入

libs/domain
  ├─ geometry              Airfoil/Wing/Propeller/Aircraft 值对象与校验
  ├─ units                 Angle/Length/Speed/Density 等明确单位
  └─ project               Project 聚合、ID、schema/version

libs/solvers
  ├─ airfoil_xfoil_adapter
  ├─ wing_vlm
  ├─ propeller_bemt
  ├─ propeller_vlm
  ├─ aircraft_vlm
  └─ optimization

libs/application
  ├─ use_cases             AnalyseAirfoil/OptimizeWing/SubmitCfdJob...
  ├─ task_service          取消、进度、线程池、结果
  └─ ports                 Repository/ApiClient/Exporter 接口

libs/infrastructure
  ├─ project_json
  ├─ hdf5_repository
  ├─ cfd_http_client
  ├─ tecplot_reader
  └─ vtk_adapters

third_party
  ├─ xfoil                 单一、版本化、带补丁
  └─ qcustomplot           单一、带许可证

tests
  ├─ unit
  ├─ numerical_golden
  ├─ parser_corpus
  ├─ integration
  └─ gui_smoke
```

核心原则：

- Domain/solver 不依赖 QWidget、QChart、QFileDialog 或网络。
- UI 不直接改求解器公开数组，只发送经过校验的输入并接收结果。
- 一个 `Project` 是唯一事实源，页面展示它，而不是各自持有互相复制的数组。
- 远程 CFD、本地文件、HDF5、XFoil 都通过窄接口适配，便于测试替身。
- 第三方代码不允许复制到业务目录后各自修改。

## 7. 重构路线图

### 阶段 0：止血和建立基线（1～2 周）

1. 建立真正的 Git 仓库/确认完整源码与数据快照，补齐 `help`、`libaries` 或明确下载步骤。
2. 让一台干净 Windows CI 机器可配置、编译、打包；随后再恢复 Linux 可移植性。
3. 修复全部 P0，尤其 HTTPS、项目文件、双删、越界、未初始化和输入验证。
4. 为现有求解结果选 5～10 个代表算例，保存输入、输出、容差、软件版本，作为 golden baseline。
5. 开启 ASan/UBSan（可支持平台）、Cppcheck/clang-tidy 和基础 compiler warnings。

完成标准：干净环境一键构建；核心 smoke test 通过；P0 为零；保存失败不丢原文件。

### 阶段 1：先抽最稳定的纯逻辑（2～4 周）

1. 抽出 `math`、几何值对象、单位和 validation，不依赖 UI。
2. 为插值、CST、网格派生量、项目 round-trip、Tecplot/HDF5 解析写单测。
3. 用 `enum class`、`std::array`/有名结构替代魔法索引和平行数组。
4. 所有值类型默认初始化；所有 solver 变为“构造即有效”。

完成标准：纯逻辑库可在无 Qt Widgets/VTK 环境测试；边界/畸形输入覆盖。

### 阶段 2：建立 Project 聚合和持久化（2～3 周）

1. 定义带稳定 ID 的 `Project`、`Airfoil`、`Wing`、`Propeller`、`Aircraft`。
2. 使用 versioned schema；读取旧格式只放在 migration adapter。
3. 项目先完整验证后替换当前状态；写入使用原子保存。
4. 页面通过 project service 获取 snapshot/command，不再直接互抄公开 QVector。

完成标准：打开-保存-再打开完全等价；损坏文件不会改变当前项目；旧格式有迁移测试。

### 阶段 3：拆求解器与异步任务（3～6 周）

1. 按 XFoil/VLM/BEMT/优化拆独立 target，每个接口为 `Input -> Result`。
2. 结果包含成功状态、收敛信息、警告、残差、耗时和版本。
3. 引入 TaskService；线程只持有输入副本；UI 只订阅进度和最终结果。
4. 添加取消、超时、关闭窗口等待和重复提交保护。

完成标准：求解器可命令行/单测调用；UI 不再 `waitForFinished/processEvents`；TSan 可用路径无竞态。

### 阶段 4：拆 UI 和基础设施（持续）

1. 将大 Display 类按 page/panel/dialog/model/controller 拆分，单文件建议不超过约 500 行（不是硬指标，职责单一优先）。
2. 图表、导出、颜色菜单、数据点交互抽成复用组件。
3. `CfdApiClient` 统一 HTTPS、认证、错误、超时；`CfdJob` 实现明确状态机。
4. VTK renderer 只接受 domain result adapter，不读取 UI 控件。
5. 清理 dead code、重复库和注释代码。

### 阶段 5：可信计算与发布工程

1. 用文献/公开软件/实验数据对 XFoil、VLM、BEMT、稳定性建立误差预算。
2. 对网格加密、Re/Ma/迎角边界和不收敛行为做系统测试。
3. 记录输入单位、坐标系、solver version 和随机种子，保证结果可追溯。
4. 建立签名安装包、升级、崩溃报告、隐私说明和第三方许可证清单。

## 8. 必须落地的工程规范

### 8.1 C++/Qt

- C++17 先保持不变，除非工具链统一后再升级。
- 值类型成员声明处初始化；禁止未初始化裸数组。
- 非 QObject 所有权使用 `std::unique_ptr`/值语义；QObject 使用 parent ownership，并在接口上注明拥有/观察。
- 参数默认 `const&`，只有需要取得所有权时按值 + move。
- 不把可变容器设 public；通过小接口保持不变量。
- 禁止 `qFatal/exit` 处理用户数据错误。
- 跨线程 QObject 遵守 affinity；GUI 只能在 GUI 线程访问。
- 每个输入 DTO 都有 `validate()`；所有索引来自 ID 映射或受检访问。

### 8.2 数值计算

- 类型或字段名明确单位；角度不得靠注释区分 degree/radian。
- 每个算法文档化坐标系、符号、输入域、外推策略和收敛标准。
- 禁止无检查显式求逆；使用 QR/SVD/合适迭代器并检查秩、残差、条件数。
- 所有循环有最大迭代数；每步检查 `std::isfinite`。
- 浮点比较使用与量纲匹配的绝对/相对容差。
- 随机优化保存 seed，保证失败可复现。

### 8.3 文件和网络

- 格式有 magic、schema version、单位、校验和可选 migration。
- 写文件采用 `QSaveFile`；路径来自 `QStandardPaths` 或项目上下文。
- 解析器永不信任数量、维度、字符串或索引；设置尺寸和内存上限。
- 只允许 HTTPS；API base URL 配置化；统一超时、重试、认证和 JSON 校验。
- 日志不得输出密码、token 或完整敏感 payload。

### 8.4 工具链和评审门禁

- 根目录提供 `README.md`：依赖版本、配置、构建、测试、打包、数据获取。
- 提交 `CMakePresets.json` 的可用 developer/CI presets，不提交个人 `.user` 配置。
- 使用 `.clang-format`、`.clang-tidy`、EditorConfig；CI 检查格式、构建、测试、静态分析。
- 自研代码开启 warnings-as-errors；第三方 target 使用 system include/独立告警策略。
- PR 要求：行为说明、测试、数值容差依据、线程/所有权影响、截图（UI 变化时）。

### 8.5 测试矩阵

| 层级 | 必测内容 |
|---|---|
| 单元 | 插值边界、CST、几何派生量、单位、GA 编解码、状态机 |
| 属性测试 | 单调站位、数组同长、保存 round-trip、有限输出、对称性 |
| Parser corpus | 空/截断/超大项目文件，POINT/BLOCK Tecplot，多维 HDF5 |
| 数值 golden | NACA 翼型、矩形/椭圆翼、已知螺旋桨、网格收敛 |
| 集成 | XFoil adapter、项目加载、远程 API mock、文件安装路径 |
| GUI smoke | 页面切换、启动/取消求解、关闭窗口、图表清理、Actor 切换 |
| 动态分析 | ASan、UBSan；并发稳定后增加 TSan |

## 9. 对代码的评价

### 优点

- 单人覆盖了气动设计、数值求解、优化、可视化和云任务，领域跨度非常大。
- 已经有一定模块意识：Airfoil/Wing/Propeller/AirPlane/SaaS 的目录边界清楚。
- 使用了 Qt 信号槽、QtConcurrent、Eigen、VTK smart pointer、HDF5 RAII 等有价值的技术。
- 一些较新的代码能看到改进方向，例如 HDF5 使用 `unique_ptr`、VTK 使用 smart pointer、部分输入开始有检查、SSE 与任务轮询开始抽到 manager。
- 功能原型丰富，适合提炼需求和建立数值基准，不必推倒所有领域代码。

### 不足

- 核心短板是“缺少工程闭环”：没有可构建基线、测试、完整资源、错误模型和发布安全线。
- 复制粘贴和 UI 驱动开发使状态与索引快速扩散，局部修复很容易制造新回归。
- 数值代码普遍假定输入正确；对科学计算软件而言，这会直接伤害结果可信度。
- 未完成能力和正式入口混在一起，用户难以判断哪些结果可信。
- 安全设计明显滞后于 SaaS 功能增长，明文认证必须视为发布阻断项。

### 量化评分（以“可维护、可交付产品”为标准）

| 维度 | 评分（10 分） | 说明 |
|---|---:|---|
| 功能与领域覆盖 | 7 | 原型能力广，很多链路已有实现 |
| 架构与边界 | 3 | 有目录模块，但运行时高度耦合 |
| 正确性与鲁棒性 | 3 | 已确认多处 UB/数据错误，缺基准验证 |
| 可维护性 | 2 | 超大类、重复代码、公开状态、命名混乱 |
| 测试与可观测性 | 1 | 无项目测试，仅零散调试输出 |
| 安全性 | 1 | 明文凭据/token，固定 HTTP 服务 |
| 可移植与可构建性 | 1 | 当前快照无法配置完成，路径写死 |
| 综合 | **3/10** | 有价值的研究原型，不具备生产发布条件 |

这个评分不是对作者能力的否定。相反，代码说明作者能独立把复杂想法做出来；问题在于项目已经跨过了“个人原型还能靠记忆维护”的规模，却没有同步补上团队工程所需的边界、验证和安全机制。最值得做的不是追求一次漂亮的大重写，而是把已有领域成果逐步放进可验证的工程框架中。

## 10. 建议立即创建的前 12 个任务

1. 禁用 HTTP SaaS 入口，迁移 HTTPS 并轮换 token。
2. 修复 `myFile` 的 `tailArray[i].num`、全量 clear 和安全解析，补 round-trip 测试。
3. 修复 QCustomPlot double delete。
4. 修复样条上界越界、`minIndex`、单点/空数组契约，补单测。
5. 给 XFoil adapter 加点数/列数/有限数限制和 RAII。
6. 修复 CFD 目录条件、flowViewer Actor 状态机、Tecplot ZONE 吞行。
7. 为所有 domain/solver 值成员提供确定默认值，并在编译器告警下清零高风险告警。
8. 修复 BEMT 输入验证和所有零来流/零 RPM/空极曲线路径。
9. 修复遗传算法符号串值、累计翼展和 tolerance/线程数校验。
10. 重写 CMake 到干净机器可构建，补齐/移除缺失源和运行数据。
11. 建立最小 CI：configure、build、CTest、Cppcheck、ASan smoke。
12. 将稳定性分析和伪 ChatGPT 功能明确标为未实现，避免误导用户。

