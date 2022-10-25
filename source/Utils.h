#pragma once
#include <cassert>
#include <fstream>
#include "Math.h"
#include "DataTypes.h"

namespace dae
{
	inline bool SlabTest_TriangleMesh(const TriangleMesh& mesh, const Ray& ray)
	{
		float tx1 = (mesh.transformedMaxAABB.x - ray.origin.x) / ray.direction.x;
		float tx2 = (mesh.transformedMinAABB.x - ray.origin.x) / ray.direction.x;

		float tmin = std::min(tx1, tx2);
		float tmax = std::max(tx1, tx2);

		float ty1 = (mesh.transformedMaxAABB.y - ray.origin.y) / ray.direction.y;
		float ty2 = (mesh.transformedMinAABB.y - ray.origin.y) / ray.direction.y;

		tmin = std::max(tmin, std::min(ty1, ty2));
		tmax = std::min(tmax, std::max(ty1, ty2));

		float tz1 = (mesh.transformedMaxAABB.z - ray.origin.z) / ray.direction.z;
		float tz2 = (mesh.transformedMinAABB.z - ray.origin.z) / ray.direction.z;

		tmin = std::max(tmin, std::min(tz1, tz2));
		tmax = std::min(tmax, std::max(tz1, tz2));

		return tmax > 0 && tmax >= tmin;
	}
	namespace GeometryUtils
	{
#pragma region Sphere HitTest
		//SPHERE HIT-TESTS
		inline bool HitTest_Sphere_slow(const Sphere& sphere, const Ray& ray, HitRecord& hitRecord, bool ignoreHitRecord = false)
		{
			//todo W1
			/*assert(false && "No Implemented Yet!");
			return false;*/

			Vector3 TC{ sphere.origin - ray.origin };
			float dp{ TC.Dot(TC, ray.direction) };
			/*float od = sqrtf(Square(TC.Magnitude()) - Square(dp));
			float tca = sqrtf(Square(sphere.radius) - Square(od));
			float t0 = dp - tca;*/

			//An *failed* optimization attempt
			float sphereRadiusSquared = Square(sphere.radius);

			float odSquared = Square(TC.Magnitude()) - Square(dp);
			float tca = sphereRadiusSquared - odSquared;
			float t0 = dp - sqrt(tca);

			//if hit distance is outside of ray scope return false
			if (t0 > ray.max || t0 < ray.min)
			{
				return false;
			}

			//if hitting sphere and closer than last closestHit
			if (odSquared <= sphereRadiusSquared && t0 < hitRecord.t)
			{
				hitRecord.didHit = true;
				if (ignoreHitRecord)
				{
					return true;
				}
				hitRecord.t = t0;
				hitRecord.materialIndex = sphere.materialIndex;
				hitRecord.origin = ray.origin + ray.direction * hitRecord.t;
				hitRecord.normal = (hitRecord.origin - sphere.origin).Normalized();
				return true;
			}
			return false;
		}

		inline bool HitTest_Sphere(const Sphere& sphere, const Ray& ray, HitRecord& hitRecord, bool ignoreHitRecord = false)
		{
			const Vector3 rayOriginToSphereOrigin{ sphere.origin - ray.origin };
			const float hypothenuseSquared{ rayOriginToSphereOrigin.SqrMagnitude() };
			const float side1{ Vector3::Dot(rayOriginToSphereOrigin, ray.direction) };

			const float distanceToRaySquared = hypothenuseSquared - side1 * side1;

			//if the distance to the ray is larger than the radius there will be no results
			//    also if equal because that is the exact border of the circle
			if (distanceToRaySquared >= sphere.radius * sphere.radius)
			{
				//hitRecord.didHit = false;
				return false;
			}

			const float distanceRaypointToIntersect = sqrt(sphere.radius * sphere.radius - distanceToRaySquared);
			const float t = side1 - distanceRaypointToIntersect;

			if (t < ray.min || t > ray.max)
			{
				//hitRecord.didHit = false;
				return false;
			}

			if (ignoreHitRecord) return true;

			hitRecord.didHit = true;
			hitRecord.materialIndex = sphere.materialIndex;
			hitRecord.t = t;
			hitRecord.origin = ray.origin + t * ray.direction;
			hitRecord.normal = Vector3(sphere.origin, hitRecord.origin).Normalized();
			return true;
		}

		inline bool HitTest_Sphere(const Sphere& sphere, const Ray& ray)
		{
			HitRecord temp{};
			return HitTest_Sphere(sphere, ray, temp, true);
		}

