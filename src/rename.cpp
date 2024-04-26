
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

namespace fs = std::filesystem ;

using StringVec = std::vector<std::string> ;
using StringSet = std::set<std::string> ;
using StringViewVec = std::vector<std::string_view> ;
using StringViewSet = std::set<std::string_view> ;

struct Context
 {
  StringViewSet rejected ;
  std::string from ;
  std::string to ;
  bool check ;
 } ;

void lower( std::string & str )
 {
  std::transform(str.begin(),str.end(),str.begin(),
   [](unsigned char c){ return std::tolower(c) ; } ) ;
 }

void rename_file( Context & cs, fs::path const & path )
 {
  // rejected extensions
  std::string ext { path.extension().generic_string() } ;
  lower(ext) ;
  if ((ext.empty())||cs.rejected.contains(ext))
   { return ; }

  // echo
  auto filename = path.filename().string() ;
  auto pos = filename.find(cs.from) ;
  if (pos==std::string::npos)
   { return ; }
  filename.replace(pos,cs.from.length(),cs.to) ;
  std::cout<<path<<" => "<<filename<<std::endl ;
  if (!cs.check)
   { fs::rename(path,path.parent_path()/filename) ; }

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
    // ignore hidden directories
    if ((std::size(dirname)>1)&&(dirname[0]=='.')&&(dirname[1]!='.'))
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

  // Other configuration and constant values
  Context cs ;
  cs.rejected = { ".exe", ".html", ".sty" } ;
  cs.from = program.get<std::string>("from") ;
  cs.to = program.get<std::string>("to") ;
  cs.check = program.get<bool>("--check") ;

  // Scan directory
  std::cout<<std::endl ;
  scan_dir(arg_path,[&]( fs::path const & path )
   { rename_file(cs, path) ; });

  std::cout<<std::endl ;
  std::exit(0) ;
 }
