#include "TimeQuery.h"

#include <sstream>
#include <tgl\tgl.h>

#pragma region Constructors/Destructors

TimeQuery::TimeQuery() {
	glGenQueries(8, &m_queries[0]);
}

TimeQuery::~TimeQuery() {
	glDeleteQueries(8, &m_queries[0]);
}

#pragma endregion
#pragma region Getters/Setters

GLint TimeQuery::getLastResult() const {
	return m_last;
}

GLint TimeQuery::getMinResult() const {
	return m_min;
}

GLint TimeQuery::getMaxResult() const {
	return m_max;
}

GLint TimeQuery::getMeanResult() const {
	GLint mean = 0;
	for (unsigned int i = 0; i < 16; i++) {
		mean += m_list[i];
	}
	return mean / 16;
}

std::string TimeQuery::toString() const {
	std::ostringstream oss;
	oss << "Minimum: " << m_min / 1000000.0f << "ms || " <<
		"Mean: " << getMeanResult() / 1000000.0f << "ms || " <<
		"Maximum: " << m_max / 1000000.0f << "ms";
	return oss.str();
}

#pragma endregion
#pragma region Static Methods

#pragma endregion
#pragma region Non-Static Methods

void TimeQuery::Begin() {
	glBeginQuery(m_target, m_queries[m_nextToStart]);

	m_nextToStart++;
	m_nextToStart %= 8;
}

void TimeQuery::End() {
	glEndQuery(m_target);

	GLint status = 0;
	glGetQueryObjectiv(m_queries[m_nextToCollect], GL_QUERY_RESULT_AVAILABLE, &status);
	if (status == GL_TRUE) {
		glGetQueryObjecti64v(m_queries[m_nextToCollect], GL_QUERY_RESULT, &m_last);
		RegisterNewValue();

		m_nextToCollect++;
		m_nextToCollect %= 8;
	}
}

void TimeQuery::Reset() {
	m_min = INT_MAX;
	m_max = INT_MIN;
	m_index = 0;
	for (GLint64 &val : m_list) {
		val = 0;
	}
}

#pragma endregion
#pragma region Additional Methods

void TimeQuery::RegisterNewValue() {
	m_list[m_index] = m_last;
	m_index++;
	m_index %= 16;

	if (m_last < m_min) {
		m_min = m_last;
	}
	if (m_last > m_max) {
		m_max = m_last;
	}
}

#pragma endregion