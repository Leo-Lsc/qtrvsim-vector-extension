.globl _start

.text
_start:
    li x1, 1                    # 设置常量 1 到寄存器 x1
    li x2, 0                    # 设置 VL 为 0
    li x3, 46                   # 设置 AVL 为 46
    li x4, 4                    # 设置 SEW 为 4（元素宽度为 4 字节）

init:
    vsetvl  x2, x3, x4          # 设置向量长度（VL）和向量宽度（SEW）
    vadd.vx v1, v0, x1          # 向量加法，v1 = v0 + x1
    vsw.v   v1, 0x0(x0)         # 将 v1 存储到内存地址 0x0(x0)
    sub x3, x3, x2              # AVL 减少
    bnez x3, init               # 如果 AVL 不为 0，跳回 init 重新设置 VL

    li x3, 46                   # 重置 AVL 为 46
    li x5, 20                   # 设置矩阵 A 的行数（20）
    li x6, 50                   # 设置矩阵 B 的列数（50）
    li x7, 0x320                # 设置矩阵 C 的基地址为 0x320

    li x16, 0                    # 行索引 i = 0

outer_loop:
    li x13, 0                    # 列索引 j = 0

inner_loop:
    li x14, 0                   # 初始化累加器 x14 为 0

vector_mul:
    vsetvl x2, x3, x4           # 设置向量长度 VL 为 AVL（每次更新）
    
    vlw.v v1, 0(x0)             # 从内存加载矩阵 A 的当前行到 v1
    vadd.vx v1, v1, x16          # 向量加法，v1 = v1 + 行索引 i（获取矩阵 A 当前行）
    vadd.vx v2, v1, x13          # 向量加法，v2 = v1 + 列索引 j（获取矩阵 B 当前列）
    vmul.vv v3, v1, v2          # 向量乘法，v3 = A 行 * B 列
    vred x10, v3                # 向量归约，计算 v3 中元素的累加和
    
    add x14, x14, x10           # 将归约结果累加到累加器 x14
    sub x3, x3, x2              # AVL 减少
    bnez x3, vector_mul         # 如果 AVL 不为 0，继续向量乘法计算

    sw x14, 0(x7)               # 将累加结果 x14 存储到矩阵 C 的当前位置
    addi x7, x7, 4              # 更新矩阵 C 的内存地址，步进 4 字节（假设每个元素 4 字节）
    addi x13, x13, 1              # 列索引 j 加 1
    li x3, 46                   # 重置 AVL 为 46
    li x14, 0                   # 重置累加器 x14
    blt x13, x6, inner_loop      # 如果列索引 j 小于列数，继续内层循环

    addi x16, x16, 1              # 行索引 i 加 1
    li x13, 0                    # 列索引 j 重置为 0
    blt x16, x5, outer_loop      # 如果行索引 i 小于行数，继续外层循环

    li x3, 1000                 # 设置 AVL 为 1000，用于矩阵加载
    li x7, 0x320                # 设置矩阵 C 的基地址为 0x320

load_matrix:
    vsetvl x2, x3, x4           # 设置向量长度 VL 为 AVL（每次更新）
    vlw.v v1, 0(x7)             # 从矩阵 C 的内存中加载数据到 v1
    sub x3, x3, x2              # AVL 减少
    bnez x3, load_matrix        # 如果 AVL 不为 0，继续加载

    nop                         # 空操作（为了跳过不必要的延迟槽）
    nop                         # 空操作
    ebreak                      # 程序结束，触发断点
