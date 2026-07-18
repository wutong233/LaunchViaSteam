## 简介

本项目为一个轻量的 Windows GUI 代理启动器，用于解决将需要管理员权限启动的非 Steam 游戏通过 Steam 启动时，Steam 无法正确监测游戏运行/退出状态的问题。代理器以管理员权限启动目标可执行并保持运行，直至游戏进程结束，从而让 Steam 能够感知游戏生命周期。作者自用。

## 功能

- 以管理员权限启动指定的游戏可执行（ShellExecuteEx + runas）。
- 获取并等待启动的游戏进程句柄，游戏退出后代理器自动结束（无需轮询）。
- 支持与可执行同名的配置文件（.ini），首次运行可交互选择游戏路径并自动保存。
- 作为 Windows GUI 程序运行，无控制台窗口，适合静默部署在 Steam 中作为非 Steam 游戏的启动器。

## 用法


1. 首次运行：程序会弹出文件选择对话，选择要代理启动的游戏 exe（示例：D:\hkrpg_cn\StarRail.exe），程序会在同目录生成同名 .ini（例如 1.ini）并保存路径。
2. 将生成的 exe 添加为 Steam 的“非 Steam 游戏”，配置 Steam 启动该 exe。Steam 启动代理器后，代理器会以管理员权限启动真实游戏并等待其退出；当游戏结束，代理器退出，Steam 即可正确检测到游戏已关闭。
3. 如需编译：使用 Visual Studio 打开项目并编译，确保链接子系统为 Windows（/SUBSYSTEM:WINDOWS）。

## 声明

本 README 由 AI（GitHub Copilot）生成。
