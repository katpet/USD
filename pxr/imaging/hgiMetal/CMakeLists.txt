set(PXR_PREFIX pxr/imaging)
set(PXR_PACKAGE hgiMetal)

if (NOT ${PXR_BUILD_GPU_SUPPORT} OR NOT ${PXR_ENABLE_METAL_SUPPORT})
    message(STATUS
        "Skipping ${PXR_PACKAGE} because PXR_BUILD_GPU_SUPPORT or PXR_ENABLE_METAL_SUPPORT is OFF")
    return()
endif()

pxr_library(hgiMetal
    LIBRARIES
        arch
        hgi
        tf
        trace
        "-framework Metal"
        "-framework AppKit"

    PUBLIC_HEADERS
        api.h
        blitCmds.h
        buffer.h
        capabilities.h
        computeCmds.h
        computePipeline.h
        diagnostic.h
        graphicsCmds.h
        graphicsPipeline.h
        hgi.h
        indirectCommandEncoder.h
        resourceBindings.h
        sampler.h
        shaderFunction.h
        shaderProgram.h
        stepFunctions.h
        texture.h

    PRIVATE_HEADERS
        conversions.h
        shaderGenerator.h
        shaderSection.h

    CPPFILES
        blitCmds.mm
        buffer.mm
        capabilities.mm
        computePipeline.mm
        computeCmds.mm
        conversions.mm
        diagnostic.mm
        graphicsCmds.mm
        graphicsPipeline.mm
        hgi.mm
        indirectCommandEncoder.mm
        resourceBindings.mm
        sampler.mm
        shaderFunction.mm
        shaderGenerator.mm
        shaderProgram.mm
        shaderSection.mm
        stepFunctions.mm
        texture.mm

    RESOURCE_FILES
        plugInfo.json

    DISABLE_PRECOMPILED_HEADERS
)
