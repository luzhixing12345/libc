
ifeq ($(strip $(V)),)
	ifeq ($(findstring s,$(filter-out --%,$(firstword $(MAKEFLAGS)))),)
		E = @echo
	else
		E = @\#
	endif
	Q = @
else
	E = @\#
	Q =
endif
export E Q


CC          := gcc
TARGET      := kemu
SRC_PATH    := src
SRC_EXT     := c
THIRD_LIB   :=

FIND	    := find
CSCOPE	    := cscope
TAGS	    := ctags
INSTALL     := install
CHECK       := check

# ------------------------- #
#          WARNING          #
# ------------------------- #
WARNING += -Wall
WARNING += -Wunused
WARNING += -Werror
WARNING += -Wformat-security
WARNING += -Wshadow
WARNING += -Wpedantic
WARNING += -Wstrict-aliasing
WARNING += -Wuninitialized
WARNING += -Wnull-dereference
WARNING += -Wformat=2
WARNING += -fno-strict-aliasing
WARNING += -Winit-self
WARNING += -Wmissing-declarations
WARNING += -Wmissing-prototypes
WARNING += -Wnested-externs
WARNING += -Wno-system-headers
WARNING += -Wold-style-definition
WARNING += -Wredundant-decls
WARNING += -Wsign-compare
WARNING += -Wstrict-prototypes
WARNING += -Wundef
WARNING += -Wvolatile-register-var
WARNING += -Wwrite-strings
WARNING += -Wno-format-nonliteral
CFLAGS	+= $(WARNINGS)

# ------------------------- #

# Translate uname -m into ARCH string
ARCH ?= $(shell uname -m | sed -e s/i.86/i386/ -e s/ppc.*/powerpc/ \
	  -e s/armv.*/arm/ -e s/aarch64.*/arm64/ -e s/mips64/mips/ \
	  -e s/riscv64/riscv/ -e s/riscv32/riscv/)

ifneq ($(THIRD_LIB),)
CFLAGS += $(shell pkg-config --cflags $(THIRD_LIB))
LDFLAGS += $(shell pkg-config --libs $(THIRD_LIB))
endif

TEST_PATH = test
TMP_PATH = tmp
RELEASE = $(TARGET)
LIB = lib$(TARGET).a
# ------------------------- #

rwildcard = $(foreach d, $(wildcard $1*), $(call rwildcard,$d/,$2) \
						$(filter $2, $d))

SRC = $(call rwildcard, $(SRC_PATH), %.$(SRC_EXT))
OBJS = $(SRC:$(SRC_EXT)=o)

comma = ,
# The dependency file for the current target
depfile = $(subst $(comma),_,$(dir $@).$(notdir $@).d)

DEPS	:= $(foreach obj,$(OBJS) $(OBJS_DYNOPT) $(OTHEROBJS) $(GUEST_OBJS),\
		$(subst $(comma),_,$(dir $(obj)).$(notdir $(obj)).d))

c_flags	= -Wp,-MD,$(depfile) -Wp,-MT,$@ $(CFLAGS)

ifeq ($(MAKECMDGOALS),debug)
CFLAGS+=-g
endif

PROGRAM = $(SRC_PATH)/$(TARGET)

all: $(PROGRAM)

debug: all

$(PROGRAM): $(OBJS)
	$(E) "  LINK    " $@
	$(Q) $(CC) $(CFLAGS) $(OBJS) $(OBJS_DYNOPT) $(OTHEROBJS) $(GUEST_OBJS) $(LDFLAGS) $(LIBS) $(LIBS_DYNOPT) $(LIBFDT_STATIC) -o $@

$(OBJS):
%.o: %.c
ifeq ($(C),1)
	$(E) "  CHECK   " $@
	$(Q) $(CHECK) -c $(CFLAGS) $(CFLAGS_DYNOPT) $< -o $@
endif
	$(E) "  CC      " $@
	$(Q) $(CC) -c $(c_flags) $(CFLAGS_DYNOPT) $< -o $@

# ------------------------- #
#          使用方法
# ------------------------- #
.PHONY: clean distclean lib release tar all test

install: all
	$(E) "  INSTALL"
	$(Q) $(INSTALL) -d -m 755 '$(DESTDIR_SQ)$(bindir_SQ)' 
	$(Q) $(INSTALL) $(PROGRAM) '$(DESTDIR_SQ)$(bindir_SQ)' 
.PHONY: install


clean:
	$(E) "  CLEAN"
	$(Q) rm -f $(DEPS) $(STATIC_DEPS) $(OBJS) $(OTHEROBJS) $(OBJS_DYNOPT) $(STATIC_OBJS) $(PROGRAM) $(PROGRAM_ALIAS) $(PROGRAM)-static $(GUEST_INIT) $(GUEST_PRE_INIT) $(GUEST_OBJS)
	$(Q) rm -f $(PROGRAM)
release:
	$(MAKE) -j4
	mkdir $(RELEASE)
	@cp $(EXE) $(RELEASE)/ 
	tar -cvf $(TARGET).tar $(RELEASE)/

help:
	$(E) ""
	$(E) "  [$(TARGET) compile help]"
	$(E) ""
	$(E) "    make              编译"
	$(E) "    make help         帮助信息"
	$(E) "    make clean        清除编译文件"
	$(E) "    make all          编译"
	$(E) "    make install      安装"
	$(E) "    make debug        调试模式"
	$(E) "    make release      打包"
	$(E) ""

ifneq ($(MAKECMDGOALS),clean)
-include $(DEPS)
-include $(STATIC_DEPS)
endif
