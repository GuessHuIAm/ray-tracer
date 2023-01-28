#ifndef ARG_PARSER_H_
#define ARG_PARSER_H_

#include <string>
#include "Tracer.hpp"

class ArgParser {
 public:
  ArgParser(int argc, const char* argv[]);

  std::string input_file;
  std::string output_file;
  std::string depth_file;
  std::string normals_file;
  size_t width;
  size_t height;

  // Rendering options.
  float depth_min;
  float depth_max;
  size_t bounces;
  bool shadows;

  // Supersampling.
  bool jitter;
  bool filter;
  int filter_grid;

  // Camera options.
  CameraType camera_type;
  float camera_size;
  std::string camera;

 private:
  void SetDefaultValues();
};

#endif  // ARG_PARSER_H
