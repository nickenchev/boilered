#include <fstream>
#include <sstream>
#include <unordered_map>

#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <vector>

#include "samplepart.h"
#include "SDL_keycode.h"
#include "input/inputevent.h"
#include "samplepart.h"
#include "core/entitycomponentsystem.h"
#include "video/texture.h"
#include "video/vertexdata.h"
#include "core/components/rendercomponent.h"
#include "core/components/positioncomponent.h"
#include "core/components/spritecomponent.h"
#include "core/components/lightingcomponent.h"
#include "video/imaging/imageloader.h"

using namespace Boiler;

auto SamplePart::loadPrimitive(const gltf::ModelAccessors &modelAccess, const gltf::Primitive &primitive)
{
	if (primitive.mode.has_value())
	{
		assert(primitive.mode == 4);
	}
	EntityComponentSystem &ecs = engine.getEcs();
	using namespace gltf::attributes;

	// get the primitive's position data
	std::vector<Vertex> vertices;
	auto positionAccess = modelAccess.getTypedAccessor<float, 3>(primitive, POSITION);
	for (auto values : positionAccess)
	{
		Vertex vertex({values[0], -values[1], values[2]});
		vertex.colour = {1, 1, 1, 1};
		vertices.push_back(vertex);
	}

	assert(primitive.attributes.find(NORMAL) != primitive.attributes.end());

	auto normalAccess = modelAccess.getTypedAccessor<float, 3>(primitive, NORMAL);
	unsigned int vertexIndex = 0;
	for (auto normal : normalAccess)
	{
		vertices[vertexIndex++].normal = {normal[0], normal[1], normal[2]};
	}

	// load the primitive indices
	std::vector<uint32_t> indices;
	if (primitive.indices.has_value())
	{
		const auto &indexAccessor = modelAccess.getModel().accessors[primitive.indices.value()];
		if (indexAccessor.componentType == 5123)
		{
			auto indexAccess = modelAccess.getTypedAccessor<unsigned short, 1>(primitive, primitive.indices.value());
			for (auto values : indexAccess)
			{
				indices.push_back(values[0]);
			}
		}
		else if (indexAccessor.componentType == 5125)
		{
			auto indexAccess = modelAccess.getTypedAccessor<unsigned int, 1>(primitive, primitive.indices.value());
			for (auto values : indexAccess)
			{
				indices.push_back(values[0]);
			}
		}
	}
	else
	{
		// generate simple indices
		for (long i = 0; i < vertices.size(); ++i)
		{
			indices.push_back(i);
		}
	}

	// load texture coordinates
	if (primitive.attributes.find(TEXCOORD_0) != primitive.attributes.end())
	{
		const auto &accessor = modelAccess.getModel().accessors[primitive.attributes.find(TEXCOORD_0)->second];
		auto texCoordAccess = modelAccess.getTypedAccessor<float, 2>(accessor);

		long vertexIdx = 0;
		for (auto values : texCoordAccess)
		{
			vertices[vertexIdx++].textureCoordinate = {values[0], values[1]};
		}
	}

	const VertexData vertData(vertices, indices);
	return engine.getRenderer().loadPrimitive(vertData);
}

