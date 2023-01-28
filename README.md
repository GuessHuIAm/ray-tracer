
# Some Extras:
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