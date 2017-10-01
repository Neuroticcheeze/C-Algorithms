#include "raytrace.hpp"
#include "screen.hpp"
#include <vector>

std::vector<geometry*> gscene;
unsigned maxBounceCount = 0;
unsigned avgSampleCount = 0;

camera::camera(const vec3 &pos, const vec3 &dir, float fov, float near_, float far_)
{
	this->pos = pos;
	this->dir = normalize(dir);
	this->fov = fov;
	this->near__ = near_;
	this->far__ = far_;

	vec2 dims = (vec2)getScreenDims();

	this->invProj = inverse(
		perspective(fov, dims.y / dims.x, near_, far_) *
		lookAt(vec3(0.0F), this->dir, vec3(0.0F, 1.0F, 0.0F))
	);
}

ray camera::project(unsigned x, unsigned y)
{
	vec2 ndc = (vec2((float)x, (float)y) / (vec2)getScreenDims()) * 2.0F - 1.0F;

	vec4 ndc4_ = invProj * vec4(ndc, 1.0F, 1.0F);
	ray newRay;
	newRay.pos = pos;
	newRay.dir = normalize((vec3)ndc4_);
	return newRay;
}

void addGeometry(geometry *geom)
{
	gscene.push_back(geom);
}

bool testScene(const ray &ray_, const geometry *previousCollision, hit_info &result)
{
	bool b = false;
	hit_info r;

	float small_t = FLT_MAX;

	for (unsigned index = 0; index < gscene.size(); ++index)
	{
		if (
			(!previousCollision || (previousCollision != gscene[index])) &&
			gscene[index]->test(ray_, r)
			)
		{
			if (r.incidence_length < small_t)
			{
				small_t = r.incidence_length;
				result = r;
				b = true;
				result.potentialHitGeom = gscene[index];

				switch (result.potentialHitGeom->_material._type)
				{
				case material::metal:
					result.rayTerm = hit_info::reflective_hit;
					break;
				case material::glowy:
					result.rayTerm = hit_info::emissive_hit;
					break;
				default:
					break;
				}
			}
		}
	}

	if (!b)
		result.rayTerm = hit_info::background_hit;

	return b;
}

geometry *getGeometry(int index)
{
	return gscene[index];
}

void initialiseRaytracer(unsigned bounces, unsigned samples)
{
	maxBounceCount = bounces;
	avgSampleCount = samples;
}

//Create a sampling ray based on the orig-geom's brdf TODO. (For now just does a perfect mirror
void __MonteCarlo_BuildRay(ray *out_ray_, const ray_tree_node *prev_)
{
	out_ray_->pos = prev_->inf.hit_position;

	printf("__MonteCarlo_BuildRay [%f, %f, %f]\n", out_ray_->pos.x, out_ray_->pos.y, out_ray_->pos.z);
	
	glm::vec3 randVec = glm::ballRand(1.0F);
	randVec.y = glm::abs(randVec.y);

	//glm::vec3 a, b, v;

	//a = randVec;
	//b = prev_->inf.hit_normal;

	//v = glm::cross(a, b);

	glm::quat t = glm::rotation(vec3(0, 1, 0), prev_->inf.hit_normal);
	randVec = t * randVec;

	out_ray_->dir = glm::normalize(-glm::reflect(-prev_->incident.dir, prev_->inf.hit_normal) + glm::ballRand(0.5F));
	//out_ray_->dir = glm::normalize(randVec);
}

void __BuildSample(const ray_tree_node *prev_, ray_tree_node *next_, bool &hitSky)
{
	//Create a sampling ray based on the orig-geom's brdf
	__MonteCarlo_BuildRay(&next_->incident, prev_);
	
	//Test against the scene
	hitSky = !testScene(next_->incident, prev_->inf.potentialHitGeom, next_->inf);
}

