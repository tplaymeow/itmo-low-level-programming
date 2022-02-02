ASM=nasm
ASMFLAGS=-f elf64
LD=ld

OUTPUT_NAME=dict_program
OUT_DIR=out

ASMS := $(wildcard *.asm)
OBJS := $(ASMS:%.asm=$(OUT_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

all: build

# include deps for .o files

-include $(DEPS)


# Create output directory

$(OUT_DIR):
	mkdir -p $(OUT_DIR)


# Create .o files from .asm files

$(OUT_DIR)/%.o: %.asm | $(OUT_DIR)
	$(ASM) $(ASMFLAGS) -o $@ -MD $(patsubst %.o,%.d,$@) $<


# Create executable file 

$(OUT_DIR)/$(OUTPUT_NAME): $(OBJS)
	$(LD) -o $(OUT_DIR)/$(OUTPUT_NAME) $(OBJS)


build: $(OUT_DIR)/$(OUTPUT_NAME)

run: build
	$(OUT_DIR)/$(OUTPUT_NAME)

clean:
	rm -r $(OUT_DIR)

.PHONY: build run clean
