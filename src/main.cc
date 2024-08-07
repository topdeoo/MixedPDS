#include "options.hh"
#include "problem.hh"
#include "types.hh"
#include <boost/program_options.hpp>
#include <boost/program_options/value_semantic.hpp>
#include <cstdlib>
#include <getopt.h>
#include <iostream>
#include <sys/resource.h>
#include <unistd.h>

#include <fmt/color.h>
#include <fmt/core.h>
#include <fmt/os.h>

namespace po = boost::program_options;

static u64 process_time() {
  struct rusage r;
  getrusage( RUSAGE_SELF, &r );
  auto t = r.ru_utime;
  return t.tv_usec;
}

int main( int argc, char **argv ) {
  Options opts;

  std::string outputfile = "none";

  po::options_description desc( "options" );

  desc.add_options()( "filename,f",
                      po::value<std::string>( &opts.filename )->required(),
                      "Input instance filename" )(
      "number-of-solution,n", po::value<u32>( &opts.na )->default_value( 1 ),
      "Number of solution constructions per iteration" )(
      "age-max,a", po::value<i32>( &opts.age_max )->default_value( 4 ),
      "Upper limit for the age-values" )(
      "milp-time-limit,t", po::value<u32>( &opts.t_milp )->default_value( 16 ),
      "Time limit of the whole algorithm" )(
      "cutoff,c", po::value<u32>( &opts.cutoff_time )->default_value( 1000 ),
      "Time limit of the whole algorithm" )( "output,o", po::value<std::string>() )(
      "help,h", "show help infomation" );

  try {

    po::variables_map vm;
    po::store( po::parse_command_line( argc, argv, desc ), vm );

    if ( vm.count( "help" ) ) {
      std::cerr << desc << std::endl;
      return EXIT_SUCCESS;
    }

    po::notify( vm );

    if ( vm.count( "output" ) ) {
      outputfile = vm["output"].as<std::string>();
    }

  } catch ( const po::error &e ) {
    fmt::print( fmt::fg( fmt::color::red ), "{}\n", e.what() );
    std::cerr << desc << std::endl;
    return EXIT_FAILURE;
  }

  Problem instance( opts );

  instance.parse();

  auto _start = process_time();

  instance.start();

  auto _end = process_time();

  fmt::println( "[solution size] {0}, [time] {1} us", instance.solution_size(),
                _end - _start );

  if ( outputfile != "none" ) {
    auto fp = fmt::output_file( outputfile );
    fp.print( "{}\n{}\n", _end - _start, instance.solution_size() );
    for ( auto &v : instance.solution() ) {
      fp.print( "{} ", v );
    }
  }

  return EXIT_SUCCESS;
}
