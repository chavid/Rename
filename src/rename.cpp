
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
  std::string from_str ;
  std::string to_str ;
  std::regex from ;
  bool once = false ;
  bool check ;
 } ;

void rename_entry( Context & cs, fs::path const & path )
 {;
  // rejected extensions
  std::string ext { path.extension().generic_string() } ;
  lower(ext) ;
  if (cs.rejected.contains(ext))
   { return ; }
  // test
  bool modified { false } ;
  auto entry_name = path.filename().string() ;
  std::smatch from_match ;
  auto replace = [&]()
   {
    std::string newname { cs.to_str } ;
    const std::regex wildcard_re { "%" } ;
    for ( std::size_t i = 1 ; i < from_match.size() ; ++i )
      { newname = std::regex_replace(newname,wildcard_re,from_match[i].str(),std::regex_constants::format_first_only) ; }
    modified = true ;
    entry_name = newname ;
   } ;
  if (cs.once)
   {
    if ( std::regex_match(entry_name,from_match,cs.from) )
     { replace() ; }
   }
  else
   {
    while ( std::regex_match(entry_name,from_match,cs.from) )
     { replace() ; }
   }
  
  // rename
  if ( modified )
   {
    std::cout<<path<<" => "<<entry_name<<std::endl ;
    if (!cs.check)
     { fs::rename(path,path.parent_path()/entry_name) ; }
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
    uf(path) ;
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
  cs.from_str = std::regex_replace(from_str,std::regex("%"),"(.+)") ;
  cs.from = std::regex { cs.from_str } ;
  cs.to_str = to_str ;
  cs.check = program.get<bool>("--check") ;
  cs.once = std::regex_match(cs.to_str,cs.from) ;

  // Scan directory
  std::cout<<std::endl ;
  scan_dir(arg_path,[&]( fs::path const & path )
   { rename_entry(cs, path) ; });

  std::cout<<std::endl ;
  std::exit(0) ;
 }
