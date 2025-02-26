//
// Copyright 2019 Pixar
//
// Licensed under the terms set forth in the LICENSE.txt file available at
// https://openusd.org/license.
//

#ifndef EXT_RMANPKG_25_0_PLUGIN_RENDERMAN_PLUGIN_RMAN_OSL_PARSER_RMAN_OSL_PARSER_H
#define EXT_RMANPKG_25_0_PLUGIN_RENDERMAN_PLUGIN_RMAN_OSL_PARSER_RMAN_OSL_PARSER_H

/// \file rmanOslParser/rmanOslParser.h

#include "pxr/pxr.h"
#include "pxr/base/vt/value.h"
#if PXR_VERSION >= 2505
#include "pxr/usd/sdr/parserPlugin.h"
#else
#include "pxr/usd/ndr/parserPlugin.h"
#endif
#include "pxr/usd/sdr/declare.h"
#include "RixShaderInfo.h"

PXR_NAMESPACE_OPEN_SCOPE

// Forward declarations
#if PXR_VERSION >= 2505
class SdrShaderNode;
struct SdrShaderNodeDiscoveryResult;
#else
class NdrNode;
struct NdrNodeDiscoveryResult;
using SdrShaderNodeDiscoveryResult = NdrNodeDiscoveryResult;
using SdrParserPlugin = NdrParserPlugin;
using SdrTokenMap = NdrTokenMap;
using SdrTokenVec = NdrTokenVec;
#endif

/// \class RmanOslParserPlugin
///
/// Parses OSL nodes. For more information on parser plugins, see the
/// documentation for `SdrParserPlugin`.
///
/// \section metadata_schema Metadata Schema
/// The following metadata is pulled out of the shader and available on the
/// destination as outlined below.
///
/// \subsection node_metadata Node Metadata
///
/// OSL Metadata Key      | Destination
/// --------------------- | ------------
/// category              | GetCategory()
/// departments           | GetDepartments()
/// help                  | GetHelp()
/// label                 | GetLabel()
/// \ref pvars "primvars" | GetPrimvars(), GetAdditionalPrimvarProperties()
/// _everything else_     | GetMetadata()
///
///
/// \subsection property_metadata Property Metadata
///
/// OSL Metadata Key    | Destination
/// ------------------- | ------------
/// connectable         | IsConnectable()
/// sdrDefinitionName   | renames parameter, sends original osl param name to 
///                     | SdrShaderProperty::GetImplementationName()
/// page                | GetPage()
/// help                | GetHelp()
/// label               | GetLabel()
/// isDynamicArray      | IsDynamicArray()
/// \ref opts "options" | GetOptions()
/// vstructMemberName   | GetVStructMemberName()
/// vstructMemberOf     | GetVStructMemberOf()
/// _everything else_   | GetMetadata()
///
///
/// \subsection opts Options Format
/// Multiple options are separated with the pipe '|' character; options can be
/// specified in one of two ways:
/// <ul>
///     <li>
///         key1|key2|key3|...
///     </li>
///     <li>
///         key1:value1|key2:value2|...
///     </li>
/// </ul>
///
/// In the first case, where the values are not specified, the value of the
/// option will be available on the node as an empty string.
///
///
/// \subsection pvars Primvar Format
/// Multiple primvars are separated with the pipe '|' character; primvars can be
/// specified in one of two ways:
/// <ul>
///     <li>
///         A string _not_ prefixed with a '$'; this indicates a normal primvar
///     </li>
///     <li>
///         A string prefixed with '$'; the part after the '$' indicates the
///         name of a string-typed input on the node whose value contains
///         additional primvars
///     </li>
/// </ul>
///
class RmanOslParserPlugin : public SdrParserPlugin
{
public:
    RmanOslParserPlugin();
    ~RmanOslParserPlugin();

#if PXR_VERSION >= 2505
    SdrShaderNodeUniquePtr ParseShaderNode(
        const SdrShaderNodeDiscoveryResult& discoveryResult) override;
#else
    NdrNodeUniquePtr Parse(const NdrNodeDiscoveryResult& discoveryResult) 
        override;
#endif

    const SdrTokenVec& GetDiscoveryTypes() const override;

    const TfToken& GetSourceType() const override;

private:
    // Determines the sdrContext for the shader from the schema base defined
    // in the node's metadata
    TfToken _getSdrContextFromSchemaBase(const SdrTokenMap& metadata) const;

    // Gets a vector of properties that are present on the specified OSL
    // query object
#if PXR_VERSION >= 2505
    SdrShaderPropertyUniquePtrVec _getNodeProperties(
#else
    NdrPropertyUniquePtrVec _getNodeProperties(
#endif
        const RixShaderQuery* sq,
        const SdrShaderNodeDiscoveryResult& discoveryResult, 
        const std::string& fallbackPrefix) const;

    // Gets all metadata for the node that's present on the specified OSL
    // query object
    SdrTokenMap _getNodeMetadata(
        const RixShaderQuery* sq,
        const SdrTokenMap &baseMetadata) const;

    // Gets all metadata for the specified OSL parameter
    SdrTokenMap _getPropertyMetadata(const RixShaderParameter* param,
        const SdrShaderNodeDiscoveryResult& discoveryResult) const;

    // Injects any metadata that is generated by the parser. This type of
    // metadata provides additional hints to the property.
    void _injectParserMetadata(SdrTokenMap& metadata,
        const TfToken& typeName) const;

    // Gets a common type + array size (if array) from the OSL parameter
    std::tuple<TfToken, size_t> _getTypeName(
        const RixShaderParameter* param,
        const SdrTokenMap& metadata) const;

    // Gets the default value of the specified param.
    VtValue _getDefaultValue(
        const RixShaderParameter* param,
        const std::string& oslType,
        size_t arraySize,
        const SdrTokenMap& metadata
    ) const;
};

PXR_NAMESPACE_CLOSE_SCOPE

#endif // EXT_RMANPKG_25_0_PLUGIN_RENDERMAN_PLUGIN_RMAN_OSL_PARSER_RMAN_OSL_PARSER_H
