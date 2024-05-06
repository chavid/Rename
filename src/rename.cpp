
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <vector>
#include <set>
#include <unordered_map>
#include <algorithm>
#include <cstdlib>
#include <cassert>
#include <thread>
#include <mutex>
#include <string>
#include <string_view>
#include <format>
#include <regex>

#include "argparse.hpp"
#include "utilities.hh"

struct Context
 {
  StringViewSet rejected ;
  std::regex from ;
  std::string to ;
  bool check ;
 } ;

void rename_file( Context & cs, fs::path const & path )
 {
  // rejected extensions
  std::string ext { path.extension().generic_string() } ;
  lower(ext) ;
  if (cs.rejected.contains(ext))
   { return ; }
  // test
  bool modified { false } ;
  auto filename = path.filename().string() ;
  std::smatch from_match ;
  while ( std::regex_match(filename,from_match,cs.from) )
   {
    std::string newname { cs.to } ;
    const std::regex wildcard_re { "%" } ;
    for ( std::size_t i = 1 ; i < from_match.size() ; ++i )
      { newname = std::regex_replace(newname,wildcard_re,from_match[i].str(),std::regex_constants::format_first_only) ; }
    modified = true ;
    filename = newname ;
   }
  
  // rename
  if ( modified )
   {
    std::cout<<path<<" => "<<filename<<std::endl ;
    if (!cs.check)
     { fs::rename(path,path.parent_path()/filename) ; }
   }

  // end
  return ;
 }

template< class RegularFileFunction > 
void scan_dir( fs::path const & path, RegularFileFunction uf )
 {
  auto s = fs::status(path) ;
  if (fs::is_symlink(s))
   { return ; }
  if (fs::is_directory(s))
   {
    std::string dirname { path.filename().string() } ;
    // ignore .* directories
    if ((std::size(dirname)>1)&&(dirname[0]=='.'))
     { return ; }
    // ignore __* directories
    if ((std::size(dirname)>1)&&(dirname[0]=='_')&&(dirname[1]=='_'))
     { return ; }
    for ( auto it = fs::directory_iterator(path) ; it != fs::directory_iterator() ; ++it )
     { scan_dir(*it,uf) ; }
   }
  else if (fs::is_regular_file(s))
   { uf(path) ; }
 }

int main( int argc, char const* argv[] )
 {
  // Parse arguments
  argparse::ArgumentParser program("rn", "0.1", argparse::default_arguments::all);
  program.add_description("Rename files using global patterns.");
  program.add_argument("-d", "--directory")
    .default_value(fs::current_path().string())
    .help("specify the directory to start from.") ;
  program.add_argument("-c", "--check")
    .default_value(false)
    .implicit_value(true)
    .help("do not really apply the renaming.") ;
  program.add_argument("from")
    .help("the input names pattern."); 
  program.add_argument("to")
    .help("the output names pattern."); 
  try
   {
    program.parse_args(argc, argv);
   }
  catch (const std::runtime_error& err)
   {
    std::cerr << err.what() << std::endl;
    std::cerr << program;
    return 1;
   }
  fs::path arg_path { program.get<std::string>("--directory") } ;

  // Look at command line arguments
  std::string from_str = program.get<std::string>("from") ;
  std::string to_str = program.get<std::string>("to") ;
  std::size_t from_count = std::count(from_str.begin(),from_str.end(),'%') ;
  std::size_t to_count = std::count(to_str.begin(),to_str.end(),'%') ;
  if (from_count!=to_count)
   { throw std::invalid_argument{ "input and output patterns must have the same number of %" } ; }

  // Configuration and constant values
  Context cs ;
  cs.rejected = { ".exe", ".html", ".sty" } ;
  from_str = escape(from_str) ;
  from_str = std::regex_replace(from_str,std::regex("%"),"(.+)") ;
  cs.from = std::regex { from_str } ;
  cs.to = to_str ;
  cs.check = program.get<bool>("--check") ;

  // Scan directory
  std::cout<<std::endl ;
  scan_dir(arg_path,[&]( fs::path const & path )
   { rename_file(cs, path) ; });

  std::cout<<std::endl ;
  std::exit(0) ;
 }
