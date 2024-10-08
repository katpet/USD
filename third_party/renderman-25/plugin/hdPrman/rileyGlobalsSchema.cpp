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

#include "hdPrman/rileyGlobalsSchema.h"

#include "pxr/imaging/hd/retainedDataSource.h"

#include "pxr/base/trace/trace.h"

// --(BEGIN CUSTOM CODE: Includes)--
// --(END CUSTOM CODE: Includes)--

PXR_NAMESPACE_OPEN_SCOPE

TF_DEFINE_PUBLIC_TOKENS(HdPrmanRileyGlobalsSchemaTokens,
    HD_PRMAN_RILEY_GLOBALS_SCHEMA_TOKENS);

// --(BEGIN CUSTOM CODE: Schema Methods)--
// --(END CUSTOM CODE: Schema Methods)--

HdPrmanRileyParamListSchema
HdPrmanRileyGlobalsSchema::GetOptions()
#if HD_API_VERSION >= 66
                                            const
#else
                                                 
#endif
{
    return HdPrmanRileyParamListSchema(_GetTypedDataSource<HdContainerDataSource>(
        HdPrmanRileyGlobalsSchemaTokens->options));
}

/*static*/
HdContainerDataSourceHandle
HdPrmanRileyGlobalsSchema::BuildRetained(
        const HdContainerDataSourceHandle &options
)
{
    TfToken _names[1];
    HdDataSourceBaseHandle _values[1];

    size_t _count = 0;

    if (options) {
        _names[_count] = HdPrmanRileyGlobalsSchemaTokens->options;
        _values[_count++] = options;
    }
    return HdRetainedContainerDataSource::New(_count, _names, _values);
}

HdPrmanRileyGlobalsSchema::Builder &
HdPrmanRileyGlobalsSchema::Builder::SetOptions(
    const HdContainerDataSourceHandle &options)
{
    _options = options;
    return *this;
}

HdContainerDataSourceHandle
HdPrmanRileyGlobalsSchema::Builder::Build()
{
    return HdPrmanRileyGlobalsSchema::BuildRetained(
        _options
    );
}

/*static*/
HdPrmanRileyGlobalsSchema
HdPrmanRileyGlobalsSchema::GetFromParent(
        const HdContainerDataSourceHandle &fromParentContainer)
{
    return HdPrmanRileyGlobalsSchema(
        fromParentContainer
        ? HdContainerDataSource::Cast(fromParentContainer->Get(
                HdPrmanRileyGlobalsSchemaTokens->rileyGlobals))
        : nullptr);
}

/*static*/
const TfToken &
HdPrmanRileyGlobalsSchema::GetSchemaToken()
{
    return HdPrmanRileyGlobalsSchemaTokens->rileyGlobals;
}

/*static*/
const HdDataSourceLocator &
HdPrmanRileyGlobalsSchema::GetDefaultLocator()
{
    static const HdDataSourceLocator locator(GetSchemaToken());
    return locator;
}

/* static */
const HdDataSourceLocator &
HdPrmanRileyGlobalsSchema::GetOptionsLocator()
{
    static const HdDataSourceLocator locator =
        GetDefaultLocator().Append(
            HdPrmanRileyGlobalsSchemaTokens->options);
    return locator;
} 

PXR_NAMESPACE_CLOSE_SCOPE