add_rules("mode.debug", "mode.release")

-- Packages
add_requires("unordered_dense")
add_requires("gurobi", { system = true })
add_requires("fmt", { system = true })
add_requires("boost", { system = true })

target("solver.elf")
    set_rundir("$(projectdir)")
    set_kind("binary")
    -- CXX Flags
    set_warnings("all", "error")
    -- set_optimize("fastest")
    add_includedirs("include")
    add_files("src/*.cc")
    add_packages("unordered_dense")
    add_packages("gurobi")
    add_packages("fmt")

