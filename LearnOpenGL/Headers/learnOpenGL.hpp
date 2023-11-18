#ifndef LEARN_OPEN_GL_H
#define LEARN_OPEN_GL_H

// #include <assimp/Importer.hpp>
// #include <assimp/postprocess.h>
// #include <assimp/scene.h>
// #include <btBulletDynamicsCommon.h>
#define GLM_FORCE_XYZW_ONLY
#define GLM_FORCE_RADIANS
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>

// Reference: https://github.com/nothings/stb/blob/master/stb_image.h#L4
// To use stb_image, add this in *one* C++ source file.
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

const int cWindowWidth = 1920;
const int cWindowHeight = 1080;

#endif
