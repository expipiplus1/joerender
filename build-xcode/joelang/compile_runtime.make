.SUFFIXES: 
all: \
	$(TARGET_BUILD_DIR)/runtime.bc

$(TARGET_BUILD_DIR)/runtime.bc: \
float.cpp.bc\
string.cpp.bc
	/usr/local/bin/llvm-link -o $(TARGET_BUILD_DIR)/runtime.bc $(TARGET_TEMP_DIR)/float.cpp.bc $(TARGET_TEMP_DIR)/string.cpp.bc

float.cpp.bc: \
$(SRCROOT)/runtime/float.cpp\
$(SRCROOT)/runtime/float.hpp\
$(SRCROOT)/runtime/types.hpp
	clang++ -std=c++11 -stdlib=libc++ -O4 -msse4.1 -Wno-return-type-c-linkage -I$(SRCROOT)/../joemath/include -c -emit-llvm $(SRCROOT)/runtime/float.cpp -o $(TARGET_TEMP_DIR)/float.cpp.bc

string.cpp.bc: \
$(SRCROOT)/runtime/string.cpp\
$(SRCROOT)/runtime/string.hpp\
$(SRCROOT)/runtime/types.hpp
	clang++ -std=c++11 -stdlib=libc++ -O4 -msse4.1 -Wno-return-type-c-linkage -I$(SRCROOT)/../joemath/include -c -emit-llvm $(SRCROOT)/runtime/string.cpp -o $(TARGET_TEMP_DIR)/string.cpp.bc
