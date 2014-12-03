#include <nau/config.h>

#if NAU_OPENGL_VERSION >= 420

#ifndef IBUFFER_H
#define IBUFFER_H

#include <string>
#include <math.h>

#include <nau/attribute.h>
#include <nau/attributeValues.h>



using namespace nau;


namespace nau
{
	namespace render
	{
		class IBuffer : public AttributeValues
		{
		public:

			INT_PROP(ID,0);
//			INT_PROP(BINDING_POINT, 1);

			UINT_PROP(SIZE, 0);

//			ENUM_PROP(TYPE, 0);

//			BOOL_PROP(BOUND, 0);
			ENUM_PROP(CLEAR, 0);

			typedef enum {
				NEVER,
				BY_FRAME
			} ClearValues; 

			static AttribSet Attribs;


			// Note: no validation is performed!
			virtual void setProp(int prop, Enums::DataType type, void *value)  = 0;

			static IBuffer* Create(std::string label);

			std::string& getLabel (void);

			//virtual void bind() = 0;
			//virtual void unbind() = 0;

			virtual void clear() = 0;

			virtual IBuffer * clone() = 0;
		
			~IBuffer(void) {};
		
		protected:

			IBuffer() { initArrays(Attribs); };

			static bool Init();
			static bool Inited;

			std::string m_Label;
		};
	};
};

#endif // NAU_OPENGL_VERSION

#endif // IBUFFER_H