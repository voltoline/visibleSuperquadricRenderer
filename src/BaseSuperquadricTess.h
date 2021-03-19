/**
  * @file BaseSuperquadricTess.h
  * @author Raphael Voltoline
  * @author Wu Shin-Ting
  * @date 2019
  */
#ifndef BASE_SUPERQUADRIC_TESS_H
#define BASE_SUPERQUADRIC_TESS_H

#include "vmtkManagementShader.h"

/**
 * @ingroup DWI_Volume_Processing
 *
 * @brief Tesselate the parametric representation of the superquadrics glyphs on the GPU.
 *
 * The parametric representation of the superquadrics glyphs presented in
 * in "Superquadric tensor glyphs" by G. Kindlmann, VISSYM'04, is applied in
 * the construction of the triangular mesh of the basic superquadric shape G,
 * whose principal axes are aligned with the axes of the reference system.
 *
 * The number of triangles
 *
 * Shaders that make up the m_baseSuperquadricProcessingShaderProgram:
 * Vertex shader: base_superquadric_processing.vert;
 * Tesselation control shader: base_superquadric_processing.tcs;
 * Tesselation evaluation shader: base_superquadric_processing.tes.
 */
class BaseSuperquadricTess
{
public:
    /**
     * @brief Constructor of an object of the class BaseSuperquadricTess.
     *
     */
    BaseSuperquadricTess();

    /**
     * @brief Initialize
     *
     */
    void initialize();

    /**
     * @brief Compute the tesselation.
     *
     */
    void compute();

    /**
     * @brief Set the buffer object containing the basic superquadric data.
     *
     * @param buffer_id Identifier of the buffer object.
     */
    void setBaseSuperquadricInfoUniformBlockBufferId(GLuint buffer_id);

    /**
     * @brief Get the buffer object containing the basic superquadric data.
     *
     * @return GLuint Identifier of the object.
     */
    GLuint getBaseSuperquadricBufferId();

private:
    vmtkManagementShader *m_baseSuperquadricProcessingShaderProgram;    /**< GPU program */

    GLuint m_baseSuperquadricProcessingTransformFeedbackID;         /**< Identifier of the TransformFeedback object */
    GLuint m_baseSuperquadricProcessingTransformFeedbackBufferID;   /**< Identifier of the buffer object storing the tesselated data */

    GLuint m_baseSuperquadricInfoUniformBlockBufferID; /**< Identifier of the buffer object containing the dimension of the basic glyph grid */
};

#endif // BASE_SUPERQUADRIC_TESS_H
