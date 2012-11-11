#include <fstream>
#include <iostream>
#include <stdexcept>
#include "input_output.hpp"


void read_file(const std::string& filename, std::vector<std::string>& line){
  // read lines from filename into a vector of strings,
  // ignore lines commented with "#"

  std::ifstream file(filename.c_str());
  if(!file.good())
    throw(std::runtime_error("Could not open file " + filename));


  std::string l;
  while(std::getline(file, l)){
    if(l[0] == '#')
      continue;			// comments

    line.push_back(l);
  }
}


void lines_to_data_points(const std::vector<std::string>& line,
			  std::vector< std::vector<double> >& data_set){
  // convert vector of lines to vector of data points,
  // which are vector<double>

  if(line.size() == 0)
    return;
  
  size_t data_point_size = 0;
  double tmp;
  std::stringstream ss(line[0]);
  while(ss >> tmp)
    ++data_point_size;
  if(ss.fail() && !ss.eof())
    throw(std::runtime_error("Could not convert data points from strings"));
  if(data_point_size == 0)
    throw(std::runtime_error("Size of first data point is apparently 0"));
    
  data_set.assign(line.size(), std::vector<double>(data_point_size));

  for(size_t i = 0; i != line.size(); ++i){
    std::stringstream ss(line[i]);
    for(size_t j = 0; j != data_point_size; ++j){
      if(!(ss >> data_set[i][j]))      
	throw(std::runtime_error(std::string("Could not read data point on line ") + x_to_string(i + 1)));
    }
    if(!ss.eof())
      throw(std::runtime_error(std::string("Data point on line ") + x_to_string(i + 1) + std::string(" is too long")));
  }
}
