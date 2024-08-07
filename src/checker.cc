#include "problem.hh"
#include <boost/program_options.hpp>
#include <cassert>
#include <cstdlib>
#include <fmt/color.h>
#include <fmt/core.h>
#include <fstream>
#include <iostream>

namespace po = boost::program_options;

int main( int argc, char **argv ) {
  po::options_description desc( "options" );

  std::string graph_filename, graph_solution;

  desc.add_options()( "graph,g",
                      po::value<std::string>( &graph_filename )->required(),
                      "Input instance filename" )(
      "solution,s", po::value<std::string>( &graph_solution )->required(),
      "Solution of the input instance" )( "help,h", "show help infomation" );

  try {

    po::variables_map vm;
    po::store( po::parse_command_line( argc, argv, desc ), vm );

    if ( vm.count( "help" ) ) {

      std::cerr << desc << std::endl;
      return EXIT_SUCCESS;
    }

    po::notify( vm );

  } catch ( const po::error &e ) {
    fmt::print( fmt::fg( fmt::color::red ), "{}\n", e.what() );
    std::cerr << desc << std::endl;
    return EXIT_FAILURE;
  }

  Problem instance;

  instance.set_filename( graph_filename );

  instance.parse();

  std::ifstream fin( graph_solution );

  u32 solution_size;

  fin >> solution_size;

  for ( u32 i = 0; i < solution_size; i++ ) {
    u32 v;
    fin >> v;
    instance.set_dominanting( v );
  }

  if ( instance.solved() ) {
    fmt::print( fmt::fg( fmt::color::green ), "PASSED\n" );
  } else {
    fmt::print( fmt::fg( fmt::color::red ), "FAILED\n" );
    auto observed_set = instance.observed_set();
    for ( auto &v : instance.vertices() ) {
      if ( !observed_set[v] ) {
        fmt::print( fmt::fg( fmt::color::yellow ), "{} ", v );
      }
    }
  }

  return EXIT_SUCCESS;
}