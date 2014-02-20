#ifndef NAUBULLETMOTIONSTATE_H
#define NAUBULLETMOTIONSTATE_H

#include <btBulletDynamicsCommon.h>

#include <nau/scene/sceneobject.h>

namespace nau
{
	namespace world
	{

		class NauBulletMotionState :
			public btMotionState
		{
		private:
			nau::scene::SceneObject *m_SceneObject;
			nau::math::ITransform *t;

		public:
			NauBulletMotionState(nau::scene::SceneObject *aObject);
			~NauBulletMotionState(void);

			/*btMotionState interface*/
			void getWorldTransform (btTransform &worldTrans) const;
			void setWorldTransform (const btTransform &worldTrans);
		};
	};
};

#endif //NAUBULLETMOTIONSTATE_H
