//
// Copyright 2025 Pixar
//
// Licensed under the terms set forth in the LICENSE.txt file available at
// https://openusd.org/license.
#include "nodeIdentifierResolvingSceneIndexPlugin.h"

#include "pxr/imaging/hdsi/nodeIdentifierResolvingSceneIndex.h"

#include "pxr/imaging/hd/sceneIndexPluginRegistry.h"

PXR_NAMESPACE_OPEN_SCOPE

TF_DEFINE_PRIVATE_TOKENS(
    _tokens,
    (glslfx)
    ((sceneIndexPluginName, "HdSt_NodeIdentifierResolvingSceneIndexPlugin"))
);

static const char* const _pluginDisplayName = "GL";

TF_REGISTRY_FUNCTION(TfType)
{
    HdSceneIndexPluginRegistry::Define<
        HdSt_NodeIdentifierResolvingSceneIndexPlugin>();
}

TF_REGISTRY_FUNCTION(HdSceneIndexPlugin)
{
    const HdSceneIndexPluginRegistry::InsertionPhase insertionPhase = 0;

    HdSceneIndexPluginRegistry::GetInstance().RegisterSceneIndexForRenderer(
        _pluginDisplayName, _tokens->sceneIndexPluginName, nullptr,
        insertionPhase, HdSceneIndexPluginRegistry::InsertionOrderAtStart);
}

HdSt_NodeIdentifierResolvingSceneIndexPlugin::
    HdSt_NodeIdentifierResolvingSceneIndexPlugin()
    = default;

HdSt_NodeIdentifierResolvingSceneIndexPlugin::
    ~HdSt_NodeIdentifierResolvingSceneIndexPlugin()
    = default;

HdSceneIndexBaseRefPtr
HdSt_NodeIdentifierResolvingSceneIndexPlugin::_AppendSceneIndex(
    const HdSceneIndexBaseRefPtr& inputSceneIndex,
    const HdContainerDataSourceHandle& inputArgs)
{
    TF_UNUSED(inputArgs);
    return HdSiNodeIdentifierResolvingSceneIndex::New(inputSceneIndex, 
                                             /* sourceType */_tokens->glslfx);
}

PXR_NAMESPACE_CLOSE_SCOPE
