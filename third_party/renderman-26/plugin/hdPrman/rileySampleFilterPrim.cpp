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

#include "hdPrman/rileySampleFilterPrim.h"

#ifdef HDPRMAN_USE_SCENE_INDEX_OBSERVER

#include "hdPrman/rileyIds.h"
#include "hdPrman/rileyTypes.h"

#include "hdPrman/rileySampleFilterSchema.h"
#include "hdPrman/utils.h"

#include "pxr/imaging/hd/sceneIndex.h"

PXR_NAMESPACE_OPEN_SCOPE

HdPrman_RileySampleFilterPrim::HdPrman_RileySampleFilterPrim(
    HdContainerDataSourceHandle const &primSource,
    const HdsiPrimManagingSceneIndexObserver * const observer,
    HdPrman_RenderParam * const renderParam)
  : HdPrman_RileyPrimBase(renderParam)
{
    HdPrmanRileySampleFilterSchema schema =
        HdPrmanRileySampleFilterSchema::GetFromParent(primSource);


    HdPrman_RileyShadingNetwork sampleFilter =
        HdPrman_RileyShadingNetwork(
            schema.GetSampleFilter());

    HdPrman_RileyParamList attributes =
        HdPrman_RileyParamList(
            schema.GetAttributes());


    _rileyId = _AcquireRiley()->CreateSampleFilter(
        riley::UserId(),

        sampleFilter.rileyObject,
        attributes.rileyObject);

// --(BEGIN CUSTOM CODE: Constructor)--
// --(END CUSTOM CODE: Constructor)--
}

void
HdPrman_RileySampleFilterPrim::_Dirty(
    const HdSceneIndexObserver::DirtiedPrimEntry &entry,
    const HdsiPrimManagingSceneIndexObserver * const observer)
{
    HdPrmanRileySampleFilterSchema schema =
        HdPrmanRileySampleFilterSchema::GetFromParent(
            observer->GetSceneIndex()->GetPrim(entry.primPath).dataSource);


    std::optional<HdPrman_RileyShadingNetwork> sampleFilter;
    if (entry.dirtyLocators.Intersects(
            HdPrmanRileySampleFilterSchema::GetSampleFilterLocator())) {
        sampleFilter =
            HdPrman_RileyShadingNetwork(
                schema.GetSampleFilter());
    };

    std::optional<HdPrman_RileyParamList> attributes;
    if (entry.dirtyLocators.Intersects(
            HdPrmanRileySampleFilterSchema::GetAttributesLocator())) {
        attributes =
            HdPrman_RileyParamList(
                schema.GetAttributes());
    };


    _AcquireRiley()->ModifySampleFilter(_rileyId,
        HdPrman_GetRileyObjectPtr(sampleFilter),
        HdPrman_GetRileyObjectPtr(attributes)
// --(BEGIN CUSTOM CODE: Modify)--
// --(END CUSTOM CODE: Modify)--
        );


}

HdPrman_RileySampleFilterPrim::~HdPrman_RileySampleFilterPrim()
{
    _AcquireRiley()->DeleteSampleFilter(_rileyId
// --(BEGIN CUSTOM CODE: Delete)--
// --(END CUSTOM CODE: Delete)--
        );

}

PXR_NAMESPACE_CLOSE_SCOPE

#endif // #ifdef HDPRMAN_USE_SCENE_INDEX_OBSERVER