Entity SamplePart::loadNode(const gltf::Model &model, const gltf::ModelAccessors &modelAccess, std::unordered_map<int, Entity> &nodeEntities, int nodeIndex)
{
	EntityComponentSystem &ecs = engine.getEcs();
	Entity nodeEntity = ecs.newEntity();

	const gltf::Node &node = model.nodes[nodeIndex];
	logger.log("Loading node: {}", node.name);

	if (node.mesh.has_value())
	{
		const auto &mesh = model.meshes[node.mesh.value()];
		logger.log("Loading mesh: {}", mesh.name);

		auto renderComp = ecs.createComponent<RenderComponent>(nodeEntity);
		for (auto &gltfPrimitive : mesh.primitives)
		{
			Primitive meshPrimitive = loadPrimitive(modelAccess, gltfPrimitive);
			// setup material if any
			if (gltfPrimitive.material.has_value())
			{
				const MaterialId materialId = materialIds[gltfPrimitive.material.value()];
				meshPrimitive.materialId = materialId;
			}
			renderComp->mesh.primitives.push_back(meshPrimitive);
		}
	}
	auto renderPos = ecs.createComponent<PositionComponent>(nodeEntity, Rect(0, 0, 0, 0));

	// decompose a matrix if available, otherwise try to load transformations directly
	if (node.matrix.has_value())
	{
		const auto &matrixArray = node.matrix.value();
		mat4 matrix = glm::make_mat4(matrixArray.data());
		vec3 scale, skew, position;
		glm::quat orientation;
		vec4 perspective;

		glm::decompose(matrix, scale, orientation, position, skew, perspective);
		renderPos->frame.position = position;
		renderPos->scale = scale;
		renderPos->orientation = glm::conjugate(orientation); // TODO: https://stackoverflow.com/questions/17918033/glm-decompose-mat4-into-translation-and-rotation
	}
	else
	{
		// otherwise load transformation directly
		if (node.scale.has_value())
		{
			renderPos->scale = {
				node.scale.value()[0],
				node.scale.value()[1],
				node.scale.value()[2]
			};
		}
		if (node.translation.has_value())
		{
			renderPos->frame.position = {
				node.translation.value()[0],
				-node.translation.value()[1],
				node.translation.value()[2]
			};
		}
		if (node.rotation.has_value())
		{
			renderPos->orientation.x = node.rotation.value()[0];
			renderPos->orientation.y = node.rotation.value()[1];
			renderPos->orientation.z = node.rotation.value()[2];
			renderPos->orientation.w = node.rotation.value()[3];
		}
	}

	// if this node has children, create them and assign the current node as parent
	if (node.children.size() > 0)
	{
		for (int childIndex : node.children)
		{
			Entity childEntity = loadNode(model, modelAccess, nodeEntities, childIndex);
			ecs.createComponent<ParentComponent>(childEntity, nodeEntity);
		}
	}

	return nodeEntity;
}

SamplePart::SamplePart(Engine &engine) : Part("Sample", engine), logger("Sample Part")
{
	moveLeft = false;
	moveRight = false;
	moveCloser = false;
	moveFurther = false;
	moveUp = false;
	moveDown = false;
	turnLeft = false;
	turnRight = false;
	lookUp = false;
	lookDown = false;

	camPosition = {0, 0, 10.0f};
	camDirection = {0, 0, -1.0f};
	camUp = {0, 1.0f, 0};
}

