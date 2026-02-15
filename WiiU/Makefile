# Makefile
# for wiiu library

.SUFFIXES:
.SECONDARY:
.PHONY: all clean install

#-------------------------------------------------------------------------------
# ToolChains
#-------------------------------------------------------------------------------
CCompiler := powerpc-eabi-gcc
CppCompiler := powerpc-eabi-g++
Linker := powerpc-eabi-g++
NameList := powerpc-eabi-nm
Archive := powerpc-eabi-gcc-ar

#-------------------------------------------------------------------------------
# Environment
#-------------------------------------------------------------------------------
DevKitPro := $(DEVKITPRO)
PortLibs := $(DevKitPro)/portlibs/ppc
Wups := $(DevKitPro)/wups
Wut := $(DevKitPro)/wut
Wums := $(DevKitPro)/wums
User := $(DevKitPro)/User

MachineDependent := -DESPRESSO -mcpu=750 -meabi -mhard-float

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
LibraryEntries := wups wut notifications mappedmemory kernel
LibraryDirs := $(PortLibs)/lib $(Wups)/lib $(Wut)/lib $(Wums)/lib
LibraryIncludeDirs := $(PortLibs)/include $(Wups)/include $(Wut)/include $(Wums)/include
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
LinkerScript := -T$(Wums)/share/libmappedmemory.ld -T$(Wums)/share/libkernel.ld -T$(Wups)/share/wups.ld
Specs := -specs=$(Wut)/share/wut.specs -specs=$(Wups)/share/wups.specs
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