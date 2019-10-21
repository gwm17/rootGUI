CC=g++
SRCDIR= ./src
OBJDIR= ./objs
INCLDIR= ./include
CPPFLAGS=-I $(INCLDIR) -I ./
CFLAGS=-std=c++11 -c -g -Wall `root-config --cflags --glibs`
LDFLAGS=`root-config --cflags --glibs`
SOURCES=$(wildcard $(SRCDIR)/*.cpp)
OBJS=$(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)
DICT_INCL=$(INCLDIR)/RootView.h $(INCLDIR)/FileSrcFrame.h $(INCLDIR)/FileConfigFrame.h $(INCLDIR)/OlineSrcFrame.h $(INCLDIR)/DivisionFrame.h $(INCLDIR)/SpectraFrame.h $(INCLDIR)/CutFrame.h $(INCLDIR)/CombineCutFrame.h $(INCLDIR)/SpectraListFrame.h $(INCLDIR)/CutListFrame.h $(INCLDIR)/ApplyCutFrame.h $(INCLDIR)/LinkDef_RV.h
DICT=$(SRCDIR)/RV_dict.cxx
LIB= $(OBJDIR)/RV_dict.o
EXE=RootView

.PHONY: clean all

all: $(EXE)

$(EXE): $(LIB) $(OBJS)
	$(CC) $(LDFLAGS) $^ -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CC) $(CPPFLAGS) $(CFLAGS) $< -o $@

$(LIB): $(DICT)
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ -c $^
	mv $(SRCDIR)/*.pcm ./

$(DICT): $(DICT_INCL)
	rootcling -f $@ -c $^

clean:
	$(RM) $(EXE) $(OBJS) $(LIB) $(DICT) *.pcm
