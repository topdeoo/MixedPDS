add_rules("mode.debug", "mode.release")

-- Packages
add_requires("unordered_dense")
add_requires("fmt")
add_requires("boost", { configs =  { all = true } })

add_includedirs("/home/hsl/lingd/packages/gurobi1103/linux64/include/")

target("solver.elf")
    set_rundir("$(projectdir)")
    set_languages("cxx17")
    set_kind("binary")
    -- CXX Flags
    set_warnings("all", "error")
    -- set_optimize("fastest")
    if is_mode("release") then
        set_optimize("fastest") 
    end
    add_includedirs("include")
    add_files("src/*.cc|checker.cc")
    add_packages("unordered_dense")
    add_packages("fmt")
    add_linkdirs("/home/hsl/lingd/packages/gurobi1103/linux64/lib/")
    add_ldflags("-lboost_program_options", "-lgurobi_c++", "-lgurobi110", { force = true })

target("checker.elf")
    set_rundir("$(projectdir)")
    set_languages("cxx17")
    set_kind("binary")
    -- CXX Flags
    set_warnings("all", "error")
    -- set_optimize("fastest")
    add_includedirs("include")
    add_files("src/*.cc|main.cc")
    add_packages("unordered_dense")
    -- add_packages("gurobi")
    add_packages("fmt")
    add_linkdirs("/home/hsl/lingd/packages/gurobi1103/linux64/lib/")
    add_ldflags("-lboost_program_options", "-lgurobi_c++", "-lgurobi110", { force = true })
