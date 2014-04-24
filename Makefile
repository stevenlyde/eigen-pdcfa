CC=gcc
CPP=g++
NVCC=/usr/local/cuda/bin/nvcc

OMP=-fopenmp
OPT=-O3
DEBUG=-g -Wall -Wno-reorder
CUDA_DEBUG=-g -G
CUDA_CC_FLAGS=--generate-code arch=compute_20,code=sm_20 -Xcompiler -rdynamic -Xcompiler -fopenmp
LD_LIBS=-lcuda -lcudart -lcublas -lrt -llapack 
#LIB_INCLUDE=-L /usr/local/cuda/lib64
CUDA_INCLUDE=-I ./include -I ./ -I ./cusp -I ./thrust -I /usr/local/cuda/include -I /usr/local/cuda/SDK/C/common/inc/
PROJECT=CFA

#default to debug builds for now
OPT_FLAGS=$(OPT)
NVCC_OPT_FLAGS=$(OPT)

all: $(PROJECT)

profile: 
	$(MAKE) DEFINES=-DPROFILE

verbose: 
	$(MAKE) DEFINES=-DDEBUG \
	CUDA_CC_FLAGS=#(CUDA_CC_FLAGS) -Xptxas-v --ptxas-options=-v

opt:
	$(MAKE) OPT_FLAGS="$(OPT)" NVCC_OPT_FLAGS="$(OPT)"

debug:
	$(MAKE) OPT_FLAGS="$(DEBUG)" NVCC_OPT_FLAGS="$(CUDA_DEBUG)"

SRCS=$(wildcard src/*.cu) $(wildcard src/*/*.cu) $(wildcard src/*/*/*.cu) $(wildcard include/*.cu)
OBJS=$(SRCS:.cu=.o)
INCLUDE_FILES=$(wildcard include/*.h) $(wildcard include/*/*.h) $(wildcard include/*/*/*.h) $(wildcard src/include/*.h) $(wildcard include/*.inl)
CUDA_INCLUDE_FILES=$(wildcard include/*.cuh) $(wildcard include/*.cinl)
CPP_FILES=$(wildcard src/*.cpp)
CPP_OBJS=$(CPP_FILES:.cpp=.o)

%.o : %.cu $(INCLUDE_FILES) $(CUDA_INCLUDE_FILES)
	$(NVCC) ${CUDA_INCLUDE} ${NVCC_OPT_FLAGS} ${CXX_FLAGS} ${CUDA_CC_FLAGS} -c $< -o $@ 

%.o : %.cpp $(INCLUDE_FILES)
	$(CPP) ${CUDA_INCLUDE} ${OPT_FLAGS} $(OMP) -lpthread -c $< -o $@

lib/$(PROJECT).a: lib $(OBJS)
	ar ruv lib/$(PROJECT).a $(OBJS)
	LIB_PP=TRUE

lib:
	mkdir lib

.PHONY clean:
	rm -f src/*.o
	rm -f lib/$(PROJECT).a
	rm -f $(PROJECT)

$(PROJECT): $(CPP_FILES) $(CPP_OBJS) lib/$(PROJECT).a
	$(NVCC) -o $(PROJECT) ${NVCC_OPT_FLAGS} ${CUDA_CC_FLAGS} ${LD_LIBS} $(CPP_OBJS) lib/$(PROJECT).a
#	$(CPP) -o $(PROJECT) ${OPT_FLAGS} $(OMP) $(LIB_INCLUDE) ${LD_LIBS} $(CPP_OBJS)
