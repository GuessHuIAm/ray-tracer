#include "ArgParser.hpp"

#include <cstring>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <iostream>

ArgParser::ArgParser(int argc, const char* argv[]) {
  SetDefaultValues();

  for (int i = 1; i < argc; i++) {
    // rendering output
    if (!strcmp(argv[i], "-input")) {
      i++;
      assert(i < argc);
      input_file = argv[i];
    } else if (!strcmp(argv[i], "-output")) {
      i++;
      assert(i < argc);
      output_file = argv[i];
    } else if (!strcmp(argv[i], "-size")) {
      i++;
      assert(i < argc);
      width = atoi(argv[i]);
      i++;
      assert(i < argc);
      height = atoi(argv[i]);
    } else if (!strcmp(argv[i], "-bounces")) {
      i++;
      assert(i < argc);
      bounces = atoi(argv[i]);
    } else if (!strcmp(argv[i], "-shadows")) {
      shadows = true;
    } else if (!strcmp(argv[i], "-projection")) {
      i++;
      assert(i < argc);
      camera = argv[i];
      if (camera == "perspective") {
        camera_type = CameraType::Perspective;
      }
      else if (camera == "orthographic") {
        camera_type = CameraType::Orthographic;
        i++;
        assert(i < argc);
        camera_size = atof(argv[i]);
        if (camera_size <= 0.f) {
          printf("Orthographic project plane size cannot be: '%s' by '%s'!\n", argv[i], argv[i]);
          exit(1);
        }
      }
      else if (camera == "multiperspective") {
        camera_type = CameraType::Multiperspective;
        i++;
        assert(i < argc);
        camera_size = atof(argv[i]);
        if (camera_size <= 0.f) {
          printf("Multiperspective arc size cannot be: '%s'!\n", argv[i]);
          exit(1);
        }
      }
      else {
        printf("Cannot detect projection type '%s'!\n", argv[i]);
      }
    } else if (!strcmp(argv[i], "-antialiased")) {
      i++;
      assert(i < argc);
      if (!strcmp(argv[i], "jitter")) {
        jitter = true;
        i++;
        assert(i < argc);
        filter_grid = atoi(argv[i]);
        if (filter_grid <= 0) {
          printf("Cannot jitter using a grid sized '%s'!\n", argv[i]);
          exit(1);
        }
      }
      else if (!strcmp(argv[i], "filter")) {
        filter = true;
        i++;
        assert(i < argc);
        filter_grid = atoi(argv[i]);
        if (filter_grid <= 0) {
          printf("Cannot grid-filter using a grid sized '%s'!\n", argv[i]);
          exit(1);
        }
      }
      else {
        printf("Unknown anti-aliasing type: '%s'\n", argv[i]);
        exit(1);
      }
    } else {
      printf("Unknown command line argument %d: '%s'\n", i, argv[i]);
      exit(1);
    }
  }

  std::cout << "Args:\n";
  std::cout << "- input: " << input_file << std::endl;
  std::cout << "- output: " << output_file << std::endl;
  std::cout << "- projection: " << camera << std::endl;
  std::cout << "- width: " << width << std::endl;
  std::cout << "- height: " << height << std::endl;
  std::cout << "- bounces: " << bounces << std::endl;
  std::cout << "- shadows: " << shadows << std::endl;
  std::cout << "- anti-aliased: " << (filter || jitter) << std::endl;
  if (jitter) {
    std::cout << "--- with " << filter_grid << " by " << filter_grid << " jittering " << std::endl;
  }
  else if (filter) {
    std::cout << "--- with " << filter_grid << " by " << filter_grid << " grid filtering " << std::endl;
  }
}

void ArgParser::SetDefaultValues() {
  input_file = "";
  output_file = "";
  normals_file = "";
  width = 200;
  height = 200;

  bounces = 0;
  shadows = false;
  filter = false;
  jitter = false;
  filter_grid = 1;
  camera_type = CameraType::Perspective;
  camera_size = 1.f;
  camera = "perspective";
}
