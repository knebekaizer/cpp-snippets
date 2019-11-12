export CXX := g++-4

export BOOST_ROOT := "/f/dev/boost_1_39_0"
export BOOST_INC := ${BOOST_ROOT}
export BOOST_LIB := ${BOOST_ROOT}/stage/lib


export ARC_ROOT := ${HOME}/work/arcadia
export WMC_ROOT := ${ARC_ROOT}/wmconsole

export POCO := ${WMC_ROOT}/contrib/poco-1.4-data
export POCO_INCL := ${POCO}/include

export STL_ROOT := ${ARC_ROOT}/util/private/stl/stlport-5.1.4

WARN_DISABLED := -Wno-deprecated -Wno-invalid-offsetof

#FILES := first
FILES := multiple_sources
C_FILES := $(addsuffix .cpp,$(FILES))
O_FILES := $(addsuffix .o,$(FILES))

.PHONY : all
#all: WMCGetApplication.o
all: test

test: ${O_FILES} 
	g++-4  $^ -o $@ -static -I${BOOST_ROOT} -L${BOOST_LIB} -lboost_program_options-gcc43-mt
#	${CXX} $^ -o $@ ${WARN_DISABLED} -I${ARC_ROOT} -I${POCO_INCL} -I${STL_ROOT}

%.o: %.cpp
	${CXX} -MMD -c $^ -o $@ -I${BOOST_ROOT}

clean:
	rm ${O_FILES}