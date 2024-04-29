AM_SRCS := riscv/npc/start.S \
           riscv/npc/trm.c \
           riscv/npc/ioe.c \
           riscv/npc/timer.c \
           riscv/npc/input.c \
           riscv/npc/cte.c \
           riscv/npc/trap.S \
           platform/dummy/vme.c \
           platform/dummy/mpe.c

CFLAGS    += -fdata-sections -ffunction-sections
LDFLAGS   += -T $(AM_HOME)/scripts/linker.ld \
						 --defsym=_pmem_start=0x80000000 --defsym=_entry_offset=0x0
LDFLAGS   += --gc-sections -e _start
CFLAGS += -DMAINARGS=\"$(mainargs)\"
#NPCFLAGS +=$(shell dirname $(IMAGE).elf)/$(NAME)-$(ARCH).bin
### work in npc/Makefile
#NPCFLAGS +=-b 
#NPCFLAGS +=-l $(shell dirname $(IMAGE).elf)/npc-log.txt
#NPCFLAGS += -e $(shell dirname $(IMAGE).elf)/$(NAME)-$(ARCH).elf
#$(info "!!!!!!!!!!!!!!!! $(NPCFLAGS)")
.PHONY: $(AM_HOME)/am/src/riscv/npc/trm.c

image: $(IMAGE).elf
	@$(OBJDUMP) -d $(IMAGE).elf > $(IMAGE).txt
	@echo + OBJCOPY "->" $(IMAGE_REL).bin
	@$(OBJCOPY) -S --set-section-flags .bss=alloc,contents -O binary $(IMAGE).elf $(IMAGE).bin

#make -c指明跳转到目录下读取那里的makefile
#M=$(PWD) 表明然后返回到当前目录继续读入、执行当前Makefile
run:image
#	$(MAKE) -C $(NPC_HOME) ISA=$(ISA) run ARGS="$(NPCFLASGS)" IMG=$(IMAGE).bin
	$(MAKE) -C $(NPC_HOME) sim

#run:image
#	$(NPC_HOME)/build/Vnpc $(NPCFLAGS)
