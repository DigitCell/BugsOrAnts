#ifndef SUPPORTBGFX_HPP
#define SUPPORTBGFX_HPP

#endif // SUPPORTBGFX_HPP


#pragma once
#include "common.h"
#include "bgfx_utils.h"
#include "imgui/imgui.h"

#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#include <bx/uint32_t.h>
#include "vec3.h"
#include "vector"


inline void mtxFromNormalScale(float* _result, const  bx::Vec3& _normal, bx::Vec3&  _scale, const bx::Vec3& _pos)
{
    bx::Vec3 tangent;
    bx::Vec3 bitangent;
    bx::calcTangentFrame(tangent, bitangent, _normal);

    store(&_result[ 0], mul(bitangent, _scale.x) );
    store(&_result[ 4], mul(_normal,   _scale.y) );
    store(&_result[ 8], mul(tangent,   _scale.z) );

    _result[ 3] = 0.0f;
    _result[ 7] = 0.0f;
    _result[11] = 0.0f;
    _result[12] = _pos.x;
    _result[13] = _pos.y;
    _result[14] = _pos.z;
    _result[15] = 1.0f;
}

struct PosColorVertex
{
    float m_pos[3];
    float m_normal[3];
    uint32_t m_abgr;

    static void init()
    {
        ms_layout
            .begin()
            .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
            .add(bgfx::Attrib::Normal,   3, bgfx::AttribType::Float)
            .add(bgfx::Attrib::Color0,   4, bgfx::AttribType::Uint8, true)
            .end();
    };

    inline static bgfx::VertexLayout ms_layout;
};

static  uint32_t tempColor1=(uint8_t(255) << 24) +
                            (uint8_t(255) << 16) +
                            (uint8_t(155) << 8)  +
                             uint8_t(255);

static  uint32_t tempColor2=(uint8_t(255) << 24) +
                            (uint8_t(155) << 16) +
                            (uint8_t(155) << 8)  +
                             uint8_t(255);

static  uint32_t tempColor3=(uint8_t(255) << 24) +
                            (uint8_t(155) << 16) +
                            (uint8_t(255) << 8)  +
                             uint8_t(255);
