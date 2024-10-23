#pragma once
#include <fstream>
#include "Maths.h"
#include "DataTypes.h"

namespace dae
{
	namespace GeometryUtils
	{
#pragma region Sphere HitTest
		//SPHERE HIT-TESTS
		inline bool HitTest_Sphere(const Sphere& sphere, const Ray& ray, HitRecord& hitRecord, bool ignoreHitRecord = false)
		{
			Vector3 rayToSphere{ ray.origin - sphere.origin };

			// all part of Qaudratic equation
			const float RayDirectionDot{ Vector3::Dot(ray.direction, ray.direction) }; //A
			const float rayToSphereDotDirection{ Vector3::Dot(2 * ray.direction, rayToSphere) }; //B
			const float rayTosphereDot{ Vector3::Dot(rayToSphere, rayToSphere) - Square(sphere.radius) }; //C
			const float discriminant{ Square(rayToSphereDotDirection) - 4 * RayDirectionDot * rayTosphereDot }; //B^2 - 4AC

			if (discriminant < 0)
			{
				//cant intersect because it will become imaginary number
				return false;
			}

			const float sqrtDiscriminant{ sqrt(discriminant) };

			const float t1{ (-rayToSphereDotDirection + sqrtDiscriminant) / (2.f * RayDirectionDot) };
			const float t2{ (-rayToSphereDotDirection - sqrtDiscriminant) / (2.f * RayDirectionDot) };

			float t{};

			//if t1 is bigger than ray.min take t1, if not take ray.max + 1, same for t2, than take the smallest one of the 2
			t = fmin(t1 > ray.min ? t1 : ray.max + 1, t2 > ray.min ? t2 : ray.max + 1);

			if (t > ray.max || t < ray.min) {
				return false;
			}

			if(!ignoreHitRecord && t < hitRecord.t)
			{
				hitRecord.t = t;
				hitRecord.origin = ray.origin + t * ray.direction;
				hitRecord.normal = (hitRecord.origin - sphere.origin).Normalized();
				hitRecord.didHit = true;
				hitRecord.materialIndex = sphere.materialIndex;
			}
			return true;
		}

		inline bool HitTest_Sphere(const Sphere& sphere, const Ray& ray)
		{
			HitRecord temp{};
			return HitTest_Sphere(sphere, ray, temp, true);
		}
#pragma endregion
#pragma region Plane HitTest
		//PLANE HIT-TESTS
		inline bool HitTest_Plane(const Plane& plane, const Ray& ray, HitRecord& hitRecord, bool ignoreHitRecord = false)
		{
			const Vector3 rayToPlane{ plane.origin - ray.origin };

			const float originToPlaneDistance{ Vector3::Dot(rayToPlane, plane.normal) };
			const float rayDirectionDotNormal{ Vector3::Dot(plane.normal, ray.direction) };

			const float t{ originToPlaneDistance / rayDirectionDotNormal };

			if (t < ray.min || t > ray.max)
				return false;

			if(!ignoreHitRecord && t < hitRecord.t)
			{
				hitRecord.t = t;
				hitRecord.origin = ray.origin + t * ray.direction;
				hitRecord.normal = plane.normal;
				hitRecord.didHit = true;
				hitRecord.materialIndex = plane.materialIndex;
			}

			return true;
		}

		inline bool HitTest_Plane(const Plane& plane, const Ray& ray)
		{
			HitRecord temp{};
			return HitTest_Plane(plane, ray, temp, true);
		}
#pragma endregion
#pragma region Triangle HitTest
		//TRIANGLE HIT-TESTS
		inline bool HitTest_Triangle(const Triangle& triangle, const Ray& ray, HitRecord& hitRecord, bool ignoreHitRecord = false)
		{
			const float normalDotDirection{ Vector3::Dot(triangle.normal,ray.direction) };

			if(!ignoreHitRecord)
			{
				switch (triangle.cullMode)
				{
				case TriangleCullMode::BackFaceCulling:
					if (normalDotDirection > 0.f) return false; // Ignore back faces
					break;
				case TriangleCullMode::FrontFaceCulling:
					if (normalDotDirection < 0.f) return false; // Ignore front faces
					break;
				case TriangleCullMode::NoCulling:
					break;
				}
			}

			if(normalDotDirection == 0.f)
			{
				return false;
			}

			const Vector3 orginToRay{ triangle.v0 - ray.origin};

			const float originToPlaneDistance{ Vector3::Dot(orginToRay, triangle.normal) };
			const float rayDirectionDotNormal{ Vector3::Dot(triangle.normal, ray.direction) };

			const float t{ originToPlaneDistance / rayDirectionDotNormal };

			if (t < ray.min || t > ray.max)
				return false;

			const Vector3 point{ ray.origin + ray.direction * t };

			Vector3 nextV;
			Vector3 CurrentV;
			for (int i = 0; i < 3; ++i)
			{
				switch (i)
				{
				case 0:
					CurrentV = triangle.v0;
					nextV = triangle.v1;
					break;
				case 1:
					CurrentV = triangle.v1;
					nextV = triangle.v2;
					break;
				case 2:
					CurrentV = triangle.v2;
					nextV = triangle.v0;
					break;
				}
				const Vector3 e{nextV - CurrentV};
				const Vector3 p{ point - CurrentV };

				const Vector3 crossEP{ Vector3::Cross(e,p).Normalized() };
				if(Vector3::Dot(crossEP, triangle.normal) < 0)
				{
					return false;
				}
			}

			if (!ignoreHitRecord && t < hitRecord.t)
			{
				hitRecord.t = t;
				hitRecord.origin = point;
				hitRecord.normal = triangle.normal;
				hitRecord.didHit = true;
				hitRecord.materialIndex = triangle.materialIndex;
			}

			return true;
		}

