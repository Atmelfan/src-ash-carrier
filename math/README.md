# Math

Supporting linear algebra library.

## Vectors

The library implements a `vecx` type of arbitrary length. 
Specializations of length 3 (`vec3`) and length 4 (`vec4`) are typically used instead of `vecx`. 

## Matrices

The library implements a `matxx` type of arbitrary length. 
Specializations are 3x3 `mat3` and 4x4 `mat4`.

Most operators are only defined for operands of equal size (i.e. vec4 and vec4, mat4 for mat4 etc).

## Matrix * vector
Matrix multiplication of a vector is implemented in vecmath.

## Utils
The utils file provides utils functions for creating 4x4 rotation/translation matrices.

**Note:** Most 3D vectors are of the vec4 type with the 4th member set to 1. 
This makes translation and rotation with 4x4 matrices simpler.
