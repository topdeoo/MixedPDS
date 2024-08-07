#include "options.hh"
#include "problem.hh"
#include "types.hh"
#include <bits/getopt_core.h>
#include <cstdlib>
#include <getopt.h>
#include <sys/resource.h>
#include <unistd.h>

#include <iostream>

const static option long_opts[] = {
    { "help", no_argument, 0, 0 },
    { "filename", required_argument, 0, 'f' },
    { "number-of-solution", required_argument, 0, 'n' },
    { "max-age", required_argument, 0, 'a' },
    { "milp-time-limit", required_argument, 0, 't' },
    { "cpu-epoch", required_argument, 0, 'e' },
    { 0, 0, 0, 0 } };

void show_help() {
  std::cerr << R"(Usage:
 ./solver.elf [options]

Options: 
  -f, --filename <filename>
	 Input instance filename

  -n, --number-of-solution
	 Number of solution constructions per iteration

  -a, --age-max <number>
	 Upper limit for the age-values

  -t, --milp-time-limit <number>
	 Time limit for MILP Solver per iteration
  
  -e, --cpu-epoch <number>
   Time limit of the whole algorithm

  -h,--help
	 Show help
  
  )";
}

static u64 process_time() {
  struct rusage r;
  getrusage( RUSAGE_SELF, &r );
  auto t = r.ru_utime;
  return t.tv_usec;
}

int main( int argc, char **argv ) {

  std::cerr << "Current working directory: " << getcwd( nullptr, 0 ) << std::endl;

  char c;
  int opt_index = 0;
  Options opts;

  while (
      ( c = getopt_long( argc, argv, "f:n:a:t:e:h", long_opts, &opt_index ) ) ) {
    if ( c == -1 ) {
      break;
    }
    switch ( c ) {
    case 'f':
      opts.filename = std::string( optarg );
      break;
    case 'n':
      opts.na = static_cast<u32>( std::atoi( optarg ) );
      break;
    case 'a':
      opts.age_max = std::atoi( optarg );
      break;
    case 't':
      opts.t_milp = static_cast<u32>( std::atoi( optarg ) );
      break;
    case 'e':
      opts.cutoff_time = static_cast<u32>( std::atoi( optarg ) );
      break;
    case 'h':
    default:
      show_help();
      exit( EXIT_FAILURE );
    }
  }

  Problem instance( opts );

  instance.parse();

  auto _start = process_time();

  instance.start();

  auto _end = process_time();

  std::cout << "[solution] " << instance.solution_size() << " [time] "
            << _end - _start << " us\n";

  return 0;
}
