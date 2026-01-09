# Makefile
# for wiiu library

.SUFFIXES:
.SECONDARY:
.PHONY: all clean send

#-------------------------------------------------------------------------------
# ToolChains
#-------------------------------------------------------------------------------
CCompiler := gcc
CppCompiler := g++
Linker := g++
NameList := nm
Archive := gcc-ar

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
DistDir := $(TopDir)/Dist

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
CppRelative := $(shell realpath --relative-to=$(SourceDir) $(CppFile))
BuildObjectCppFile := $(patsubst %.cpp,$(BuildObjectDir)/Cpp/%.o,$(CppRelative))

DistLibraryFile := $(DistDir)/$(Target).a
InstallLibDir := $(User)/Lib
InstallIncDir := $(User)/Include

#-------------------------------------------------------------------------------
# Libraries
#-------------------------------------------------------------------------------
LibraryEntries := ws2_32
LibraryDirs := 
LibraryIncludeDirs := 
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
CppFlags := $(MachineDependent) $(IncludeFlags) $(LibraryFlags) -Wall -O3 -ffunction-sections -std=c++23

#-------------------------------------------------------------------------------
# Linker Flags
#-------------------------------------------------------------------------------
LinkerScript := 
Specs := 
LinkerFlags := $(LinkerScript) $(Specs) -g

#-------------------------------------------------------------------------------
# Rules
#-------------------------------------------------------------------------------
all: $(DistLibraryFile)

$(BuildObjectDir)/Cpp/%.o: $(SourceDir)/%.cpp
	@echo $(notdir $<)
	$(call cpp2o,$<,$@,$(BuildDependenceDir)/$*.d,$(CppFlags))

$(DistDir)/%.a: $(BuildObjectCppFile)
	@echo linking ... $(notdir $@)
	$(call o2a,$^,$@)

-include $(BuildDependenceDir)/*.d

clean:
	@echo clean ...
	@rm -rf $(BuildDir) $(DistDir)

install: $(DistLibraryFile)
	@echo installing ... $(notdir $<)
	@mkdir -p $(InstallLibDir)
	@mkdir -p $(InstallIncDir)
	@cp $< $(InstallLibDir)/
	@cp -r $(PublicDir)/* $(InstallIncDir)/