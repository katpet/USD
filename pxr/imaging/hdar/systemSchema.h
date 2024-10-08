//
// Copyright 2023 Pixar
//
// Licensed under the terms set forth in the LICENSE.txt file available at
// https://openusd.org/license.
//
////////////////////////////////////////////////////////////////////////

/* ************************************************************************** */
/* **                                                                      ** */
/* ** This file is generated by a script.                                  ** */
/* **                                                                      ** */
/* ** Do not edit it directly (unless it is within a CUSTOM CODE section)! ** */
/* ** Edit hdSchemaDefs.py instead to make changes.                        ** */
/* **                                                                      ** */
/* ************************************************************************** */

#ifndef PXR_IMAGING_HDAR_SYSTEM_SCHEMA_H
#define PXR_IMAGING_HDAR_SYSTEM_SCHEMA_H

/// \file

#include "pxr/imaging/hdar/api.h"

#include "pxr/imaging/hd/schema.h"

// --(BEGIN CUSTOM CODE: Includes)--
#include "pxr/base/tf/declarePtrs.h"
// --(END CUSTOM CODE: Includes)--

PXR_NAMESPACE_OPEN_SCOPE

// --(BEGIN CUSTOM CODE: Declares)--
TF_DECLARE_REF_PTRS(HdSceneIndexBase);
// --(END CUSTOM CODE: Declares)--

#define HDAR_SYSTEM_SCHEMA_TOKENS \
    (assetResolution) \
    (resolverContext) \

TF_DECLARE_PUBLIC_TOKENS(HdarSystemSchemaTokens, HDAR_API,
    HDAR_SYSTEM_SCHEMA_TOKENS);

//-----------------------------------------------------------------------------

// The HdarSystemSchema specifies a container that will hold "system" data
// that is relevant to asset resolution.
//

class HdarSystemSchema : public HdSchema
{
public:
    /// \name Schema retrieval
    /// @{

    HdarSystemSchema(HdContainerDataSourceHandle container)
      : HdSchema(container) {}

    /// Retrieves a container data source with the schema's default name token
    /// "assetResolution" from the parent container and constructs a
    /// HdarSystemSchema instance.
    /// Because the requested container data source may not exist, the result
    /// should be checked with IsDefined() or a bool comparison before use.
    HDAR_API
    static HdarSystemSchema GetFromParent(
        const HdContainerDataSourceHandle &fromParentContainer);

    /// @}

// --(BEGIN CUSTOM CODE: Schema Methods)--

    /// Evaluates the asset resolution "system" data source for \p fromPath.
    ///
    /// If found, the return value will be non-null and \p foundAtPath
    /// will contain the path at which the non-null result was found.
    /// Otherwise, this returns null.
    ///
    /// This operation will be linear in the length of \p fromPath.
    HDAR_API
    static HdContainerDataSourceHandle GetFromPath(
            HdSceneIndexBaseRefPtr const &inputScene,
            SdfPath const &fromPath,
            SdfPath *foundAtPath);

// --(END CUSTOM CODE: Schema Methods)--

    /// \name Member accessor
    /// @{

    HDAR_API
    HdResolverContextDataSourceHandle GetResolverContext() const; 

    /// @}

    /// \name Schema location
    /// @{

    /// Returns a token where the container representing this schema is found in
    /// a container by default.
    HDAR_API
    static const TfToken &GetSchemaToken();

    /// Returns an HdDataSourceLocator (relative to the prim-level data source)
    /// where the container representing this schema is found by default.
    HDAR_API
    static const HdDataSourceLocator &GetDefaultLocator();

    /// @} 

    /// \name Schema construction
    /// @{

    /// \deprecated Use Builder instead.
    ///
    /// Builds a container data source which includes the provided child data
    /// sources. Parameters with nullptr values are excluded. This is a
    /// low-level interface. For cases in which it's desired to define
    /// the container with a sparse set of child fields, the Builder class
    /// is often more convenient and readable.
    HDAR_API
    static HdContainerDataSourceHandle
    BuildRetained(
        const HdResolverContextDataSourceHandle &resolverContext
    );

    /// \class HdarSystemSchema::Builder
    /// 
    /// Utility class for setting sparse sets of child data source fields to be
    /// filled as arguments into BuildRetained. Because all setter methods
    /// return a reference to the instance, this can be used in the "builder
    /// pattern" form.
    class Builder
    {
    public:
        HDAR_API
        Builder &SetResolverContext(
            const HdResolverContextDataSourceHandle &resolverContext);

        /// Returns a container data source containing the members set thus far.
        HDAR_API
        HdContainerDataSourceHandle Build();

    private:
        HdResolverContextDataSourceHandle _resolverContext;

    };

    /// @}
};

PXR_NAMESPACE_CLOSE_SCOPE

#endif