void __BuildRow(std::vector<ray_tree_node*> &prev, std::vector<ray_tree_node*> &next, std::vector<uvec2> &__sr, int t)
{
	int incCount = prev.size();

	//As big as there are incidence rays on the previous row.
	__sr.resize(incCount);

	//For each most recent incidence ray
	for (int inc_ = 0; inc_ < incCount; ++inc_)
	{
		ray_tree_node *prevNode = prev[inc_];
		ray& incident = prevNode->incident;
		
		//BRDF specifics of prevNode
		int brdfSampleCount;

		if (prevNode->inf.rayTerm == hit_info::reflective_hit)
			brdfSampleCount = glm::pow(avgSampleCount, 1.0F / (2.0F *  (float)t / maxBounceCount));//TODO: Fix this to actually be adaptive to the brdf on geom of prevNode
		else
			brdfSampleCount = 0;

		//Set the sample range
		uvec2 &sr = __sr[inc_];
		sr.x = next.size();		// start
		sr.y = brdfSampleCount;	// count

		//For each sample
		for (int smp_ = 0; smp_ < brdfSampleCount; ++smp_)
		{
			//Create a new node
			ray_tree_node *node = new ray_tree_node;
			node->prev = inc_;

			//Fill the node
			static bool hitSky = false;
			__BuildSample(prevNode, node, hitSky);

			//Finally add to the bounce row
			next.push_back(node);
		}
	}
}

bool buildRayTree(const ray &initialCameraRay, ray_tree *rayTree)
{
	rayTree->bounce = maxBounceCount;

	//Step 1: Initialise fields in te ray tree
	rayTree->data = new std::vector<ray_tree_node*>[maxBounceCount + 1];
	rayTree->sampleRanges = new std::vector<uvec2>[maxBounceCount + 1];//The first row is never used since that is the camera's row and no scattering has occurred yet.

	bool initCamTestResult;

	//Step 2: Initial scene sample from the camera
	{
		ray_tree_node *node = new ray_tree_node; 
		node->incident = initialCameraRay;
		node->prev = -1;
		initCamTestResult = testScene(initialCameraRay, nullptr, node->inf);

		rayTree->sampleRanges[0].push_back(uvec2(0, 1));
		rayTree->data[0].push_back(node);
	}

	if (!initCamTestResult)
		return true;

	//Step 3: Recursively follow the other light paths.
	int bouncelvl__ = 1;
	while (bouncelvl__ < (maxBounceCount + 1))
	{
		__BuildRow(rayTree->data[bouncelvl__ - 1], rayTree->data[bouncelvl__], rayTree->sampleRanges[bouncelvl__], bouncelvl__);
		++bouncelvl__;
	}
	
	return true;
}

//if (rayTree->sampleRanges[bouncelvl__].size() > 0)printf("bounce multi sample count = %d\n", rayTree->sampleRanges[bouncelvl__][0].y);
vec4 evaluateRayTree(ray_tree *rayTree)
{
	geometry *directHit = rayTree->data[0][0]->inf.potentialHitGeom;

	const vec4 gbackground = vec4(0.1F);

	if (!directHit)
		return gbackground;

	if (directHit && directHit->_material._type == material::glowy)
		return (vec4)directHit->_material._col;

	std::vector<_colour> *colourEvaluation = new std::vector<_colour>[maxBounceCount + 1];

	int bouncelvl__ = maxBounceCount;
	while (bouncelvl__ > 0)
	{
		//Short-hands
		std::vector<_colour> *colEvalRow = colourEvaluation + bouncelvl__;
		std::vector<uvec2> *sampRanRow = rayTree->sampleRanges + bouncelvl__;
		int rowSize__ = sampRanRow->size();

		//Resize the row to the same as the sampleRanges size at the same row. So we can fit all the colours in
		colEvalRow->resize(rowSize__);

		//For each of the sample ranges
		for (int sampRanInd_ = 0; sampRanInd_ < rowSize__; ++sampRanInd_)
		{
			_colour &col = colEvalRow->at(sampRanInd_);
			uvec2 &range = sampRanRow->at(sampRanInd_);

			//Iterate over the sample range for the current row
			for (int nodeInd_ = range.x; nodeInd_ < (range.x + range.y); ++nodeInd_)
			{
				//Short-hands
				ray_tree_node *node = rayTree->data[bouncelvl__][nodeInd_];
				geometry *potHitGeom = node->inf.potentialHitGeom;

				_colour colour__(0.0F);

				bool outOfBounces = (bouncelvl__ == maxBounceCount);
				bool terminatedOnBackground = potHitGeom == nullptr; 

				//If the lightpath terminated on the background/skybox (aka, there is no geometry recorded in the hit information for the node)
				if (terminatedOnBackground)
				{
					colour__ = gbackground;//ambient colour TODO: make this controllable & make a cpu cubemap sampler
				}

				//Else the lightpath hit something in the scene
				else
				{

					//If the lightpath can't scatter from the geometry since we ran out of bounces
					if (outOfBounces)
					{
						//Since we can't sample further, set to the material's colour
						//colour__ = (vec4)potHitGeom->_material._col;
					}

					//Else this node has children (samples) in the row below it.
					else
					{
						//We hit geometry and there are samples to evaluate...

						_colour matCol = (vec4)potHitGeom->_material._col;
						
						const _colour &samps = colourEvaluation[bouncelvl__][nodeInd_];
						
						_colour sampleCol = samps * samps.a;
						
						colour__ = samps * matCol;
					}
				}

				col += colour__;
			}

			//Complete the average colour calculation.
			col /= range.y;

			//Point the parent node to this new evaluated colour.
			int sampleRangeParentIndex = rayTree->data[bouncelvl__][range.x]->prev;
			rayTree->data[bouncelvl__ - 1][sampleRangeParentIndex]->evalColourPtr = sampRanInd_;
		}

		--bouncelvl__;
	}

	//Final average of the first bounce row and return the result.
	vec4 finalCol = vec4(0.0F);
	auto &colEvalRow = colourEvaluation[1];
	for (int t = 0; t < colEvalRow.size(); ++t)
		finalCol += colEvalRow[t];

	finalCol += (_colour)directHit->_material._col;

	finalCol /= colEvalRow.size() + 1;

	delete[] colourEvaluation;

	return finalCol;

	//ray_tree_node *node = rayTree->data[0][0];
	//if (node->inf.potentialHitGeom) {
	//	return (vec4)node->inf.potentialHitGeom->_material._col * (node->inf.potentialHitGeom->_material._type == material::glowy ? 1.0F : glm::pow(glm::dot(-node->incident.dir, node->inf.hit_normal), 2.0F));
	//}
	//return vec4(0.1F, 0.1F, 0.1F, 1.0F);
}

