####### Compiler, tools and options

# CXX           = g++  #must be already installed


# ifeq ($(CXX),g++)
ifneq (,$(findstring g++,$(CXX)))
OMPFLAG		= -fopenmp
endif


# ifeq ($(CXX),icc)
ifneq (,$(findstring icc,$(CXX)))
OMPFLAG		= -openmp
endif

CFLAGS        = -pipe -Wall -W -D_REENTRANT $(DEFINES)
CXXFLAGS      = -msse -msse2 -msse3 -mssse3 -O3 -Wno-long-long -Iopenlab_mod -fno-threadsafe-statics -lrt  -march=native
CXXFLAGSSIMPL = -msse -msse2 -O2 -Iopenlab_mod -march=native
LINK          = $(CXX)
LFLAGS        = -Wl,--as-needed
LIBS          = -L/usr/lib64
TBB_LIBS      = -ltbb -L/usr/lib -I/usr/include -lrt
AR            = ar cqs
RANLIB        = 
TAR           = tar -cf
COMPRESS      = gzip -9f
COPY          = cp -f
SED           = sed
COPY_FILE     = $(COPY)
COPY_DIR      = $(COPY) -rk
INSTALL_FILE  = install -m 644 -p
INSTALL_DIR   = $(COPY_DIR)
INSTALL_PROGRAM = install -m 755 -p
DEL_FILE      = rm -f
SYMLINK       = ln -sf
DEL_DIR       = rmdir
MOVE          = mv -f
CHK_DIR_EXISTS= test -d
MKDIR         = mkdir -p

####### Output directory

OBJECTS_DIR   = ./

####### Files

SOURCES       = Fit.cxx Fit.h FitClasses.h
TBB_SOURCES		= openlab_mod/multithread.h openlab_mod/parallel_for_simple.h openlab_mod/parallel_for_simpleHT.h

BINARIES      = single singleVc tbb tbbVc omp ompVc pseudo x87 double
OUTPUT				= data/fit.dat data/fitdiff.dat data/time.dat QualityHisto/*.root
DESTDIR       = 


default: singleVc
	@echo "use $(CXX)"

####### Implicit rules

.SUFFIXES: .o .c .cpp .cc .cxx .C

# old options
single: openlab_mod/P4_F32vec4.h $(SOURCES)
	$(CXX) -DSIMPLSIMD $(CXXFLAGSSIMPL) $(LIBS) -o "$@" Fit.cxx 

omp: openlab_mod/P4_F32vec4.h $(SOURCES)
	$(CXX) $(OMPFLAG) -DSIMPLSIMD -DOMP $(CXXFLAGSSIMPL) $(LIBS) -o "$@" Fit.cxx

tbb: $(TBB_SOURCES) $(SOURCES)
	$(CXX) -DTBB -DSIMPLSIMD $(CXXFLAGSSIMPL) $(LIBS) $(TBB_LIBS) -o "$@" Fit.cxx

pseudo: openlab_mod/PSEUDO_F32vec4.h $(SOURCES)
	$(CXX) -DNOTSIMD $(CXXFLAGSSIMPL) $(LIBS) -o "$@" Fit.cxx


singleVc: $(SOURCES)
	$(CXX) $(CXXFLAGS) $(LIBS) -o "$@" Fit.cxx  -lVc 

ompVc: openlab_mod/P4_F32vec4.h $(SOURCES)
	$(CXX) $(OMPFLAG) -DOMP $(CXXFLAGS) $(LIBS) -o "$@" Fit.cxx -lVc

tbbVc: $(TBB_SOURCES) $(SOURCES)
	$(CXX) -DTBB $(CXXFLAGS) $(LIBS) $(TBB_LIBS) -o "$@" Fit.cxx


x87: $(SOURCES)
	$(CXX) -DX87 $(CXXFLAGS) $(LIBS) -o "$@" Fit.cxx

double: openlab_mod/P4_F64vec2.h $(SOURCES)
	$(CXX) -DDOUBLE_PRECISION $(CXXFLAGS) $(LIBS) -o "$@" Fit.cxx

valarray: openlab_mod/ValVec.h $(SOURCES)
	$(CXX) -DVALARRAY $(CXXFLAGS) $(LIBS) -o "$@" Fit.cxx

double_debug: openlab_mod/P4_F64vec2.h Fit.cxx FitClasses.h Fit.h
	$(CXX) -DDOUBLE_PRECISION -g -o "$@" Fit.cxx

debug: openlab_mod/P4_F64vec2.h Fit.cxx FitClasses.h Fit.h
	$(CXX) -g -o "$@" Fit.cxx

.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.cc.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.cxx.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.C.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.c.o:
	$(CC) -c $(CFLAGS) $(INCPATH) -o "$@" "$<"

all: $(BINARIES)

.tar.bz2: clean
	tar cj --exclude=hltsse_openlab"$@" --exclude=.*.swp -C .. hltsse > hltsse_openlab"$@"

clean:
	-$(DEL_FILE) $(BINARIES) $(OUTPUT)

info:
	@echo "use $(CXX)"
####### Sub-libraries

distclean: clean
	-$(DEL_FILE) $(TARGET) 
	-$(DEL_FILE) Makefile



compiler_rcc_make_all:
compiler_rcc_clean:
compiler_image_collection_make_all: qmake_image_collection.cpp
compiler_yacc_decl_make_all:
compiler_yacc_decl_clean:
compiler_yacc_impl_make_all:
compiler_yacc_impl_clean:
compiler_lex_make_all:
compiler_lex_clean:
compiler_clean: compiler_moc_header_clean compiler_uic_clean 



install:  install_target install_sources  FORCE

uninstall: uninstall_target uninstall_sources   FORCE

FORCE:

