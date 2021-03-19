/**
  * @file Superquadric3DRenderGeometryShader.h
  * @author Raphael Voltoline
  * @author Wu Shin-Ting
  * @date 2019
  */
#ifndef SUPERQUADRIC_3D_RENDERER_GEOMETRY_SHADER_H
#define SUPERQUADRIC_3D_RENDERER_GEOMETRY_SHADER_H

#include "BaseSuperquadricRendererGPU.h"

/**
 * @ingroup DWI_Volume_Processing
 *
 * @brief  Derived class of BaseSuperquadricRendererGPU to render adaptative triangular meshes of the visible tensor glyphs using geometry shader (OpenGL >= 4.0).
 *
*/

class Superquadric3DRendererGeometryShader: public BaseSuperquadricRendererGPU
{

public:
    /**
     * @brief Constructor of an object of the class Superquadric3DRenderGeometryShader.
     *
     * @param volume_dim dimensions of the volume, in voxels.
     * @param scaleFactors scale factors for x-, y- and z-axis.
     */
    Superquadric3DRendererGeometryShader(const unsigned short *volume_dim, const float *scaleFactors);

protected:
    /**
     * @brief overrided method to create a custom indirect rendering command processor
     */
    virtual BaseIndirectRenderingCommandProcessing* createIndirectRenderingCommandProcessor();

    /**
     * @brief overrided method to create a custom renderer to render superquadric with the calculated tringular mesh
     */
    virtual BaseSuperquadricRendererProgramShader* createSuperquadricRenderer();

};

#endif // SUPERQUADRIC_3D_RENDERER_GEOMETRY_SHADER_H