void deleteRayTree(ray_tree **tree)
{
	ray_tree *tree_ = *tree;

	for (int t = 0; t < tree_->bounce + 1; ++t)
	{
		for each (ray_tree_node *node in tree_->data[t])
		{
			delete node;
		}
	}

	delete[] tree_->data;
	delete[] tree_->sampleRanges;

	delete *tree;
	tree = nullptr;
}

void finaliseRaytracer()
{
	for each (const geometry * geo in gscene)
	{
		delete geo;
	}

	gscene.clear();
}

#include "..\Gizmos.h"
#include "my_geometries.hpp"
void drawSceneGeometryAndCamera(const camera &camera)
{
	for each (const geometry * geo in gscene)
	{
		vec4 mat;
		mat = (vec4)geo->_material._col + vec4(0.0F, 0.0F, 0.0F, 1.0F);

		const sphere *s = nullptr;
		const triangle *t = nullptr;
		const plane *p = nullptr;

		switch(geo->type_)
		{
		case geometry::sphere:
			s = (const sphere *)geo;
			Gizmos::addSphere(s->pos, s->radius, 20, 40, mat);
			break;

		case geometry::triangle:
			t = (const triangle *)geo;
			Gizmos::addTri(t->vert0.pos, t->vert1.pos, t->vert2.pos, mat);
			break;

		case geometry::plane:
			const plane *p = (const plane *)geo;
			mat4 m__;
			//m__ = glm::lookAt();
			Gizmos::addDisk(vec3(p->pos), 100.0F, 4, mat, &m__);
			break;
		}
	}

	vec2 dims = (vec2)getScreenDims();
	Gizmos::addFrustum(camera.pos, camera.dir, camera.fov, dims.y / dims.x, camera.near__, camera.far__, glm::vec4(1, 1, 1, 1));
}

void drawRayTree(const ray_tree *rayTree)
{
	for (int t = 0; t < rayTree->bounce + 1; ++t)
	{
		for each (ray_tree_node *node in rayTree->data[t])
		{
			const ray *r = &node->incident;

			if (node->inf.rayTerm != hit_info::background_hit)
				Gizmos::addLine(r->pos, node->inf.hit_position, 
					vec4(1.0F, 1.0F, 1.0F, 1.0F), t == 0 ? vec4(1, 0, 0, 1) : vec4(0.0F, 1.0F, 0.0F, 1.0F));

			else
				Gizmos::addLine(r->pos, r->pos + r->dir * 8.0F, 
					vec4(1.0F, 1.0F, 1.0F, 1.0F), t == 0 ? vec4(1, 0, 0, 1) : vec4(0.85F, 0.5F, 1.0F, 0.0F));
		}
	}
}