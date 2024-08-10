#pragma once

#include "types.hh"

struct Options {
  //* instance filename
  std::string filename;
  //* $t_{MILP}$: Time limit for MILP Solver per iteration
  u32 t_milp;
  Options() = default;
  ~Options() = default;

  Options( const Options &rhs ) = default;
};
