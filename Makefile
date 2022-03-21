OBJS      = mmc.o
TEST_TARGET = mmc_test
TEST_OBJS = mmc_test.o
CXX    = g++
PREFIX = /usr/local
INSTALL_DIR = $(PREFIX)/include/mmc
BUILD_DIR = ./build
IS_STATIC_LIB = false
LIB_DST = /usr/lib/

ifeq ($(IS_STATIC_LIB),true)
CMAKE_OPS = -DSTATIC_LIBS=ON
LIB = $(BUILD_DIR)/libmmc.a
else
LIB = $(BUILD_DIR)/libmmc.so
endif

install:
	$(shell sed -i -e '2s/^#.*\sIS_DEBUG*/#undef IS_DEBUG/' mmc.h )
	mkdir -p $(INSTALL_DIR)
	cp mmc.hpp $(INSTALL_DIR)
	mkdir -p build
	cmake -S . -B build $(CMAKE_OPS)
	cmake --build build
	cp $(LIB) $(LIB_DST)
	

test: $(TEST_TARGET)

$(TEST_TARGET): $(OBJS) $(TEST_OBJS)
	$(shell sed -i -e '2s/^#.*\sIS_DEBUG*/#define IS_DEBUG/' mmc.h )
	$(CXX) -o $@ $^
	./$(TEST_TARGET)

%.o: %.cc
	$(CXX) -c -o $@ $^

clean:;
	rm -f $(TARGET) $(OBJS) $(TEST) $(TEST).o
	rm -rf build

clean_all:;
	make clean
	rm -f $(INSTALL_DIR)/mmc.hpp
	rm -f $(LIB_DST)/libmmc*