void SamplePart::onStart()
{
	engine.getRenderer().setClearColor({0, 0, 0});
	//std::string base = "/home/nenchev/Developer/projects/boiler-3d/data";
	std::string base = "data/glTF-Sample-Models-master/2.0";
	std::string modelName = "FlightHelmet";
	std::string modelPath = fmt::format("/{}/glTF/", modelName);
	std::string modelFile = fmt::format("{}.gltf", modelName);
	std::string bufferPath{base + modelPath};
	// load GLTF file
	std::string gltfFile{base + modelPath + modelFile};

	logger.log("Loading glTF: {}", gltfFile);

	EntityComponentSystem &ecs = engine.getEcs();
    
	std::ifstream infile(gltfFile);
	std::stringstream buffer;
	buffer << infile.rdbuf();
	const std::string jsonString = buffer.str();
	infile.close();

	auto model = Boiler::gltf::load(jsonString);

	// load all of the buffers
	std::vector<std::vector<std::byte>> buffers;
	for (const auto &buffer : model.buffers)
	{
		buffers.push_back(loadBuffer(bufferPath, buffer));
	}

	// load materials
	for (int i = 0; i < model.materials.size(); ++i)
	{
		Material newMaterial;
		const gltf::Material &material = model.materials[i];
		if (material.pbrMetallicRoughness.value().baseColorTexture.has_value())
		{
			const gltf::MaterialTexture &matTexture = material.pbrMetallicRoughness.value().baseColorTexture.value();
			const gltf::Texture &texture = model.textures[matTexture.index.value()];
			const gltf::Image &image = model.images[texture.source.value()];

			if (image.uri.length() > 0)
			{
				const std::string imagePath = base + modelPath + image.uri;
				const ImageData imageData = ImageLoader::load(imagePath);

				// load the texture into GPU mem
				auto texture = engine.getRenderer().loadTexture(imagePath, imageData);
				newMaterial.baseTexture = texture;
			}
		}
		if (material.alphaMode == "BLEND")
		{
			newMaterial.alphaMode = AlphaMode::BLEND;
		}
		else if (material.alphaMode == "MASK")
		{
			newMaterial.alphaMode = AlphaMode::MASK;
		}
		else
		{
			newMaterial.alphaMode = AlphaMode::OPAQUE;
		}
		newMaterial.color = vec4(1, 1, 1, 1);
		MaterialId materialId = engine.getRenderSystem().addMaterial(newMaterial);
		materialIds.push_back(materialId);
	}

	LightSource light1({0, 10, 0}, {1, 1, 1});
	Entity eLight1 = ecs.newEntity();
	auto lightComp = ecs.createComponent<LightingComponent>(eLight1, light1);

	// Model accessors which are used for typed access into buffers
	gltf::ModelAccessors modelAccess(model, std::move(buffers));

	// grab the default scene and load the node heirarchy
	const gltf::Scene &scene = model.scenes[model.scene];
	std::unordered_map<int, Entity> nodeEntities;
	for (auto &nodeIndex : scene.nodes)
	{
		loadNode(model, modelAccess, nodeEntities, nodeIndex);
	}

    auto keyListener = [this](const KeyInputEvent &event)
	{
		//EntityComponentSystem &ecs = engine.getEcs();

		if (event.keyCode == SDLK_a)
		{
			moveLeft = event.state == ButtonState::DOWN;
		}
		else if (event.keyCode == SDLK_d)
		{
			moveRight = event.state == ButtonState::DOWN;
		}
		else if (event.keyCode == SDLK_s)
		{
			moveCloser = event.state == ButtonState::DOWN;
		}
		else if (event.keyCode == SDLK_w)
		{
			moveFurther = event.state == ButtonState::DOWN;
		}
		else if (event.keyCode == SDLK_PAGEUP)
		{
			moveUp = event.state == ButtonState::DOWN;
		}
		else if (event.keyCode == SDLK_PAGEDOWN)
		{
			moveDown = event.state == ButtonState::DOWN;
		}
		else if (event.keyCode == SDLK_LEFT)
		{
			turnLeft = event.state == ButtonState::DOWN;
		}
		else if (event.keyCode == SDLK_RIGHT)
		{
			turnRight = event.state == ButtonState::DOWN;
		}
		else if (event.keyCode == SDLK_UP)
		{
			lookUp = event.state == ButtonState::DOWN;
		}
		else if (event.keyCode == SDLK_DOWN)
		{
			lookDown = event.state == ButtonState::DOWN;
		}
	};
	engine.addKeyInputListener(keyListener);
}

void SamplePart::update(double deltaTime)
{
	const float speed = 3.0f;
	EntityComponentSystem &ecs = engine.getEcs();
	if (moveLeft)
	{
		glm::vec3 moveAmount = glm::cross(camDirection, camUp);
		moveAmount *= deltaTime * speed;
		camPosition -= moveAmount;
	}
	else if (moveRight)
	{
		glm::vec3 moveAmount = glm::cross(camDirection, camUp);
		moveAmount *= deltaTime * speed;
		camPosition += moveAmount;
	}

	if (moveCloser)
	{
		glm::vec3 moveAmount = camDirection;
		moveAmount *= speed * deltaTime;
		camPosition -= moveAmount;
	}
	else if (moveFurther)
	{
		glm::vec3 moveAmount = camDirection;
		moveAmount *= speed * deltaTime;
		camPosition += moveAmount;
	}

	if (moveUp)
	{
		camPosition.y -= speed * deltaTime;
	}
	else if (moveDown)
	{
		camPosition.y += speed * deltaTime;
	}

	if (lookUp)
	{
		const glm::vec3 axis = glm::cross(camUp, camDirection);
		camDirection = glm::rotate(camDirection, static_cast<float>(speed * deltaTime), axis);
		camUp = glm::cross(camDirection, axis);
	}
	else if (lookDown)
	{
		const glm::vec3 axis = glm::cross(camUp, camDirection);
		camDirection = glm::rotate(camDirection, static_cast<float>(-speed * deltaTime), axis);
		camUp = glm::cross(camDirection, axis);
	}

	if (turnLeft)
	{
		camDirection = glm::rotate(camDirection, static_cast<float>(speed * deltaTime), camUp);
	}
	else if (turnRight)
	{
		camDirection = glm::rotate(camDirection, static_cast<float>(-speed * deltaTime), camUp);
	}

	glm::mat4 view = glm::lookAt(camPosition, camPosition + camDirection, glm::vec3(0.0f, 1.0f, 0.0f));
	engine.getRenderer().setViewMatrix(view);
	engine.getRenderer().setCameraPosition(camPosition);
}