		inline bool HitTest_Triangle(const Triangle& triangle, const Ray& ray)
		{
			HitRecord temp{};
			return HitTest_Triangle(triangle, ray, temp, true);
		}
#pragma endregion
#pragma region TriangeMesh HitTest
		inline bool HitTest_TriangleMesh(const TriangleMesh& mesh, const Ray& ray, HitRecord& hitRecord, bool ignoreHitRecord = false)
		{
			int currentTriangle{0};
			for (size_t i = 0; i < mesh.indices.size(); i += 3, currentTriangle++)
			{
				//create a triangle
				Triangle triangle;
				triangle.v0 = mesh.transformedPositions[mesh.indices[i]];
				triangle.v1 = mesh.transformedPositions[mesh.indices[i + 1]];
				triangle.v2 = mesh.transformedPositions[mesh.indices[i + 2]];
				triangle.normal = mesh.transformedNormals[currentTriangle];
				triangle.materialIndex = mesh.materialIndex;
				triangle.cullMode = mesh.cullMode;

				//use the HitTest_Traingle cuz I am lazy
				if (HitTest_Triangle(triangle, ray, hitRecord, ignoreHitRecord))
					return true;
			}
			return false;
		}

		inline bool HitTest_TriangleMesh(const TriangleMesh& mesh, const Ray& ray)
		{
			HitRecord temp{};
			return HitTest_TriangleMesh(mesh, ray, temp, true);
		}
#pragma endregion
	}

	namespace LightUtils
	{
		//Direction from target to light
		inline Vector3 GetDirectionToLight(const Light& light, const Vector3 origin)
		{
			const Vector3 result = light.origin - origin;
			return result;
		}

		inline ColorRGB GetRadiance(const Light& light, const Vector3& target)
		{
			if(light.type == LightType::Point)
			{
				return light.color * (light.intensity / Square((light.origin - target).Magnitude()));
			}

			if(light.type == LightType::Directional)
			{
				return light.color * light.intensity;
			}

			return {};
		}
	}

	namespace Utils
	{
		//Just parses vertices and indices
#pragma warning(push)
#pragma warning(disable : 4505) //Warning unreferenced local function
		static bool ParseOBJ(const std::string& filename, std::vector<Vector3>& positions, std::vector<Vector3>& normals, std::vector<int>& indices)
		{
			std::ifstream file(filename);
			if (!file)
				return false;

			std::string sCommand;
			// start a while iteration ending when the end of file is reached (ios::eof)
			while (!file.eof())
			{
				//read the first word of the string, use the >> operator (istream::operator>>) 
				file >> sCommand;
				//use conditional statements to process the different commands	
				if (sCommand == "#")
				{
					// Ignore Comment
				}
				else if (sCommand == "v")
				{
					//Vertex
					float x, y, z;
					file >> x >> y >> z;
					positions.push_back({ x, y, z });
				}
				else if (sCommand == "f")
				{
					float i0, i1, i2;
					file >> i0 >> i1 >> i2;

					indices.push_back((int)i0 - 1);
					indices.push_back((int)i1 - 1);
					indices.push_back((int)i2 - 1);
				}
				//read till end of line and ignore all remaining chars
				file.ignore(1000, '\n');

				if (file.eof())
					break;
			}

			//Precompute normals
			for (uint64_t index = 0; index < indices.size(); index += 3)
			{
				uint32_t i0 = indices[index];
				uint32_t i1 = indices[index + 1];
				uint32_t i2 = indices[index + 2];

				Vector3 edgeV0V1 = positions[i1] - positions[i0];
				Vector3 edgeV0V2 = positions[i2] - positions[i0];
				Vector3 normal = Vector3::Cross(edgeV0V1, edgeV0V2);

				if (std::isnan(normal.x))
				{
					int k = 0;
				}

				normal.Normalize();
				if (std::isnan(normal.x))
				{
					int k = 0;
				}

				normals.push_back(normal);
			}

			return true;
		}
#pragma warning(pop)
	}
}