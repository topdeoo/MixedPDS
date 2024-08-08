add_rules("mode.debug", "mode.release")

-- Packages
add_requires("unordered_dense")
add_requires("gurobi", { system = true })
add_requires("fmt", { system = true })
add_requires("pacman::boost", { configs =  { all = true } })

target("solver.elf")
    set_rundir("$(projectdir)")
    set_languages("cxx17")
    set_kind("binary")
    -- CXX Flags
    set_warnings("all", "error")
    if is_mode("debug") then 
        add_cxxflags("-fstandalone-debug")
        add_cxflags("-fsanitize=address")
        add_ldflags("-fsanitize=address")
    end 
    
    if is_mode("release") then
        set_optimize("fastest") 
    end
    add_includedirs("include")
    add_files("src/*.cc|checker.cc")
    add_packages("unordered_dense")
    add_packages("gurobi")
    add_packages("fmt")
    add_ldflags("-lboost_program_options")

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
    add_packages("gurobi")
    add_packages("fmt")
    add_ldflags("-lboost_program_options")
