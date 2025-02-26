//
// Copyright 2021 Pixar
//
// Licensed under the terms set forth in the LICENSE.txt file available at
// https://openusd.org/license.
//
#include "pxr/imaging/hd/materialNetwork2Interface.h"

PXR_NAMESPACE_OPEN_SCOPE

TF_DEFINE_PRIVATE_TOKENS(
    _tokens,
    (colorSpace)
    (typeName)
);

HdMaterialNode2 *
HdMaterialNetwork2Interface::_GetNode(const TfToken &nodeName) const
{
    if (!_materialNetwork) {
        return nullptr;
    }

    if (nodeName == _lastAccessedNodeName) {
        return _lastAccessedNode;
    }

    _lastAccessedNodeName = nodeName;

    std::map<SdfPath, HdMaterialNode2>::iterator it =
        _materialNetwork->nodes.find(SdfPath(nodeName.data()));


    if (it == _materialNetwork->nodes.end()) {
        _lastAccessedNode  = nullptr;
    } else {
        _lastAccessedNode = &(it->second);
    }

    return _lastAccessedNode;
}

HdMaterialNode2 *
HdMaterialNetwork2Interface::_GetOrCreateNode(const TfToken &nodeName)
    const
{
    HdMaterialNode2 *result = _GetNode(nodeName);
    if (result) {
        return result;
    }

    if (!_materialNetwork) {
        return result;
    }

    _lastAccessedNode = &_materialNetwork->nodes[SdfPath(nodeName.data())];
    return  _lastAccessedNode;
}

TfTokenVector
_GetKeysFromVtDictionary(const VtDictionary& dict, const std::string prefix="")
{
    TfTokenVector keys;
    for (const auto& it : dict) {

        const std::string key =
            prefix.empty() 
                ? it.first 
                : prefix + ":" + it.first;
        keys.emplace_back(TfToken(key));

        if (it.second.IsHolding<VtDictionary>()) {
            TfTokenVector subKeys =
                _GetKeysFromVtDictionary(
                    it.second.UncheckedGet<VtDictionary>(), key);
            keys.insert(keys.end(), subKeys.begin(), subKeys.end());
        }
    }
    return keys;
}

TfTokenVector
HdMaterialNetwork2Interface::GetMaterialConfigKeys() const
{
    return _GetKeysFromVtDictionary(_materialNetwork->config);
}

VtValue
HdMaterialNetwork2Interface::GetMaterialConfigValue(const TfToken& key) const
{
    const auto it = _materialNetwork->config.find(key.GetString());
    if (it != _materialNetwork->config.end()) {
        return it->second;
    }
    return VtValue();
}

TfTokenVector
HdMaterialNetwork2Interface::GetNodeNames() const
{
    TfTokenVector result;
    if (_materialNetwork) {
        result.reserve(_materialNetwork->nodes.size());
        for (const auto& nameNodePair : _materialNetwork->nodes) {
            result.push_back(TfToken(nameNodePair.first.GetString()));
        }
    }

    return result;
}

TfToken
HdMaterialNetwork2Interface::GetNodeType(const TfToken &nodeName) const
{
    if (HdMaterialNode2 *node = _GetNode(nodeName)) {
        return node->nodeTypeId;
    }

    return TfToken();
}

TfTokenVector
HdMaterialNetwork2Interface::GetNodeTypeInfoKeys(const TfToken& nodeName) const
{
    return {};
}

VtValue
HdMaterialNetwork2Interface::GetNodeTypeInfoValue(
    const TfToken& nodeName, const TfToken& key) const
{
    return VtValue();
}

TfTokenVector
HdMaterialNetwork2Interface::GetAuthoredNodeParameterNames(
    const TfToken &nodeName) const
{
    TfTokenVector result;
    if (HdMaterialNode2 *node = _GetNode(nodeName)) {
        result.reserve(node->parameters.size());
        for (const auto& nameValuePair : node->parameters) {
            result.push_back(nameValuePair.first);
        }
    }
    return result;
}

VtValue
HdMaterialNetwork2Interface::GetNodeParameterValue(
    const TfToken &nodeName,
    const TfToken &paramName) const
{
    if (HdMaterialNode2 *node = _GetNode(nodeName)) {
        const auto it = node->parameters.find(paramName);
        if (it != node->parameters.end()) {
            return it->second;
        }
    }

    return VtValue();
}

HdMaterialNetworkInterface::NodeParamData
HdMaterialNetwork2Interface::GetNodeParameterData(
    const TfToken &nodeName,
    const TfToken &paramName) const
{
    HdMaterialNetworkInterface::NodeParamData paramData;
    if (HdMaterialNode2 *node = _GetNode(nodeName)) {
        // Value
        const auto vIt = node->parameters.find(paramName);
        if (vIt != node->parameters.end()) {
            paramData.value = vIt->second;
        }
        // ColorSpace
        const TfToken colorSpaceParamName(SdfPath::JoinIdentifier(
            _tokens->colorSpace, paramName));
        const auto csIt = node->parameters.find(colorSpaceParamName);
        if (csIt != node->parameters.end()) {
            paramData.colorSpace = csIt->second.Get<TfToken>();
        }
        // TypeName
        const TfToken typeNameParamName(SdfPath::JoinIdentifier(
            _tokens->typeName, paramName));
        const auto tnIt = node->parameters.find(typeNameParamName);
        if (tnIt != node->parameters.end()) {
            paramData.typeName = tnIt->second.Get<TfToken>();
        }
    }
    return paramData;
}

