#ifndef SAMPLEPART_H
#define SAMPLEPART_H

#include "boiler.h"
#include "core/part.h"
#include "core/materialid.h"
#include "gltf.h"
#include "typedaccessor.h"
#include "modelaccessors.h"

class SamplePart : public Boiler::Part
{
	Boiler::Logger logger;
	std::vector<Boiler::Entity> objects;

	glm::vec3 camPosition;
	glm::vec3 camDirection;
	glm::vec3 camUp;

	std::vector<Boiler::MaterialId> materialIds;

	bool moveLeft, moveRight, moveCloser, moveFurther, moveUp, moveDown, turnLeft, turnRight, lookUp, lookDown;

	auto loadPrimitive(const Boiler::gltf::ModelAccessors &modelAccess, const Boiler::gltf::Primitive &primitive);
	Boiler::Entity loadNode(const Boiler::gltf::Model &model, const Boiler::gltf::ModelAccessors &modelAccess, std::unordered_map<int, Boiler::Entity> &nodeEntities, int nodeIndex);
	
public:
    SamplePart(Boiler::Engine &engine);

    void onStart() override;
    void update(double deltaTime) override;
};

#endif /* SAMPLEPART_H */
