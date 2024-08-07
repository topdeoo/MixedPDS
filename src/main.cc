#include "options.hh"
#include "problem.hh"
#include "types.hh"
#include <bits/getopt_core.h>
#include <boost/program_options.hpp>
#include <cstdlib>
#include <getopt.h>
#include <sys/resource.h>
#include <unistd.h>

#include <iostream>

namespace po = boost::program_options;

static u64 process_time() {
  struct rusage r;
  getrusage( RUSAGE_SELF, &r );
  auto t = r.ru_utime;
  return t.tv_usec;
}

int main( int argc, char **argv ) {
  Options opts;
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
      "Time limit of the whole algorithm" )( " help,h ", "show help infomation" );

  try {

    po::variables_map vm;
    po::store( po::parse_command_line( argc, argv, desc ), vm );

    if ( vm.count( "help" ) ) {
      std::cerr << desc << std::endl;
      return EXIT_SUCCESS;
    }

    po::notify( vm );

  } catch ( const po::error &e ) {
    std::cerr << "\033[031mError: " << e.what() << "\033[m\n";
    std::cerr << desc << std::endl;
    return EXIT_FAILURE;
  }

  Problem instance( opts );

  instance.parse();

  auto _start = process_time();

  instance.start();

  auto _end = process_time();

  std::cout << "[solution] " << instance.solution_size() << " [time] "
            << _end - _start << " us\n";
  return EXIT_SUCCESS;
}
