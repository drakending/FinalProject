﻿#pragma once

#include <iostream>
#include <string>

#include <glad/glad.h>

inline GLenum implCheckGLErrors(const char* file, int line) {
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR) {
		std::string error;
		switch (errorCode) {
		case GL_INVALID_ENUM: error = "INVALID_ENUM"; break;
		case GL_INVALID_VALUE: error = "INVALID_VALUE"; break;
		case GL_INVALID_OPERATION: error = "INVALID_OPERATION"; break;
		case GL_OUT_OF_MEMORY: error = "OUT_OF_MEMORY"; break;
		case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
		default: error = "UNKNOWN_ERROR"; break;
		}

		std::cerr << error << " | " << file << " (" << line << ")" << std::endl;
	}

	return errorCode;
}

#define checkGLErrors() implCheckGLErrors(__FILE__, __LINE__)