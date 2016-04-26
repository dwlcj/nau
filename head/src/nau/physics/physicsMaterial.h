#ifndef _NAU_PHYSICS_MATERIAL
#define _NAU_PHYSICS_MATERIAL

#include "nau/attributeValues.h"
#include "nau/enums.h"
#include "nau/math/data.h"


namespace nau 
{
	namespace physics
	{
		class PhysicsMaterial: public AttributeValues 
		{
		public:
			FLOAT_PROP(MASS, 0);
			ENUM_PROP(SCENE_TYPE, 0);

			static AttribSet Attribs;

			PhysicsMaterial();

		protected:
			static bool Init();
			static bool Inited;

		};
	};
};

#endif