DYNLINK_NVSDK_FLAG := y
OUTPUT_DYNLIB_FLAG := n
#DEBUG_MODE := y
DEBUG_MODE := n


src := ./NvDec.cpp 		\
	   ./nvcodec.cpp 	\
	   ./bufex_nv.cpp 	\
	   ./NvSdk.cpp 		\
	   ./PicQueue.cpp 	\
	   ./common.cpp 	\
	   ./test.cpp		\
	   ./DemuxFile.cpp
#	   ./colorspace.cpp

ifeq ($(DYNLINK_NVSDK_FLAG), y)
src += ../common/src/dynlink_cuda.cpp \
	   ../common/src/dynlink_nvcuvid.cpp
endif

#CC := nvcc
CC := g++

#C_FLAGS := -w -DHAVE_NVCUVID -std=c++11 -Xcompiler -fPIC
C_FLAGS := -DHAVE_NVCUVID  -fPIC -fvisibility=hidden -std=c++11

ifeq ($(DEBUG_MODE), y)
C_FLAGS += -g -O0
else
C_FLAGS += -O3
endif

CXX_FLAGS := -I./ \
			 -I../include \
			 -I../../sncodec/include \
			 -I../../sncodec/src

ifneq ($(DYNLINK_NVSDK_FLAG), y)
CXX_FLAGS += -I/usr/local/cuda/include \
			 -L/usr/local/cuda/lib64 -lcuda -lcudart \
			 -L/usr/lib/x86_64-linux-gun -lnvcuvid
else
CXX_FLAGS += -I../common/inc 
endif

LD_LIBRARIES := -lpthread 

ifneq ($(DYNLINK_NVSDK_FLAG), y)
CXX_FLAGS += -L/usr/local/cuda/lib64 -lcuda -lcudart \
			 -L/usr/lib/x86_64-linux-gun -lnvcuvid
else
CXX_FLAGS += -ldl
endif


ifeq ($(OUTPUT_DYNLIB_FLAG), y)
target := libcuvidcodec.so 

all:
	@echo $(CC) $(C_FLAGS) $(CXX_FLAGS) $(src) -shared -o $(target) $(LD_LIBRARIES)
	@$(CC) $(C_FLAGS) $(CXX_FLAGS) $(src) -shared -o $(target) $(LD_LIBRARIES)
else
#target := libnvcodec.a 
obj := ${patsubst %.cpp, %.o, $(src)}	

all:$(obj)
	#@ar -r $(target) $(obj) 
	#@ranlib $(target)
	g++ -o test $(obj) -ldl -lpthread
	
$(obj):%.o : %.cpp
	@echo $(CC) $(C_FLAGS) $(CXX_FLAGS) -c $^ -o $@ $(LD_LIBRARIES)
	@$(CC) $(C_FLAGS) $(CXX_FLAGS) -c $^ -o $@ $(LD_LIBRARIES)

endif


clean:
	rm -rf $(target) *.o
