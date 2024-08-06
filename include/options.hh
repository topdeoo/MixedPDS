#pragma once

#include "types.hh"

struct Options {
  //* instance filename
  std::string filename;
  //* $n_a$: Number of solution constructions per iteration
  u32 na;
  //* $age_{max}$: Upper limit for the age-values
  i32 age_max;
  //* $t_{MILP}$: Time limit for MILP Solver per iteration
  u32 t_milp;

  u32 cutoff_time;

  Options() = default;
  ~Options() = default;

  Options( const Options &rhs ) = default;
};
