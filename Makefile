
# wget https://raw.githubusercontent.com/luzhixing12345/libc/main/Makefile

CC = gcc
TARGET = libc
SRC_PATH = src
SRC_EXT = c
THIRD_LIB =

CFLAGS = -Wall -Wunused -Werror -Wformat-security -Wshadow -Wpedantic -Wstrict-aliasing -Wuninitialized -Wnull-dereference -Wformat=2
CFLAGS += $(shell pkg-config --cflags $(THIRD_LIB))
LDFLAGS += $(shell pkg-config --libs $(THIRD_LIB))

TEST_PATH = test
TMP_PATH = tmp
RELEASE = $(TARGET)
LIB = lib$(TARGET).a
# ------------------------- #

rwildcard = $(foreach d, $(wildcard $1*), $(call rwildcard,$d/,$2) \
						$(filter $2, $d))

SRC = $(call rwildcard, $(SRC_PATH), %.$(SRC_EXT))
OBJ = $(SRC:$(SRC_EXT)=o)


ifeq ($(MAKECMDGOALS),debug)
CFLAGS+=-g
endif

all: $(SRC_PATH)/$(TARGET)

debug: all

$(SRC_PATH)/$(TARGET): $(OBJ)
	$(CC) $^ $(LDFLAGS) -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# ------------------------- #
#          使用方法
# ------------------------- #
.PHONY: clean distclean lib release tar all test

# make : 编译
# make clean: 清除编译的中间文件
# make distclean: 清除所有编译结果
# make lib: 将所有obj整合到一个.a
# make release: 导出release库
# make tar: 打包release
# make install: 安装release库
# make uninstall: 卸载release库

# Define variables for formatting
CP_FORMAT = "[cp]\t%-20s -> %s\n"
MV_FORMAT = "[mv]\t%-20s -> %s\n"

mount:
	mkdir -p $(TMP_PATH)
	$(SRC_PATH)/$(TARGET) $(TMP_PATH)
	@echo "mount success"

umount:
	umount $(TMP_PATH)
	rm -rf $(TMP_PATH)
	@echo "umount success"

test:
	$(MAKE) clean
	$(MAKE) debug -j4
	python test.py

clean:
	rm -f $(OBJ) $(SRC_PATH)/$(TARGET)
release:
	$(MAKE) -j4
	mkdir $(RELEASE)
	@cp $(EXE) $(RELEASE)/ 
	tar -cvf $(TARGET).tar $(RELEASE)/

distclean:
	rm -r $(RELEASE) $(RELEASE).tar

help:
	@echo -e ""
	@echo -e "\tmake \t\t\t编译"
	@echo -e "\tmake debug \t\t编译debug版本"
	@echo -e "\tmake test \t\t测试"
	@echo -e "\tmake clean \t\t清除编译的中间文件"
	@echo -e "\tmake distclean \t\t清除所有编译结果"
	@echo -e "\tmake lib \t\t将所有obj整合到一个.a"
	@echo -e "\tmake release \t\t导出release库"
	@echo -e "\tmake tar \t\t打包release"
	@echo -e "\tmake install \t\t安装release库"
	@echo -e "\tmake uninstall \t\t卸载release库"
	@echo -e "\tmake help \t\t显示帮助信息"
	@echo -e ""
# -Wshadow 局部变量和全局变量同名
# -Wpedantic 严格的ISO C 标准警告
# -Wstrict-aliasing 警告可能会引发严格别名规则的别名问题
# -Wunnitialized 未初始化局部变量
# -Wnull-dereference 警告可能的空指针解引用
# -Wformat=2 字符串格式化
# -Wformat-security 警告可能会引发格式字符串安全问题
