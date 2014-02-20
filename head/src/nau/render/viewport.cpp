#include <nau/render/viewport.h>
#include <nau.h>

using namespace nau::render;
using namespace nau::math;

bool
Viewport::Init() {

	// VEC2
	Attribs.add(Attribute(ORIGIN, "ORIGIN", Enums::DataType::VEC2, true, new vec2(0.0f, 0.0f)));
	Attribs.add(Attribute(SIZE, "SIZE", Enums::DataType::VEC2, true, new vec2(0.0f, 0.0f)));
	Attribs.add(Attribute(SET_ORIGIN, "SET_ORIGIN", Enums::DataType::VEC2, false, new vec2(0.0f, 0.0f)));
	Attribs.add(Attribute(SET_SIZE, "SET_SIZE", Enums::DataType::VEC2, false, new vec2(1.0f, 1.0f)));

	// VEC4
	Attribs.add(Attribute(CLEAR_COLOR, "CLEAR_COLOR", Enums::DataType::VEC4, false, new vec4()));

	// BOOL
	Attribs.add(Attribute(FULL, "FULL", Enums::DataType::BOOL, false, new bool(false)));

	// FLOAT
	Attribs.add(Attribute(RATIO, "RATIO", Enums::DataType::FLOAT, false, new float(0.0f)));
	return true;
}


AttribSet Viewport::Attribs;
bool Viewport::Inited = Viewport::Init();


Viewport::Viewport(void) :
	m_Name("default")
{
	Attribs.initAttribInstanceBoolArray(m_BoolProps);
	Attribs.initAttribInstanceFloatArray(m_FloatProps);
	EVENTMANAGER->addListener("WINDOW_SIZE_CHANGED", this);
}


Viewport::~Viewport(void)
{
	EVENTMANAGER->removeListener("WINDOW_SIZE_CHANGED",this);
}


void 
Viewport::setName(std::string aName)
{
	m_Name = aName;
}


std::string&
Viewport::getName() 
{
	return m_Name;
}


void *
Viewport::getProp(int prop, Enums::DataType type) {

	switch (type) {

	case Enums::FLOAT:
		assert(m_FloatProps.count(prop) > 0);
		return(&(m_FloatProps[prop]));
		break;
	case Enums::VEC4:
		assert(m_Float4Props.count(prop) > 0);
		return(&(m_Float4Props[prop]));
		break;
	case Enums::INT:
		assert(m_IntProps.count(prop) > 0);
		return(&(m_IntProps[prop]));
		break;
		
	}
	return NULL;
}


void 
Viewport::setProp(int prop, Enums::DataType type, void *value) {

	switch (type) {

		case Enums::FLOAT:
			if (prop < COUNT_FLOATPROPERTY)
				setProp((FloatProperty)prop, *(float *)value);
			else
				m_FloatProps[prop] = *(float *)value;
			break;
		case Enums::VEC4:
			if (prop < COUNT_FLOAT4PROPERTY)
				setProp((Float4Property)prop, *(vec4 *)value);
			else
				m_Float4Props[prop].set((vec4 *)value);
			break;
		case Enums::INT:
			m_IntProps[prop] = *(int *)value;
			break;
	}
}


void
Viewport::setProp(Float4Property prop, const vec4& values) {

	m_Float4Props[prop] = values;
}


const vec4 &
Viewport::getPropf4(Float4Property prop) {

	return m_Float4Props[prop];
}


void
Viewport::setProp(BoolProperty prop, bool value) {

	if (prop == FULL) {
		m_BoolProps[prop] = value;

		if (value == true) {
			m_Float2Props[SIZE] = vec2(NAU->getWindowWidth(), NAU->getWindowWidth());
		}
	}
}


bool
Viewport::getPropb(BoolProperty prop) {

	return m_BoolProps[prop];
}


void
Viewport::setProp(FloatProperty prop, float value) {

	m_FloatProps[prop] = value;

	// if ratio is bigger than zero
	if (value > 0.0f) {
		setProp(SIZE, m_Float2Props[SET_SIZE]);
		setProp(ORIGIN, m_Float2Props[SET_ORIGIN]);
	}
}


float
Viewport::getPropf(FloatProperty prop) {

	switch(prop) {
	
		case RATIO:
			return (m_Float2Props[SIZE].x / m_Float2Props[SIZE].y);
	}
	return 0;
}




