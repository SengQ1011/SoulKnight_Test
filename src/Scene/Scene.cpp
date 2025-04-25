//
// Created by QuzzS on 2025/4/25.
//

#include "Scene/Scene.hpp"
#include "Override/nGameObject.hpp"

void Scene::FlushPendingObjectsToRendererAndCamera()
{
	for (const std::shared_ptr<nGameObject>& obj : m_PendingObjects) {
		if (!obj) continue;

		const auto renderer = m_Root;
		const auto camera = m_Camera;

		if (renderer && obj->GetDrawable()) renderer->AddChild(obj);
		if (camera) camera->AddChild(obj);
	}
	m_PendingObjects.clear();
}

