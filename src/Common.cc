/*
  Common functions between different files

*/

#include <algorithm>

#include "../include/Common.h"

//==============================================================
std::vector<CONFIND::Operation> CONFIND::Timer::Ops = {} ;
//==============================================================
std::ostream& operator << ( std::ostream &output, CONFIND::Operation op )
{ 
  char tmp[75] ;
  sprintf(tmp, "%-40s %-10.2f", op.name.c_str(), op.duration) ;
  output << tmp ;
  return output;
}

//==============================================================
void CONFIND::ReportTiming()
{
  std::cout<< "  ------------------------------------------------------------------ \n" ;
  std::cout<< " |\t\t***\t Opeartion Timing Report \t***\t    |\n" ;
  std::cout<< " |------------------------------------------------------------------|\n" ;
  std::cout<< " | # \t Name\t\t\t\t\t Duration (ms)      |\n" ;
  std::cout<< " |------------------------------------------------------------------|\n" ;
  for (size_t i = 0; i < CONFIND::Timer::Ops.size() ; i++)
  {
      std::cout<< " | " << i+1 << " |\t" << CONFIND::Timer::Ops[i] << "\t    |\n" ;
  }
  std::cout<< "  ================================================================== \n\n" ;
}

//==============================================================
std::ostream& operator << ( std::ostream &output, CONFIND::Quantity q )
{ 
  char tmp[50] ;
  sprintf(tmp, "%.3e \u00B1 %.3e", q.val, q.err) ;
  output << tmp ;
  return output;            
}

//==============================================================
std::ostream& operator << ( std::ostream &output, CONFIND::Range<double> r )
{ 
  char tmp[50] ;
  sprintf(tmp, "[%.3e , %.3e]", r.min, r.max) ;
  output << tmp ;
  return output;            
}

//==============================================================
std::ostream& operator << ( std::ostream &output, CONFIND::Range<int> r )
{ 
  char tmp[50] ;
  sprintf(tmp, "[%d , %d]", r.min, r.max) ;
  output << tmp ;
  return output;            
}

//==============================================================
std::ostream& operator << ( std::ostream &output, CONFIND::Range<size_t> r )
{ 
  char tmp[50] ;
  sprintf(tmp, "[%zu , %zu]", r.min, r.max) ;
  output << tmp ;
  return output;            
}

//==============================================================
// std::istream& operator>>(std::istream& str, CSVRow& data)
// {
//   data.readNextRow(str);
//   return str;
// }
//==============================================================

//==============================================================
// Records the input quantity (int, float or double) and saves in
// multiple (# of threads) text files.
template <class T> 
void CONFIND::saveVec(const std::vector<T>& list, std::string out_file_str, const char* mode)
{
  if (list.size() ==0 ) return ;
  out_file_str = out_file_str + ".dat" ;

  std::FILE * out_file ;
  out_file = fopen(out_file_str.c_str(), mode) ;

  if (!out_file) { std::cout << "File: '" << out_file_str << "' didn't open!" << "\n" ;}

  for(size_t i=0 ; i < list.size() ; ++i)
    { fprintf(out_file,"%.4e\n", list[i]) ; }

  fclose(out_file) ;
}

//-------------------------------------------------------
// Specialization for int instances
template <> 
void CONFIND::saveVec<int>(const std::vector<int>& list, std::string out_file_str, const char* mode)
{
  if (list.size() ==0 ) return ;

  out_file_str = out_file_str + ".dat" ;

  std::FILE * out_file ;
  out_file = fopen(out_file_str.c_str(), mode) ;

  for(size_t i=0; i < list.size(); ++i)
    { fprintf(out_file,"%d\n", list[i]) ; }

  fclose(out_file) ;
}

//-------------------------------------------------------
// Specialization for Coord3D instances
template <> 
void CONFIND::saveVec<CONFIND::Coord3D>(const std::vector<CONFIND::Coord3D>& list, std::string out_file_str, const char* mode)
{
  if (list.size() ==0 ) return ;

  out_file_str = out_file_str + ".dat" ;

  std::FILE * out_file ;
  out_file = fopen(out_file_str.c_str(), mode) ;

  fprintf(out_file,"%6c\t%14c\t%14c\n", 'X', 'Y', 'Z') ;
  for(size_t i=0; i < list.size(); ++i)
    { fprintf(out_file,"%.4e\t%.4e\t%.4e\n", list[i].x, list[i].y, list[i].z) ; }

  fclose(out_file) ;
}

