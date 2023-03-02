# MaQueOS

本仓库为《MaQueOS：基于龙芯LoongArch架构的教学版操作系统》教材的代码仓库。

## 一、环境部署

1、在虚拟机中安装 ubuntu 20.04

2、更新源

```bash
sudo gedit /etc/apt/sources.list
```

```
deb https://mirrors.tuna.tsinghua.edu.cn/ubuntu/ focal main restricted universe multiverse
deb https://mirrors.tuna.tsinghua.edu.cn/ubuntu/ focal-updates main restricted universe multiverse
deb https://mirrors.tuna.tsinghua.edu.cn/ubuntu/ focal-backports main restricted universe multiverse
deb https://mirrors.tuna.tsinghua.edu.cn/ubuntu/ focal-security main restricted universe multiverse
```

```bash
sudo apt update
sudo apt upgrade
```

3、安装软件

```bash
sudo apt install libspice-server-dev libsdl2-2.0-0 libfdt-dev libusbredirparser-dev libfuse3-dev libcurl4 build-essential gcc-multilib libpython2.7 libnettle7 git
```

4、克隆仓库

```bash
git clone https://gitee.com/dslab-lzu/maqueos.git
```

## 二、仓库目录介绍

本仓库中包含《MaQueOS：基于龙芯LoongArch架构的教学版操作系统》教材中12章内容对应的实验代码，分别位于code*目录中。
例如第1章的实验代码在code1中，第2章在code2中，以此类推。

```
.
├── cross-tool  // 交叉编译环境
├── README.md
├── code1       // 第1章实验代码
├── code2       // 第2章实验代码
├── ...
└── code12      // 第12章实验代码
    ├── run             // 实验目录
    ├── xtfs            // xtfs目录
    │   ├── bin                 // MaQueOS应用程序
    │   │   ├── asm.h
    │   │   ├── compile.sh
    │   │   ├── create.S            // 创建hello_xt文件
    │   │   ├── destroy.S           // 删除hello_xt文件
    │   │   ├── hello.S             // 测试output系统调用和软件定时器
    │   │   ├── print.S             // 测试带参数的进程的创建
    │   │   ├── read.S              // 从hello_xt文件中读数据
    │   │   ├── share.S             // 测试页例外
    │   │   ├── shmem.S             // 测试共享内存
    │   │   ├── sync.S              // 测试sync系统调用
    │   │   ├── write.S             // 向hello_xt文件中写数据
    │   │   └── xtsh.S              // MaQueOS使用的shell程序（xtsh）
    │   └── src                 // xtfs工具源码
    └── kernel          // MaQueOS源代码目录
        ├── Makefile
        ├── drv                 // 硬盘、键盘、显示器驱动
        │   ├── console.c
        │   ├── disk.c
        │   └── font.c
        ├── excp                // 中断
        │   ├── exception.c
        │   └── exception_handler.S
        ├── fs                  // 文件系统
        │   └── xtfs.c
        ├── include             // 头文件
        │   └── xtos.h
        ├── init                // 初始化
        │   ├── head.S
        │   └── main.c
        ├── mm                  // 内存
        │   └── memory.c
        └── proc                // 进程
            ├── ipc.c
            ├── proc0
            ├── process.c
            └── swtch.S
```

## 二、编译运行调试

进入code*目录中的实验目录 `run` ，编译、运行、调试都在这个目录下进行。

1、编译运行

在 run 目录下，执行以下命令。

```
./run.sh
```

2、调试

调试分为两步，需要两个终端：

（1）在第一个终端中，在 run 目录下，执行以下命令。

```
./run.sh -d
```

（2）在第二个终端中，在 run 目录下，执行以下命令，启动gdb（gdb调试在该终端中进行）。

```
./gdb.sh
```
