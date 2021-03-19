/**
  * @file IndirectRenderingCommandProcessing.h
  * @author Raphael Voltoline
  * @author Wu Shin-Ting
  * @date 2019
  */
#ifndef INDIRECT_RENDERING_COMMAND_PROCESSING_H
#define INDIRECT_RENDERING_COMMAND_PROCESSING_H

#include "BaseIndirectRenderingCommandProcessing.h"

/**
 * @ingroup DWI_Volume_Processing
 *
 * @brief Derived class of BaseIndirectRenderingCommandProcessing
 *
 * Provide the geometry shader for the scan step.
 *
 * Using geometry shader, the mesh resolution Delta used to represent a triangular mesh is computed dynamically
 * with respect to the maximum of covered pixels by all visible voxels, max_p:
 * Delta = rounding(3+square_root(4*max_p/(square_root(max_p)))). The number of visible voxels
 * are also computed for instancing rendering.
 *
 */

class IndirectRenderingCommandProcessing: public BaseIndirectRenderingCommandProcessing
{
public:
    /**
     * @brief Constructor of an object of the class IndirectRenderingCommandProcessing.
     *
     */
    IndirectRenderingCommandProcessing();
};

#endif // INDIRECT_RENDERING_COMMAND_PROCESSING_H
