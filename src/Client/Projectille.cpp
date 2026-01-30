#include "Projectille.h"
#include "Util.h"

Projectille::Projectille() : MultiEntity()
{
	SetMultiEntityType(MultiEntityType::PROJECTILLE);
	AddEntity(EntityType::PROJECTILLE);
}

Projectille::~Projectille()
{
}

void Projectille::Update()
{
	EntityWrapper* entity = GetEntity(EntityType::PROJECTILLE);
	DirectX::XMFLOAT3 currentDir = entity->GetCpuEntity()->transform.dir;
	currentDir.x += (m_gravityDir.x - currentDir.x) * m_gravityStrength * cpuTime.delta;
	currentDir.y += (m_gravityDir.y - currentDir.y) * m_gravityStrength * cpuTime.delta;
	currentDir.z += (m_gravityDir.z - currentDir.z) * m_gravityStrength * cpuTime.delta;
	Util::NormalizeXMFLOAT3(currentDir);
	entity->LookTo(currentDir);
	entity->Move(m_velocity * cpuTime.delta);
}