TfTokenVector
HdMaterialNetwork2Interface::GetNodeInputConnectionNames(
    const TfToken &nodeName) const
{
    TfTokenVector result;
    if (HdMaterialNode2 *node = _GetNode(nodeName)) {
        result.reserve(node->inputConnections.size());
        for (const auto& nameConnectionsPair : node->inputConnections) {
            result.push_back(nameConnectionsPair.first);
        }
    }
    return result;
}

HdMaterialNetworkInterface::InputConnectionVector
HdMaterialNetwork2Interface::GetNodeInputConnection(
    const TfToken &nodeName,
    const TfToken &inputName) const
{
    InputConnectionVector result;
    if (HdMaterialNode2 *node = _GetNode(nodeName)) {
        const auto it = node->inputConnections.find(inputName);
        if (it != node->inputConnections.end()) {
            result.reserve(it->second.size());
            for (const HdMaterialConnection2 &c : it->second) {
                result.push_back({TfToken(c.upstreamNode.GetString()),
                    c.upstreamOutputName});
            }
        }
    }
    return result;
}

void
HdMaterialNetwork2Interface::DeleteNode(const TfToken &nodeName)
{
    if (_materialNetwork) {
        _materialNetwork->nodes.erase(SdfPath(nodeName.data()));
    }
}

void
HdMaterialNetwork2Interface::SetNodeType(
    const TfToken &nodeName,
    const TfToken &nodeType)
{
    if (HdMaterialNode2 *node = _GetOrCreateNode(nodeName)) {
        node->nodeTypeId = nodeType;
    }
}

void
HdMaterialNetwork2Interface::SetNodeTypeInfoValue(
    const TfToken &nodeName,
    const TfToken &key,
    const VtValue &value)
{
    // no-op
}

void
HdMaterialNetwork2Interface::SetNodeParameterValue(
    const TfToken &nodeName,
    const TfToken &paramName,
    const VtValue &value)
{
    if (HdMaterialNode2 *node = _GetOrCreateNode(nodeName)) {
        node->parameters[paramName] = value;
    }
}

void
HdMaterialNetwork2Interface::SetNodeParameterData(
    const TfToken &nodeName,
    const TfToken &paramName,
    const NodeParamData &paramData)
{
    if (HdMaterialNode2 *node = _GetOrCreateNode(nodeName)) {
        // Value
        node->parameters[paramName] = paramData.value;

        // ColorSpace
        if (!paramData.colorSpace.IsEmpty()) {
            const TfToken csParamName(
                SdfPath::JoinIdentifier(_tokens->colorSpace, paramName));
            node->parameters[csParamName] = VtValue(paramData.colorSpace);
        }
        // TypeName
        if (!paramData.typeName.IsEmpty()) {
            const TfToken tnParamName(
                SdfPath::JoinIdentifier(_tokens->typeName, paramName));
            node->parameters[tnParamName] = VtValue(paramData.typeName);
        }
    }
}

void
HdMaterialNetwork2Interface::DeleteNodeParameter(
    const TfToken &nodeName,
    const TfToken &paramName)
{
    if (HdMaterialNode2 *node = _GetNode(nodeName)) {
        node->parameters.erase(paramName);

        // ColorSpace
        const TfToken csParamName(
            SdfPath::JoinIdentifier(_tokens->colorSpace, paramName));
        node->parameters.erase(csParamName);
        // TypeName
        const TfToken tnParamName(
            SdfPath::JoinIdentifier(_tokens->typeName, paramName));
        node->parameters.erase(tnParamName);
    }
}

void
HdMaterialNetwork2Interface::SetNodeInputConnection(
    const TfToken &nodeName,
    const TfToken &inputName,
    const InputConnectionVector &connections)
{
    if (HdMaterialNode2 *node = _GetOrCreateNode(nodeName)) {
        std::vector<HdMaterialConnection2> connections2;
        connections2.reserve(connections.size());
        for (const InputConnection &c : connections) {
            connections2.push_back({SdfPath(c.upstreamNodeName.GetString()),
                c.upstreamOutputName});
        }
        node->inputConnections[inputName] = connections2;
    }
}

void
HdMaterialNetwork2Interface::DeleteNodeInputConnection(
    const TfToken &nodeName,
    const TfToken &inputName)
{
    if (HdMaterialNode2 *node = _GetNode(nodeName)) {
        node->inputConnections.erase(inputName);
    }
}

TfTokenVector
HdMaterialNetwork2Interface::GetTerminalNames() const
{
    TfTokenVector result;
    if (_materialNetwork) {
        result.reserve(_materialNetwork->terminals.size());
        for (const auto& nameConnectionPair : _materialNetwork->terminals) {
            result.push_back(nameConnectionPair.first);
        }
    }

    return result;
}

HdMaterialNetwork2Interface::InputConnectionResult
HdMaterialNetwork2Interface::GetTerminalConnection(
    const TfToken &terminalName) const
{
    if (_materialNetwork) {
        const auto it = _materialNetwork->terminals.find(terminalName);
        if (it != _materialNetwork->terminals.end()) {
            const HdMaterialConnection2 &c = it->second;
            return InputConnectionResult(true,
                                        {c.upstreamNode.GetAsToken(),
                                         c.upstreamOutputName});
        }
    }
    return InputConnectionResult(false, InputConnection());
}

void
HdMaterialNetwork2Interface::DeleteTerminal(
    const TfToken &terminalName)
{
    if (_materialNetwork) {
        _materialNetwork->terminals.erase(terminalName);
    }
}

void
HdMaterialNetwork2Interface::SetTerminalConnection(
    const TfToken &terminalName,
    const InputConnection &connection)
{
    if (_materialNetwork) {
        _materialNetwork->terminals[terminalName] =
            { SdfPath(connection.upstreamNodeName.GetString()),
              connection.upstreamOutputName };
    }
}

PXR_NAMESPACE_CLOSE_SCOPE
