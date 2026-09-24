# 四种交换写法 —— 构建与实测
#
#   make            编译全部
#   make run        跑自检（四种写法 × 5 组值 + 同地址陷阱）
#   make run-all    跑全部三个实验
#   make asm        生成汇编对照（需要 pwsh）
#   make clean

CC      ?= gcc
CFLAGS  ?= -O2 -Wall -Wextra -std=c99
BUILD   := build

BINS := $(BUILD)/swap $(BUILD)/test_overflow $(BUILD)/test_alias

.PHONY: all run run-all asm clean

all: $(BINS)

$(BUILD):
	@mkdir -p $(BUILD)

$(BUILD)/swap: src/swap.c src/swap.h | $(BUILD)
	$(CC) $(CFLAGS) -o $@ src/swap.c

$(BUILD)/test_overflow: src/test_overflow.c | $(BUILD)
	$(CC) $(CFLAGS) -o $@ src/test_overflow.c

$(BUILD)/test_alias: src/test_alias.c | $(BUILD)
	$(CC) $(CFLAGS) -o $@ src/test_alias.c

run: $(BUILD)/swap
	@echo "== 四种写法自检 =="
	@$(BUILD)/swap || true

run-overflow: $(BUILD)/test_overflow
	@echo "== 整数溢出 vs 浮点误差 =="
	@$(BUILD)/test_overflow

run-alias: $(BUILD)/test_alias
	@echo "== 同地址异或清零 =="
	@$(BUILD)/test_alias

run-all: run run-overflow run-alias

# 汇编对照。脚本在 Windows 上用 pwsh，逻辑见 tools/compare-asm.ps1
asm:
	pwsh -NoProfile -File tools/compare-asm.ps1 -Compiler $(CC)

clean:
	rm -rf $(BUILD)
