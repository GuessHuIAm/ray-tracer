#include <iostream>
#include <chrono>

#include "gloo/Scene.hpp"
#include "gloo/components/MaterialComponent.hpp"

#include "hittable/Sphere.hpp"
#include "Tracer.hpp"
#include "SceneParser.hpp"
#include "ArgParser.hpp"

using namespace GLOO;

int main(int argc, const char* argv[]) {
  ArgParser arg_parser(argc, argv);
  SceneParser scene_parser;
  auto scene = scene_parser.ParseScene(arg_parser.input_file);

  Tracer tracer(scene_parser.GetCameraSpec(),
                arg_parser.camera_type, arg_parser.camera_size,
                glm::ivec2(arg_parser.width, arg_parser.height),
                arg_parser.bounces, scene_parser.GetBackgroundColor(),
                scene_parser.GetCubeMapPtr(), arg_parser.shadows,
                arg_parser.filter, arg_parser.filter_grid,
                arg_parser.jitter);
  tracer.Render(*scene, arg_parser.output_file);
  return 0;
}
