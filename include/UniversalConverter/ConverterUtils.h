#pragma once

class Link;
class OMENull;
class OMEObject;

bool ProcessOMENull(Link* pLn,OMENull* pNullObj, OMEObject* pObj);
bool VerifyRelationship(Link* pLn);

