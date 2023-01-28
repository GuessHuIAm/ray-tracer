# Get Running:
1. At the root, run `cmake ./`.
2. Then, run `make`.
3. You should have an executable program called `ray_tracer` in your folder now!
4. To run the program, run `./ray_tracer` with the following tags:
    - `-input [scene_name].txt`
    
        For example, `-input scene07_arch.txt`. Sample scenes can be found in the `assets` folder.

    - `-output [file_name].png.txt`
        
        Give it any spaceless file name you want.

    - `-size [length] [width]`
    - `-shadows`

        If you desire shadows in your scene.
    - `-bounces [num_bounces]`
        If you want the light to bounce throughout the scene. Good for transparent objects.

# Some Extra Tags:
## Antialiasing
### Grid Filtering
Use this filtering by adding the tag `-antialiased filter [n > 0]` to filter with a `n x n` grid.

Example: `-antialiased filter 3`

### Jittering
Use jittering by adding the tag `-antialiased jitter [n > 0]` to jitter with by taking random samples using `n x n` subpixels.

Example: `-antialiased jitter 3`

## Cool Cameras
### Orthographic:
To use the orthographic camera, add the tag `-projection orthographic [size > 0]`, which renders your scenewith a "camera" plane scaled by `size`.

Example: `-projection orthographic 3`

### Multiperspective:
 To use the multiperspective camera, add the tag `-projection multiperspective [size > 0]`, which renders a scene with a camera on a parabolic track scaled by `size`.
 
 Example: `-projection multiperspective 9.25`