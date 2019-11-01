# Cpp_coroutine

参考 [coroutine](https://github.com/cloudwu/coroutine)

## 开发平台

MacOS Mojave 10.14.6

CentOS Linux release 7.6.1810 (Core)

**Mac平台上实现的context貌似出现了问题,当前版本不再支持context有关api**

## 编译器版本

Apple clang version 11.0.0 (clang-1100.0.20.17)

GCC version 4.8.5 20150623 (Red Hat 4.8.5-39) (GCC)



## 技术要点

学习目的，主要做的还是对大佬的C版本用了C++的方法封装

+ 运用context保存协程上下文
+ 利用堆上的空间作为运行栈
+ 协程数目超限可平稳扩容