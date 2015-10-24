#include "nau/geometry/vertexData.h"
#include "nau/config.h"
#include <assert.h>

#ifdef NAU_OPENGL
#include "nau/render/opengl/glVertexArray.h"
#endif

using namespace nau::geometry;
using namespace nau::render;
using namespace nau::math;

std::vector<VertexData::Attr> VertexData::NoData;

const std::string VertexData::Syntax[] = {
	"position", 
	"normal", 
	"color",
	"texCoord0",
	"texCoord1",
	"texCoord2",
	"texCoord3",
	"tangent",
	"bitangent",
	"triangleID",
	"custom0",
	"custom1",
	"custom2",
	"custom3",
	"custom4",
	"custom5",
	"index"};


unsigned int 
VertexData::GetAttribIndex(std::string attribName) {

	unsigned int index;

	for (index = 0; index < VertexData::MaxAttribs; index++) {

		if (VertexData::Syntax[index].compare(attribName) == 0)
			return(index);
	}
	assert("Attrib Index out of range");
	return (MaxAttribs);
}


VertexData* 
VertexData::create (std::string name) {

	VertexData *v;
#ifdef NAU_OPENGL
	v = new GLVertexArray;
#elif NAU_DIRECTX
	v = new DXVertexArray;
#endif
	v->m_Name = name;

	return v;
}


VertexData::VertexData(void) {

	for (int i = 0; i < VertexData::MaxAttribs; i++) {
		m_InternalArrays[i] = 0;
	}
}


VertexData::~VertexData(void) {

	for (int i = 0; i < VertexData::MaxAttribs; i++) {

		if (0 != m_InternalArrays[i]){
			delete m_InternalArrays[i];
		}
	}
}


void
VertexData::setName(std::string &name) {

	m_Name = name;
}


std::vector<VertexData::Attr>& 
VertexData::getDataOf (unsigned int type) {

	if (0 == m_InternalArrays[type]) {
		return VertexData::NoData;
	}
	return (*m_InternalArrays[type]);
}


void 
VertexData::setDataFor (unsigned index, std::vector<Attr>* dataArray) {

	assert(index < VertexData::MaxAttribs && *dataArray != VertexData::NoData);

	if (0 != m_InternalArrays[index])
		delete m_InternalArrays[index];

	if (index < VertexData::MaxAttribs && *dataArray != VertexData::NoData) {
		m_InternalArrays[index] = dataArray;
	}
}


int
VertexData::add (VertexData &aVertexData) {

	size_t offset = 0;

	std::vector<Attr> &vertices = getDataOf (GetAttribIndex("position"));

	offset = vertices.size();

	if (offset == 0) {

		for (int i = 0; i < VertexData::MaxAttribs; i++) {
			std::vector<Attr> &newVec = aVertexData.getDataOf(i);
			if (newVec != VertexData::NoData) {
				std::vector<Attr> *aVec = new std::vector<Attr>;
				aVec->insert(aVec->end(),newVec.begin(), newVec.end());
				setDataFor(i,aVec);
			}
		}
	}
	else {

		for (int i = 0; i < VertexData::MaxAttribs; i++) {

			std::vector<Attr> &thisVec = getDataOf(i);
			std::vector<Attr> &newVec = aVertexData.getDataOf(i);

			if (newVec != VertexData::NoData && thisVec != VertexData::NoData) {
			
				thisVec.insert(thisVec.end(),newVec.begin(), newVec.end());
			}
			else if (newVec != VertexData::NoData && thisVec == VertexData::NoData) {
			
				std::vector<Attr> *aVec = new std::vector<Attr>(offset);
				aVec->insert(aVec->end(), newVec.begin(), newVec.end());
				setDataFor(i,aVec);
			}
			else if (newVec == VertexData::NoData && thisVec != VertexData::NoData) {

				size_t size = aVertexData.getDataOf(GetAttribIndex("position")).size();
				thisVec.resize(offset + size);
			}
		}
	}

	return (int)offset;
}


void 
VertexData::unitize(vec3 &vCenter, vec3 &vMin, vec3 &vMax) {

	std::vector<Attr> &vertices = getDataOf (GetAttribIndex("position"));

	if (vertices == VertexData::NoData) 
		return;

	float max;
	vec3 diff;

	diff = vMax;
	diff -= vMin;
	if (diff.x > diff.y) {

		if (diff.x > diff.z) {
			max = diff.x;
		}
		else {
			max = diff.z;
		} 
	}
	else if (diff.y > diff.z) {

		max = diff.y;
	}
	else {
		max = diff.z;
	}

	max *= 0.5;
	//scale vertices so that min = -1 and max = 1
	for(unsigned int i = 0; i < vertices.size(); i++) {
	
		vertices[i].x = (vertices[i].x - vCenter.x) / max; 
		vertices[i].y = (vertices[i].y - vCenter.y) / max; 
		vertices[i].z = (vertices[i].z - vCenter.z) / max; 
	}

}


