# Makefile
# for windows library

.SUFFIXES:
.SECONDARY:
.PHONY: all clean install

#-------------------------------------------------------------------------------
# ToolChains
#-------------------------------------------------------------------------------
CCompiler := gcc
CppCompiler := g++
Linker := g++
NameList := nm
Archive := ar

#-------------------------------------------------------------------------------
# Environment
#-------------------------------------------------------------------------------
Msys := /d/msys64/
MinGW := $(Msys)/mingw64
User := $(MinGW)/User

#-------------------------------------------------------------------------------
# Directories
#-------------------------------------------------------------------------------
TopDir := $(CURDIR)

Target := libNetwork

SourceDir := $(TopDir)/Source
IncludeDir := $(TopDir)/Include $(TopDir)/Public
PublicDir := $(TopDir)/Public
BuildDir := $(TopDir)/Build
DestDir := $(TopDir)/Dest

#-------------------------------------------------------------------------------
# Macros
#-------------------------------------------------------------------------------
include $(TopDir)/Tools.mk

#-------------------------------------------------------------------------------
# Files
#-------------------------------------------------------------------------------
BuildObjectDir := $(BuildDir)/Object
BuildDependenceDir := $(BuildDir)/Dependence

CppFile := $(shell find $(SourceDir) -type f -name '*.cpp')
CppRelative := $(call abs2rel,$(CppFile),$(SourceDir))
BuildObjectCppFile := $(patsubst %.cpp,$(BuildObjectDir)/Cpp/%.o,$(CppRelative))

DestLibraryFile := $(DestDir)/$(Target).a
InstallLibDir := $(User)/Lib
InstallIncDir := $(User)/Include

#-------------------------------------------------------------------------------
# Libraries
#-------------------------------------------------------------------------------
LibraryEntries := 
LibraryDirs := $(User)/Lib
LibraryIncludeDirs := $(User)/Include
LibraryDirFlags := $(foreach dir,$(LibraryDirs),-L$(dir))
LibraryFlags := $(foreach entry,$(LibraryEntries),-l$(entry))

#-------------------------------------------------------------------------------
# Includes
#-------------------------------------------------------------------------------
IncludeDirs := $(IncludeDir) $(BuildIncludeDir) $(LibraryIncludeDirs)
IncludeFlags := $(foreach dir,$(IncludeDirs),-I$(dir))

#-------------------------------------------------------------------------------
# Cpp Flags
#-------------------------------------------------------------------------------
CppFlags := $(IncludeFlags) $(LibraryFlags) -Wall -O3 -ffunction-sections -std=c++23

#-------------------------------------------------------------------------------
# Linker Flags
#-------------------------------------------------------------------------------
LinkerScript := ws2_32
Specs :=
LinkerFlags := $(LinkerScript) $(Specs) -g

#-------------------------------------------------------------------------------
# Rules
#-------------------------------------------------------------------------------
all: $(DestLibraryFile)

$(BuildObjectDir)/Cpp/%.o: $(SourceDir)/%.cpp
	@echo $(notdir $<)
	$(call cpp2o,$<,$@,$(BuildDependenceDir)/$*.d,$(CppFlags))

$(DestDir)/%.a: $(BuildObjectCppFile)
	@echo linking ... $(notdir $@)
	$(call o2a,$^,$@)

-include $(BuildDependenceDir)/*.d

clean:
	@echo clean ...
	@rm -rf $(BuildDir) $(DestDir)

install: $(DestLibraryFile)
	@echo installing ... $(notdir $<)
	@mkdir -p $(InstallLibDir)
	@mkdir -p $(InstallIncDir)
	@cp $< $(InstallLibDir)/
	@cp -r $(PublicDir)/* $(InstallIncDir)/