export CXX := g++-4

TARGET := mmap.exe
FILES := mmap
C_FILES := $(addsuffix .cpp,$(FILES))
O_FILES := $(addsuffix .o,$(FILES))

.PHONY : all
#all: WMCGetApplication.o
all: ${TARGET}


%.o: %.cpp
	${CXX} -c $^ -o $@ -I../

${TARGET}: ${O_FILES}
	${CXX} -o $@ ${O_FILES}

run: ${TARGET}
	./${TARGET}

clean:
	rm ${O_FILES} ${TARGET}