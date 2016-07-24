LOCAL_DIR := /home/dcashman/projects/cryptopals
LIBS_DIR := /home/dcashman/projects
SRC_DIR := $(LOCAL_DIR)/src
TEST_DIR := $(LOCAL_DIR)/test
TEST_SRC_DIR := $(TEST_DIR)/src
INC_DIRS := $(LOCAL_DIR)/include
OUT_DIR := $(LOCAL_DIR)/out
TEST_OUT_DIR := $(OUT_DIR)/tests

SRCS := \
    Analysis.cpp \
    BinaryBlob.cpp \
    main.cpp \
    matasano_set01.cpp \
    utils.c

HDRS := $(wildcard $(INC_DIR)/*.h)

TEST_SRCS := \
    sample_test.cpp \

# global target(s)
MATASANO := $(OUT_DIR)/matasano
TESTS =  $(TEST_OUT_DIR)/matasano_tests

# initial compiler options
CC  := clang
COMMON_CC_FLAGS := -c -Wall -Werror -pthread
CFLAGS := $(COMMON_CC_FLAGS)
CPPFLAGS := $(COMMON_CC_FLAGS) -std=c++11

# collection of libs to build against and rely upon
# TODO: add proper build support for this?
EXT_SHARED_LIBS := \
    $(LIBS_DIR)/openssl/libcrypto.so \

BUILT_STATIC_LIBS :=

# hackily deal with this for now, should come up system for external libs
INC_DIRS += \
    $(LIBS_DIR)/openssl/include \

INC_FLAGS :=  $(foreach d, $(INC_DIRS), -I$(d))

.Phony : all
all: $(MATASANO) $(TESTS)

# deal w/c code
c_srcs := $(filter %.c, $(SRCS))
c_objs := $(addprefix $(OUT_DIR)/,$(c_srcs:.c=.o))
$(c_objs): PRIVATE_CC := $(CC)
$(c_objs): PRIVATE_CFLAGS := $(CFLAGS) $(INC_FLAGS)
$(c_objs): $(OUT_DIR)/%.o: $(SRC_DIR)/%.c $(HDRS)
	mkdir -p $(dir $@)
	$(PRIVATE_CC) $(PRIVATE_CFLAGS) -o $@ $<


cpp_srcs := $(filter %.cpp, $(SRCS))
cpp_objs := $(addprefix $(OUT_DIR)/,$(cpp_srcs:.cpp=.o))
$(cpp_objs): PRIVATE_CC := $(CC)
$(cpp_objs): PRIVATE_CPPFLAGS := $(CPPFLAGS) $(INC_FLAGS)
$(cpp_objs): $(OUT_DIR)/%.o: $(SRC_DIR)/%.cpp $(HDRS)
	mkdir -p $(dir $@)
	$(PRIVATE_CC) $(PRIVATE_CPPFLAGS) -o $@ $<

all_objs := $(c_objs) $(cpp_objs)

$(MATASANO) : PRIVATE_LD := $(CC)
$(MATASANO) : PRIVATE_LD_LIBS := -lstdc++ -lm
$(MATASANO) : $(all_objs) $(EXT_SHARED_LIBS)
	mkdir -p $(dir $@)
	$(PRIVATE_LD) -o $@ $^ $(PRIVATE_LD_LIBS)


################################################################################
# add gtest support
# built from sample makefile at $(GTEST_DIR)/make/Makefile
GTEST_PATH := googletest/googletest
GTEST_DIR := $(LIBS_DIR)/$(GTEST_PATH)/
CPPFLAGS += -isystem $(GTEST_DIR)/include
GTEST_HEADERS = $(GTEST_DIR)/include/gtest/*.h \
                $(GTEST_DIR)/include/gtest/internal/*.h

GTEST_SRCS_ = $(GTEST_DIR)/src/*.cc $(GTEST_DIR)/src/*.h $(GTEST_HEADERS)
# TODO: consolidate into fewer rules (like c_objs) below
$(TEST_OUT_DIR)/gtest-all.o : $(GTEST_SRCS_)
	mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) -I$(GTEST_DIR) $(CXXFLAGS) -c \
            $(GTEST_DIR)/src/gtest-all.cc -o $@

$(TEST_OUT_DIR)/gtest_main.o : $(GTEST_SRCS_)
	mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) -I$(GTEST_DIR) $(CXXFLAGS) -c \
            $(GTEST_DIR)/src/gtest_main.cc -o $@

$(TEST_OUT_DIR)/gtest.a : $(TEST_OUT_DIR)/gtest-all.o
	mkdir -p $(dir $@)
	$(AR) $(ARFLAGS) $@ $^

$(TEST_OUT_DIR)/gtest_main.a : $(TEST_OUT_DIR)/gtest-all.o $(TEST_OUT_DIR)/gtest_main.o
	mkdir -p $(dir $@)
	$(AR) $(ARFLAGS) $@ $^

BUILT_STATIC_LIBS += \
    $(TEST_OUT_DIR)/gtest.a \
    $(TEST_OUT_DIR)/gtest_main.a \

INC_DIRS += \
    $(LIBS_DIR)/$(GTEST_PATH)/include \
    $(TEST_DIR)/include \

INC_FLAGS :=  $(foreach d, $(INC_DIRS), -I$(d))
HDRS += $(wildcard $(TEST_DIR)/include/*.h)

cpp_test_srcs := $(addprefix $(TEST_SRC_DIR)/,$(TEST_SRCS:.cpp=.o))
cpp_test_objs := $(addprefix $(TEST_OUT_DIR)/,$(not-dir $(TEST_SRCS:.cpp=.o)))
$(cpp_test_objs): PRIVATE_CC := $(CC)
$(cpp_test_objs): PRIVATE_CPPFLAGS := $(CPPFLAGS) $(INC_FLAGS)
$(cpp_test_objs): $(TEST_OUT_DIR)/%.o: $(TEST_SRC_DIR)/%.cpp $(HDRS)
	mkdir -p $(dir $@)
	$(PRIVATE_CC) $(PRIVATE_CPPFLAGS) -o $@ $<

$(TESTS) : PRIVATE_LD := $(CC)
$(TESTS) : PRIVATE_LD_LIBS := -lstdc++ -lm  -lpthread
$(TESTS) : PRIVATE_STATIC_LIBS := $(BUILT_STATIC_LIBS)
$(TESTS) : $(cpp_test_objs) $(EXT_SHARED_LIBS) $(BUILT_STATIC_LIBS)
	mkdir -p $(dir $@)
	$(PRIVATE_LD) -o $@ $^ $(PRIVATE_LD_LIBS) $(PRIVATE_STATIC_LIBS)
# end gtest support section 
################################################################################

.Phony : clean
clean :
	rm -r $(OUT_DIR)/*
