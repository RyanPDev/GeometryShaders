#pragma once

const int NUM_BILLBOARDS{ 10 };

#pragma region Obj

// Cat
const char* catObj = "obj/cat.obj";

#pragma endregion

#pragma region Texture Paths

// Billboards
const char* treeTexture = "materials/tree_texture.png";
const char* treeTexture2 = "materials/tree_texture2.png";
const char* treeTexture3 = "materials/tree_texture3.png";

// Model
const char* catTexture = "materials/cat_texture.jpg";

#pragma endregion

#pragma region Shader Paths

// Imported Models
const char* modelVS = "shaders/models/shader.vs";
const char* modelFS = "shaders/models/shader.fs";

// Billboard
const char* bbVS = "shaders/billboard/bbshader.vs";
const char* bbFS = "shaders/billboard/bbshader.fs";
const char* bbGS = "shaders/billboard/bbshader.gs";

// Explosion
const char* explosionVS = "shaders/explosion/exshader.vs";
const char* explosionFS = "shaders/explosion/exshader.fs";
const char* explosionGS = "shaders/explosion/exshader.gs";

#pragma endregion