		inline bool HitTest_Sphere_Analytic(const Sphere& sphere, const Ray& ray, HitRecord& hitRecord, bool ignoreHitRecord = false)
		{
			//A diffrent way of doing it

			Vector3 TC{ sphere.origin - ray.origin };
			float dp{ TC.Dot(TC, ray.direction) };
			/*float od = sqrtf(Square(TC.Magnitude()) - Square(dp));
			float tca = sqrtf(Square(sphere.radius) - Square(od));
			float t0 = dp - tca;*/

			//An *failed* optimization attempt
			float sphereRadiusSquared = Square(sphere.radius);

			float odSquared = Square(TC.Magnitude()) - Square(dp);
			float tca = sphereRadiusSquared - odSquared;
			float t0 = dp - sqrtf(tca);

			//if hit distance is outside of ray scope return false
			if (t0 > ray.max || t0 < ray.min) return false;

			//if hitting sphere and closer than last closestHit
			if (odSquared <= sphereRadiusSquared && t0 < hitRecord.t)
			{
				hitRecord.didHit = true;
				hitRecord.t = t0;
				hitRecord.materialIndex = sphere.materialIndex;
				hitRecord.origin = ray.origin + ray.direction.Normalized() * hitRecord.t;
				hitRecord.normal = (hitRecord.origin - sphere.origin).Normalized();
				return true;
			}
			return false;
		}

#pragma endregion
#pragma region Plane HitTest
		//PLANE HIT-TESTS
		inline bool HitTest_Plane(const Plane& plane, const Ray& ray, HitRecord& hitRecord, bool ignoreHitRecord = false)
		{
			//todo W1
			/*assert(false && "No Implemented Yet!");
			return false;*/

			float t = Vector3::Dot(plane.origin - ray.origin, plane.normal) / Vector3::Dot(ray.direction, plane.normal);

			if (t > ray.min && t < ray.max)
			{
				Vector3 p = ray.origin + t * ray.direction;

				if (t < hitRecord.t)
				{
					hitRecord.didHit = true;
					hitRecord.t = t;
					hitRecord.materialIndex = plane.materialIndex;
					hitRecord.origin = p;
					hitRecord.normal = plane.normal;
					return true;
				}
			}
			return false;
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
			//todo W5
			/*assert(false && "No Implemented Yet!");
			return false;*/
			Vector3 normal = triangle.normal;
			
			float viewAngle = Vector3::Dot(normal, ray.direction);
			if (viewAngle == 0 ) return false; // Ray is parallel to the triangle

			switch (triangle.cullMode)
			{
			case TriangleCullMode::BackFaceCulling:
				if (!ignoreHitRecord)
				{
					if (viewAngle > 0) return false;
				}
				else
				{
					if (viewAngle < 0) return false;
				}
				
				break;
			case TriangleCullMode::FrontFaceCulling:
				if (!ignoreHitRecord)
				{
					if (viewAngle < 0) return false;
				}
				else
				{
					if (viewAngle > 0) return false;
				}
				break;
			case TriangleCullMode::NoCulling:
				break;
			}
			

			
			Vector3 center = (triangle.v0 + triangle.v1 + triangle.v2) / 3;
			Vector3 L = center - ray.origin;
			float t = Vector3::Dot(L, normal) / Vector3::Dot(ray.direction, normal);
			
			if (t < ray.min || t > ray.max) return false; // Ray is too long/short
			
			Vector3 p = ray.origin + t * ray.direction;
			
			Vector3 pointToSide = p - triangle.v0;
			Vector3 a = triangle.v1 - triangle.v0;
			if (Vector3::Dot(normal, Vector3::Cross(a, pointToSide)) < 0) return false; // Point is not in tiangle
			pointToSide = p - triangle.v0;
			Vector3 b = triangle.v2 - triangle.v0;
			if (Vector3::Dot(normal, Vector3::Cross(b, pointToSide)) > 0) return false; // Point is not in tiangle
			pointToSide = p - triangle.v1;
			Vector3 c = triangle.v2 - triangle.v1;
			if (Vector3::Dot(normal, Vector3::Cross(c, pointToSide)) < 0) return false; // Point is not in tiangle


			if (ignoreHitRecord) return true;

			hitRecord.didHit = true;
			hitRecord.materialIndex = triangle.materialIndex;
			hitRecord.t = t;
			hitRecord.origin = p;
			hitRecord.normal = normal;

			
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
			//todo W5
			/*assert(false && "No Implemented Yet!");
			return false;*/

			// slabtest
			if (!SlabTest_TriangleMesh(mesh, ray))
			{
				return false;
			}
			

			Ray newRay = ray;
			
			for (size_t i = 0; i < mesh.transformedNormals.size(); i++)
			{
				Triangle triangle{};
				triangle.cullMode = mesh.cullMode;
				triangle.materialIndex = mesh.materialIndex;
				triangle.normal = mesh.transformedNormals[i];
				
				int startIndex = i * 3;
				triangle.v0 = mesh.transformedPositions[mesh.indices[startIndex]];
				triangle.v1 = mesh.transformedPositions[mesh.indices[++startIndex]];
				triangle.v2 = mesh.transformedPositions[mesh.indices[++startIndex]];
				
				if (HitTest_Triangle(triangle, newRay, hitRecord, ignoreHitRecord))
				{
					if (ignoreHitRecord) // for shadows that don't care about hitrecord distance
					{
						return true;
					}
					newRay.max = hitRecord.t;
				}
			}
			return hitRecord.didHit;
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
			//todo W3
			/*assert(false && "No Implemented Yet!");
			return {};*/

			if (light.type == dae::LightType::Directional)
			{
				return Vector3(FLT_MAX, FLT_MAX, FLT_MAX);
			}

			return light.origin - origin;
		}

		inline ColorRGB GetRadiance(const Light& light, const Vector3& target)
		{
			//todo W3
			/*assert(false && "No Implemented Yet!");
			return {};*/

			if (light.type == LightType::Directional)
			{
				return light.color * light.intensity;
			}


			// light color * irradiance
			return light.color * (light.intensity / (light.origin - target).SqrMagnitude());
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

				if(isnan(normal.x))
				{
					int k = 0;
				}

				normal.Normalize();
				if (isnan(normal.x))
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