//-------------------------------------------------------
// Specialization for double vector instances
template <> 
void CONFIND::saveVec<std::vector<double> >(const std::vector<std::vector<double> >& list, std::string out_file_str,
                                   std::string delim, const char* mode)
{
  if (list.size() ==0 ) return ;

  std::FILE * out_file ;
  out_file = fopen(out_file_str.c_str(), mode) ;

  for(size_t i=0; i < list.size(); ++i)
  { 
    for(size_t j=0; j < list[i].size(); ++j)
    {
      // If we want CSV file, and not the last element of the line
      if(j != list[i].size()-1 )
        fprintf(out_file,"%.4e%s", list[i][j], delim.c_str() ) ; 
      else
        fprintf(out_file,"%.4e", list[i][j]) ; 
    }
    fprintf(out_file,"\n");
  }

  fclose(out_file) ;
}

//-------------------------------------------------------
// Specialization for int vector instances
template <> 
void CONFIND::saveVec<std::vector<int> >(const std::vector<std::vector<int> >& list, std::string out_file_str,
                                std::string delim, const char* mode)
{
  if (list.size() ==0 ) return ;

  std::FILE * out_file ;
  out_file = fopen(out_file_str.c_str(), mode) ;

  for(size_t i=0; i < list.size(); ++i)
  { 
    for(size_t j=0; j < list[i].size(); ++j)
    { 
      // If we want CSV file, and not the last element of the line
      if( j != list[i].size()-1 )
        fprintf(out_file,"%d%s", list[i][j], delim.c_str() ) ; 
      else
        fprintf(out_file,"%d", list[i][j]) ; 
      
    }
    fprintf(out_file,"\n");
  }

  fclose(out_file) ;
}

//==============================================================
template <class T> 
bool contains( std::vector<T>& Vec, T Element ) 
{
  if (std::find(Vec.begin(), Vec.end(), Element) != Vec.end())
      return true ;

  return false ;
}

//==============================================================
template <class T> void rm_elem(std::vector<T>& list, 
                                std::vector<size_t> rm_index_list)
// Removing certain members of a vector, 
// given another vector containing an index set.
{
      size_t    j = 0 ;
  for(size_t i = 0 ; i < list.size() ; ++i)
  {
    if( contains(rm_index_list, i+j) )
    {
      list.erase(list.begin()+i) ;
      i--                        ;
      j++                        ;
    }
  }
}

//==============================================================
// Root color mapping 
// Ref: https://root.cern.ch/doc/master/classTColor.html
std::array<EColor, 13> RColor = {kRed, kGreen, kBlue, kMagenta,
                                 kTeal, kOrange, kCyan, kViolet,
                                kGray, kPink, kAzure, kYellow, kSpring} ;

EColor CONFIND::RColorMap(size_t i) 
{ 
  return static_cast<EColor>(RColor[i % 13] + 5*i/13) ;
}

//==============================================================
//                    explicit instantiations
//==============================================================
// SaveVec
template void CONFIND::saveVec<float>(const std::vector<float>&, std::string, const char*) ;
template void CONFIND::saveVec<double>(const std::vector<double>&, std::string, const char* mode) ;
template void CONFIND::saveVec<int>(const std::vector<int>&, std::string, const char* mode) ;
template void CONFIND::saveVec<CONFIND::Coord3D>(const std::vector<CONFIND::Coord3D>&, std::string, const char*) ;

template void CONFIND::saveVec<std::vector<double> >(const std::vector<std::vector<double> >&, std::string, std::string, const char*) ;
template void CONFIND::saveVec<std::vector<int> >(const std::vector<std::vector<int> >&, std::string, std::string, const char*) ;
// //--------------------------------
// // contains
// template bool contains<int>(std::vector<int>&, int) ;
// template bool contains<double>(std::vector<double>&, double) ;
// template bool contains<size_t>(std::vector<size_t>&, size_t) ;

// //--------------------------------
// // rm_elem
// template void rm_elem<double>(std::vector<double>&, std::vector<size_t>) ;
//==============================================================