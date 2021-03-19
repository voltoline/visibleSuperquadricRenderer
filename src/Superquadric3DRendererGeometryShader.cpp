#include "Superquadric3DRendererGeometryShader.h"

#include "IndirectRenderingCommandProcessing.h"
#include "InstancedSuperquadricRendererProgramShader.h"

Superquadric3DRendererGeometryShader::Superquadric3DRendererGeometryShader(const unsigned short *volume_dim, const float *scaleFactors):
    BaseSuperquadricRendererGPU(volume_dim, scaleFactors)
{

}

BaseIndirectRenderingCommandProcessing *Superquadric3DRendererGeometryShader::createIndirectRenderingCommandProcessor()
{
    IndirectRenderingCommandProcessing *indirect_rendering_command_processing = new IndirectRenderingCommandProcessing();

    return indirect_rendering_command_processing;
}

BaseSuperquadricRendererProgramShader *Superquadric3DRendererGeometryShader::createSuperquadricRenderer()
{
    InstancedSuperquadricRendererProgramShader *superquadric_renderer =
            new InstancedSuperquadricRendererProgramShader(getVolumeDim(), getColorCodedFaTBO(), getTensorAttributesTBO(), getSuperquadricTransformationTBO());

    return superquadric_renderer;
}