void
Viewport::setProp(Float2Property prop, const vec2& values){

	float width = values.x;
	float height = values.y;
	
	switch(prop) {
		case SIZE:
		case SET_SIZE:
			m_Float2Props[SET_SIZE] = values;
			if (m_FloatProps[RATIO] > 0.0f)
				m_Float2Props[SET_SIZE].y = m_Float2Props[SET_SIZE].x * m_FloatProps[RATIO];

			if (m_BoolProps[FULL] == false) {

				if (width <=1) {
					m_Float2Props[SIZE].x = width * NAU->getWindowWidth();
				}
				else {
					m_Float2Props[SIZE].x = width;
				}

				if (height <= 1) {
					m_Float2Props[SIZE].y = height * NAU->getWindowHeight();
				}
				else {
					m_Float2Props[SIZE].y = height;
				}

				if (m_FloatProps[RATIO] > 0) {
					m_Float2Props[SIZE].y = m_Float2Props[SIZE].x * m_FloatProps[RATIO];
				}
			}
			break;
		case ORIGIN:
		case SET_ORIGIN:
			m_Float2Props[SET_ORIGIN] = values;

			if (values.x < 1) {
				m_Float2Props[ORIGIN].x = NAU->getWindowWidth() * values.x;
			}
			else {
				m_Float2Props[ORIGIN].x = values.x;
			}

			if (values.y < 1) {
				m_Float2Props[ORIGIN].y = NAU->getWindowHeight() * values.y;
			}
			else {
				m_Float2Props[ORIGIN].y = values.y;
			}
			break;
	}
}


const vec2 &
Viewport::getPropf2(Float2Property prop) {

	return m_Float2Props[prop];
}


void
Viewport::eventReceived(const std::string &sender, const std::string &eventType, IEventData *evtData) {

	if (eventType != "WINDOW_SIZE_CHANGED")
		return;

	vec3 *ev = (vec3 *)(evtData->getData());

	if (m_BoolProps[FULL]) {
	
		m_Float2Props[SIZE].set(ev->x, ev->y);
	}
	else {
		if (m_Float2Props[SET_ORIGIN].x <= 1)
			m_Float2Props[ORIGIN].x = ev->x * m_Float2Props[SET_ORIGIN].x;

		if (m_Float2Props[SET_ORIGIN].y <= 1)
			m_Float2Props[ORIGIN].y = ev->y * m_Float2Props[SET_ORIGIN].y;

		if (m_Float2Props[SET_SIZE].x <= 1 )
			m_Float2Props[SIZE].x = ev->x * m_Float2Props[SET_SIZE].x;

		if (m_Float2Props[SET_SIZE].y <= 1 )
			m_Float2Props[SIZE].y = ev->y * m_Float2Props[SET_SIZE].y;

		if (m_FloatProps[RATIO] > 0)
			m_Float2Props[SIZE].y = m_Float2Props[SIZE].x * m_FloatProps[RATIO];
	}
	EVENTMANAGER->notifyEvent("VIEWPORT_CHANGED", m_Name, "", NULL); 
}


//float 
//Viewport::getRatio()
//{
//	if (m_Size.y == 0)
//		return 1;
//	else
//		return m_Size.x / m_Size.y;
//}
//
//
//void 
//Viewport::setRatio(float m) 
//{
//	if (m > 0) {
//		m_Ratio = m;
//		m_Size.y = m_Size.x * m_Ratio;
//	}
//}


//const vec2&
//Viewport::getSize (void) 
//{
//	return m_Size;
//}


//void
//Viewport::setSize (float width, float height)
//{
//	if (width <=1) {
//		m_RelSize.x = width;
//		m_Size.x = m_RelSize.x * NAU->getWindowWidth();
//	}
//	else {
//		m_RelSize.x = -1;
//		m_Size.x = width;
//	}
//
//	if (height <= 1) {
//		m_RelSize.y = height;
//		m_Size.y = m_RelSize.y * NAU->getWindowHeight();
//	}
//	else {
//		m_RelSize.y = -1;
//		m_Size.y = height;
//	}
//
//	if (m_Ratio > 0)
//		m_Size.y = m_Size.x * m_Ratio;
//}
//
//
//const vec2&
//Viewport::getOrigin (void)
//{
//	return m_Origin;
//}
//
//
//void
//Viewport::setOrigin (float x, float y)
//{
//	if (x < 1) {
//		m_RelOrigin.x = x;
//		m_Origin.x = NAU->getWindowWidth() * x;
//	}
//	else {
//		m_RelOrigin.x = -1;
//		m_Origin.x = x;
//	}
//
//	if (y < 1) {
//		m_RelOrigin.y = y;
//		m_Origin.y = NAU->getWindowHeight() * y;
//	}
//	else {
//		m_RelOrigin.y = -1;
//		m_Origin.y = y;
//	}
//	
//}


//const vec4&
//Viewport::getBackgroundColor (void)
//{
//	return m_BackgroundColor;
//}
//
//
//void
//Viewport::setBackgroundColor (const vec4& aColor)
//{
//	m_BackgroundColor = aColor;
//}


//bool 
//Viewport::isFixed (void)
//{
//	return m_Fixed;
//}
//
//
//void 
//Viewport::setFixed (bool value)
//{
//	m_Fixed = value;
//}
//
//
//bool 
//Viewport::isRelative (void)
//{
//	return (m_RelOrigin.x != -1 || m_RelOrigin.y != -1 || 
//		    m_RelSize.x != -1 || m_RelSize.y != -1);
//}
//
