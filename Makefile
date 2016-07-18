LOCAL_DIR := /home/dcashman/projects/cryptopals
LIBS_DIR := /home/dcashman/projects
INC_DIRS := $(LOCAL_DIR)/include
OUT_DIR := $(LOCAL_DIR)/out
SRC_DIR := $(LOCAL_DIR)/src

SRCS := \
    Analysis.cpp \
    BinaryBlob.cpp \
    main.cpp \
    matasano_set01.cpp \
    utils.c

# collection of libs to build against and rely upon
EXT_SHARED_LIBS := \
    $(LIBS_DIR)/openssl/libcrypto.so

# hackily deal with this for now, should come up system for external libs
INC_DIRS := \
    $(INC_DIRS) \
    $(LIBS_DIR)/openssl/include \

CC  := clang
COMMON_CC_FLAGS := -c -Wall -Werror $(foreach d, $(INC_DIRS), -I$(d))

# global target(s)
MATASANO := $(OUT_DIR)/matasano
.Phony : all
all: $(MATASANO)

# deal w/c code
CFLAGS := $(COMMON_CC_FLAGS)
c_srcs := $(filter %.c, $(SRCS))
c_objs := $(addprefix $(OUT_DIR)/,$(c_srcs:.c=.o))
$(c_objs): PRIVATE_CC := $(CC)
$(c_objs): PRIVATE_CFLAGS := $(CFLAGS)
$(c_objs): $(OUT_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(dir $@)
	$(PRIVATE_CC) $(PRIVATE_CFLAGS) -o $@ $<

CPPFLAGS := $(COMMON_CC_FLAGS) -std=c++11
cpp_srcs := $(filter %.cpp, $(SRCS))
cpp_objs := $(addprefix $(OUT_DIR)/,$(cpp_srcs:.cpp=.o))
$(cpp_objs): PRIVATE_CC := $(CC)
$(cpp_objs): PRIVATE_CPPFLAGS := $(CPPFLAGS)
$(cpp_objs): $(OUT_DIR)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(dir $@)
	$(PRIVATE_CC) $(PRIVATE_CPPFLAGS) -o $@ $<

all_objs := $(c_objs) $(cpp_objs)

$(MATASANO) : PRIVATE_LD := $(CC)
$(MATASANO) : PRIVATE_LD_LIBS := -lstdc++ -lm
$(MATASANO) : $(all_objs) $(EXT_SHARED_LIBS)
	mkdir -p $(dir $@)
	$(PRIVATE_LD) -o $@ $^ $(PRIVATE_LD_LIBS)

.Phony : clean
clean :
	rm -r $(OUT_DIR)/*
