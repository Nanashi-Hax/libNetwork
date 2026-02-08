# Tools.mk for windows

# cpp2o
# $1: input file
# $2: output file
# $3: dependence file
# $4: flags
define cpp2o
    @mkdir -p $(dir $2)
    @mkdir -p $(dir $3)
    @$(CppCompiler) -c $1 -o $2 -MMD -MP -MF $3 $4
endef

# o2exe
# $1: input files
# $2: output file
# $3: linker flags
# $4: library directory flags
# $5: library link flags
# $6: map file
define o2exe
    @mkdir -p $(dir $2)
    @$(Linker) $1 -o $2 $3 $4 $5 -Wl,-Map,$6
endef

# o2a
# $1: input files
# $2: output file
define o2a
    @mkdir -p $(dir $2)
	@$(Archive) -rcs $2 $1
endef

# abs2rel
# $1: input absolute pathes
# $2: base directory
define abs2rel
    $(if $(strip $1),$(shell realpath --relative-to=$2 $1))
endef