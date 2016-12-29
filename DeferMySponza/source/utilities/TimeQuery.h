#pragma once

#include <vector>
#include <GL/glcorearb.h>

class TimeQuery {
public:
#pragma region Constructors/Destructors
	TimeQuery();
	~TimeQuery();
#pragma endregion
#pragma region Getters/Setters
	GLint getLastResult() const;
	GLint getMinResult() const;
	GLint getMaxResult() const;
	GLint getMeanResult() const;

	std::string toString() const;
#pragma endregion
#pragma region Static Methods

#pragma endregion
#pragma region Non-Static Methods
	void Begin();
	void End();
	void Reset();
#pragma endregion
private:
#pragma region Members
	GLuint m_nextToStart{ 0 };
	GLuint m_nextToCollect{ 0 };
	GLuint m_queries[8];

	GLint64 m_last{ 0 };
	GLint m_min{ INT_MAX };
	GLint m_max{ INT_MIN };

	GLuint m_index{ 0 };
	GLint64 m_list[16]{ 0 };

	const GLenum m_target{ GL_TIME_ELAPSED };
#pragma endregion
#pragma region Additional Methods
	void RegisterNewValue();
#pragma endregion
};