#ifndef _INPUT_OUTPUT_H_
#define _INPUT_OUTPUT_H_

#include <sstream>
#include <string>
#include <vector>

// helper functions for reading and writing files

void read_file(const std::string& filename, std::vector<std::string>& line);
void lines_to_data_points(const std::vector<std::string>& line, std::vector< std::vector<double> >& data_set);

template<typename T>
std::string x_to_string(const T& x){
  // (C++11 has this for int, double, ...)
  // convert x to string by using stringstreams
  std::stringstream ss;
  ss << x;
  return ss.str();
}

#endif /* _INPUT_OUTPUT_H_ */