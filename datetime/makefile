export CXX := g++-4

export ARC_ROOT := ${HOME}/work/arcadia
export WMC_ROOT := ${ARC_ROOT}/wmconsole

export POCO := ${WMC_ROOT}/contrib/poco-1.4-data
export POCO_INCL := ${POCO}/include

export STL_ROOT := ${ARC_ROOT}/util/private/stl/stlport-5.1.4

WARN_DISABLED := -Wno-deprecated -Wno-invalid-offsetof

TARGET := elapsed.exe
FILES := elapsed
C_FILES := $(addsuffix .cpp,$(FILES))
O_FILES := $(addsuffix .o,$(FILES))

.PHONY : all run
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