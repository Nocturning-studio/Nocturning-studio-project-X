///////////////////////////////////////////////////////////////
// DummyArtifact.cpp
// DummyArtefact - артефакт пустышка
///////////////////////////////////////////////////////////////

#include "DummyArtifact.h"
#include "PhysicsShell.h"
#include "stdafx.h"

CDummyArtefact::CDummyArtefact(void)
{
}

CDummyArtefact::~CDummyArtefact(void)
{
}

void CDummyArtefact::Load(LPCSTR section)
{
    inherited::Load(section);
}
