#ifndef GRAPHMODULE_HPP
#define GRAPHMODULE_HPP

#pragma once
#include "common/common.h"
#include "common/bgfx_utils.h"
#include "common/imgui/imgui.h"
#include "bounds.h"

#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#include "camera.h"
#include <bx/uint32_t.h>
#include <debugdraw/debugdraw.h>

#include "bgfx_utils.h"

#include <bgfx/bgfx.h>
#include <bx/commandline.h>
#include <bx/endian.h>
#include <bx/math.h>
#include <bx/readerwriter.h>
#include <bx/string.h>

#include <cuda_runtime.h>
#include "algorithm"
#include <cmath>

#include "SupportBGFX.hpp"

#define SPRITE_TEXTURE_SIZE 1024

class GraphModule: public entry::AppI
{
public:
    GraphModule(const char* _name, const char* _description, const char* _url)
        : entry::AppI(_name, _description, _url),
          m_pt(0)
    {

    }

    void init(int32_t _argc, const char* const* _argv,  uint32_t _width, uint32_t _height) override
    {
        Args args(_argc, _argv);

        m_width  = _width;
        m_height = _height;
        m_debug  = BGFX_DEBUG_TEXT;
        m_reset  = BGFX_RESET_VSYNC;

        bgfx::Init init;
        init.type     = args.m_type;
        init.vendorId = args.m_pciId;
        init.resolution.width  = m_width;
        init.resolution.height = m_height;
        init.resolution.reset  = m_reset;
        bgfx::init(init);

        // Enable debug text.
        bgfx::setDebug(m_debug);

        // Set view 0 clear state.
        bgfx::setViewClear(0
            , BGFX_CLEAR_COLOR|BGFX_CLEAR_DEPTH
            , 0x303030ff
            , 1.0f
            , 0
            );

        // Get renderer capabilities info.
        const bgfx::Caps* caps = bgfx::getCaps();
        m_instancingSupported = 0 != (caps->supported & BGFX_CAPS_INSTANCING);

        // Create vertex stream declaration.
        PosColorVertex::init();

        m_numLights = 4;
        u_lightPosRadius = bgfx::createUniform("u_lightPosRadius", bgfx::UniformType::Vec4, m_numLights);
        u_lightRgbInnerR = bgfx::createUniform("u_lightRgbInnerR", bgfx::UniformType::Vec4, m_numLights);

        //m_program = loadProgram("vs_metaballs", "fs_metaballs");


        for(int icolor=0; icolor<255;icolor++)
        {

            int baseColorR=(int)(145*(((float) rand() / (RAND_MAX))));
            int baseColorG=(int)(145*(((float) rand() / (RAND_MAX))));
            int baseColorB=(int)(145*(((float) rand() / (RAND_MAX))));

        }

        m_timeOffset = bx::getHPCounter();

        cameraCreate();
        cameraSetPosition({ 35.0f, 375.f, 155.0f });
        cameraSetVerticalAngle(bx::toRad(-90));
        cameraSetHorizontalAngle(bx::toRad(0));

        ddInit();

        imguiCreate();
        ImGui::LoadIniSettingsFromDisk("tempImgui.ini");
         //ImGui::SaveIniSettingsToDisk("tempImgui.ini");
    }



    uint32_t ColorConvert(int r, int g, int b)
    {
      uint32_t c;
      c = r;
      c <<= 8;
      c |= g;
      c <<= 8;
      c |= b;
      return c;
    }

    int shutdown() override
    {
        ImGui::SaveIniSettingsToDisk("tempImgui.ini");

        imguiDestroy();

        //experiment.freeMemory();
        // Cleanup.
        //bgfx::destroy(m_ibh);
       // bgfx::destroy(m_vbh);
        bgfx::destroy(m_program);
        //bgfx::destroy(m_textureColor);
        //bgfx::destroy(m_textureNormal);
        //bgfx::destroy(s_texColor);
       //bgfx::destroy(s_texNormal);
        bgfx::destroy(u_lightPosRadius);
        bgfx::destroy(u_lightRgbInnerR);

        // Shutdown bgfx.
        bgfx::shutdown();

        return 0;
    }

    template<typename Ty>
    bool intersect(DebugDrawEncoder* _dde, const Ray& _ray, const Ty& _shape)
    {
       Hit hit;
       if (::intersect(_ray, _shape, &hit))
       {
           _dde->push();

           _dde->setWireframe(false);

           _dde->setColor(0xff0000ff);

           _dde->drawCone(hit.pos, bx::mad(hit.plane.normal, 0.7f, hit.pos), 0.1f);

           _dde->pop();

           return true;
       }

       return false;
    }

    void CreateMesh()
    {


    }

    bool update(Environment* env) override
    {
        if (!entry::processEvents(m_width, m_height, m_debug, m_reset, &m_mouseState) )
        {
            imguiBeginFrame(m_mouseState.m_mx
                ,  m_mouseState.m_my
                , (m_mouseState.m_buttons[entry::MouseButton::Left  ] ? IMGUI_MBUT_LEFT   : 0)
                | (m_mouseState.m_buttons[entry::MouseButton::Right ] ? IMGUI_MBUT_RIGHT  : 0)
                | (m_mouseState.m_buttons[entry::MouseButton::Middle] ? IMGUI_MBUT_MIDDLE : 0)
                ,  m_mouseState.m_mz
                , uint16_t(m_width)
                , uint16_t(m_height)
                );

            showExampleDialog(this);

            ImGui::Begin("Experiment commands");

                ImGui::SliderInt("Mesh size", &meshSize, 1, 9);

                if (ImGui::Button("Step Graph")){
                   stepGraphPress=true;
                }

                ImGui::Checkbox("Start-Pause generating", &StartPausePhysics);


            ImGui::End();


            imguiEndFrame();


            // Set view 0 default viewport.
            //bgfx::setViewRect(0, 0, 0, uint16_t(m_width), uint16_t(m_height) );

            // This dummy draw call is here to make sure that view 0 is cleared
            // if no other draw calls are submitted to view 0.

            float time = (float)( (bx::getHPCounter()-m_timeOffset)/double(bx::getHPFrequency() ) );

            int64_t now = bx::getHPCounter() - m_timeOffset;
            static int64_t last = now;
            const int64_t frameTime = now - last;
            last = now;
            const double freq = double(bx::getHPFrequency() );
            const float deltaTime = float(frameTime/freq);

            // Update camera.

            //cameraUpdate(deltaTime, m_mouseState);
            // Update camera.
            cameraUpdate(deltaTime, m_mouseState, ImGui::MouseOverArea() );


            float view[16];
            cameraGetViewMtx(view);

            float proj[16];

            // Set view and projection matrix for view 0.
            {
               bx::mtxProj(proj, 60.0f, float(m_width)/float(m_height), 0.1f, 1750.0f, bgfx::getCaps()->homogeneousDepth);

               bgfx::setViewTransform(0, view, proj);
               bgfx::setViewRect(0, 0, 0, uint16_t(m_width), uint16_t(m_height) );
            }

            float mtxVp[16];
            bx::mtxMul(mtxVp, view, proj);

            float mtxInvVp[16];
            bx::mtxInverse(mtxInvVp, mtxVp);

            const bx::Vec3 at  = { 0.0f,  0.0f, 0.0f };
            const bx::Vec3 eye = { 5.0f, 10.0f, 5.0f };
            bx::mtxLookAt(view, eye, at);
            bx::mtxProj(proj, 45.0f, float(m_width)/float(m_height), 1.0f, 15.0f, bgfx::getCaps()->homogeneousDepth);
            bx::mtxMul(mtxVp, view, proj);


            Ray ray = makeRay(
                               (float(m_mouseState.m_mx)/float(m_width)  * 2.0f - 1.0f)
                            , -(float(m_mouseState.m_my)/float(m_height) * 2.0f - 1.0f)
                            , mtxInvVp
                            );

           // bgfx::touch(0);



            float lightPosRadius[4][4];
            for (uint32_t ii = 0; ii < m_numLights; ++ii)
            {
                lightPosRadius[ii][0] = -50.0f+ii*50.0f;
                lightPosRadius[ii][1] = 30.0f*ii;
                lightPosRadius[ii][2] = -50.0f+57.5f+3*ii;
                lightPosRadius[ii][3] = 555.0f;
            }

            bgfx::setUniform(u_lightPosRadius, lightPosRadius, m_numLights);

            float lightRgbInnerR[4][4] =
            {
                { 1.0f, 0.7f, 0.7f, 0.8f },
                { 0.7f, 0.7f, 1.0f, 0.8f },
                { 0.7f, 1.0f, 0.7f, 0.8f },
                { 1.0f, 0.4f, 0.7f, 0.8f },
            };

            bgfx::setUniform(u_lightRgbInnerR, lightRgbInnerR, m_numLights);

            constexpr uint32_t kSelected = 0xff80ffff;
            constexpr uint32_t kOverlapA = 0xff0000ff;
            constexpr uint32_t kOverlapB = 0xff8080ff;

            DebugDrawEncoder dde;

            dde.begin(0);
            dde.drawAxis(-3.0f, 0.0f, -3.0f,5.0f);
           // dde.drawAxis(0.0f, 0.0f, 0.0f,5);
            {
                const bx::Vec3 normal = { 0.0f,  1.0f, 0.0f };
                const bx::Vec3 pos    = { 50.0f, 0.0f, 50.0f };
                dde.drawGrid(Axis::Y, pos, 250, 1.0f);
            }

            dde.setColor(0xffffffff);
           // const uint16_t instanceStride =128;

            // 80 bytes stride = 64 bytes for 4x4 matrix + 16 bytes for RGBA color.
            const uint16_t instanceStride = 64;
            const uint16_t numInstances   = 10;

            dde.push();

              // dde.lineTo(seg);
               bx::Vec3 temp=(bx::Vec3(0,0,0));
               Sphere sphere1 = { temp, 1.00f };
               dde.setColor(tempColor2);
               dde.setWireframe(false);
               dde.setLod(1);
               dde.draw(sphere1);


            dde.pop();

            float zoom=0.3f;
            for(auto bug:env->bugs)
            {

                for(int i=1; i<bug.segments.size();i++)
                {
                    bx::Vec3 tempVect=bx::Vec3(0);
                    tempVect.x=  bug.segments[i-1].getX();
                    tempVect.y= 5.0f;
                    tempVect.z= bug.segments[i-1].getY();

                    bx::Vec3 tempVect2=bx::Vec3(0);
                    tempVect2.x=  bug.segments[i].getX();
                    tempVect2.y= 5.0f;
                    tempVect2.z= bug.segments[i].getY();

                    dde.push();
                        Cylinder ray = { tempVect,
                                         tempVect2 ,
                                         2.0f};
                       dde.setColor(tempColor1);
                       dde.setWireframe(false);
                       dde.setLod(1);

                        dde.draw(ray);

                    dde.pop();

                }
                for(auto segment:bug.segments)
                {

                   bx::Vec3 tempVect=bx::Vec3(0);
                   tempVect.x=  segment.getX();
                   tempVect.y= 5.0f;
                   tempVect.z= segment.getY();

                   dde.push();
                      Sphere sphere2 = {tempVect, 3.0f};
                      dde.setColor(tempColor1);
                      dde.setWireframe(false);
                      dde.setLod(1);
                      dde.draw(sphere2);

                    dde.pop();

                //    dde.moveTo(tempVect);
                //    dde.setColor(tempColor2);
                //   dde.lineTo(bx::add(tempVect,bx::mul(bx::Vec3(agent.direction.x,agent.direction.y,agent.direction.z),15.f)));

                float legSegmentHeight=7.5f;
                {
                    Leg LL=segment.legs[0];
                    float dx = LL.foot.x - LL.pos.x;
                    float dy = LL.foot.y - LL.pos.y;
                    float dist =sqrt(dx * dx + dy * dy);
                    float elbowAngle =acos(dist / LL.length) * sign(LL.direction);
                    float footDirection = atan2(LL.foot.y - LL.pos.y, LL.foot.x - LL.pos.x);
                    float elbowX = LL.pos.x+ cos(footDirection + elbowAngle) * LL.length* 0.5;
                    float elbowY = LL.pos.y+sin(footDirection + elbowAngle) * LL.length * 0.5;


                    float radiusLeg=1.0f;
                    bx::Vec3 tempVect=bx::Vec3(0);
                    tempVect.x=  LL.foot.x;
                    tempVect.y= 2.0f;
                    tempVect.z= LL.foot.y;

                    dde.push();
                       Sphere sphere2 = {tempVect, radiusLeg};
                       dde.setColor(tempColor1);
                       dde.setWireframe(false);
                       dde.setLod(1);
                       dde.draw(sphere2);

                     dde.pop();

                    {
                        bx::Vec3 tempVect=bx::Vec3(0);
                        tempVect.x= LL.pos.x;
                        tempVect.y= 2.0f;
                        tempVect.z= LL.pos.y;

                        bx::Vec3 tempVect2=bx::Vec3(0);
                        tempVect2.x= elbowX;
                        tempVect2.y= legSegmentHeight;
                        tempVect2.z= elbowY;

                        //dde.moveTo(tempVect);
                       // dde.setColor(tempColor2);
                       // dde.lineTo(tempVect2);

                        dde.push();
                            Cylinder ray = { tempVect,
                                             tempVect2 ,
                                             0.15f};
                           dde.setColor(tempColor2);
                           dde.setWireframe(false);
                           dde.setLod(1);

                            dde.draw(ray);

                        dde.pop();

                        dde.push();
                           Sphere sphere2 = {tempVect2, radiusLeg};
                           dde.setColor(tempColor1);
                           dde.setWireframe(false);
                           dde.setLod(1);
                           dde.draw(sphere2);

                         dde.pop();
                    }
                    {

                        bx::Vec3 tempVect=bx::Vec3(0);
                        tempVect.x= elbowX;
                        tempVect.y= legSegmentHeight;
                        tempVect.z= elbowY;

                        bx::Vec3 tempVect2=bx::Vec3(0);
                        tempVect2.x= LL.foot.x;
                        tempVect2.y= 2.0f;
                        tempVect2.z= LL.foot.y;

                        //dde.moveTo(tempVect);
                       // dde.setColor(tempColor2);
                       // dde.lineTo(tempVect2);

                        dde.push();
                            Cylinder ray = { tempVect,
                                             tempVect2 ,
                                             0.15f};
                           dde.setColor(tempColor2);
                           dde.setWireframe(false);
                           dde.setLod(1);

                            dde.draw(ray);

                        dde.pop();


                     }

                }
                    {
                        Leg LL=segment.legs[1];
                        float dx = LL.foot.x - LL.pos.x;
                        float dy = LL.foot.y - LL.pos.y;
                        float dist =sqrt(dx * dx + dy * dy);
                        float elbowAngle =acos(dist / LL.length) * sign(LL.direction);
                        float footDirection = atan2(LL.foot.y - LL.pos.y, LL.foot.x - LL.pos.x);
                        float elbowX = LL.pos.x+ cos(footDirection + elbowAngle) * LL.length* 0.5;
                        float elbowY = LL.pos.y+sin(footDirection + elbowAngle) * LL.length * 0.5;


                        float radiusLeg=1.0f;
                        bx::Vec3 tempVect=bx::Vec3(0);
                        tempVect.x=  LL.foot.x;
                        tempVect.y= 2.0f;
                        tempVect.z= LL.foot.y;

                        dde.push();
                           Sphere sphere2 = {tempVect, radiusLeg};
                           dde.setColor(tempColor1);
                           dde.setWireframe(false);
                           dde.setLod(1);
                           dde.draw(sphere2);

                         dde.pop();

                        {
                            bx::Vec3 tempVect=bx::Vec3(0);
                            tempVect.x= LL.pos.x;
                            tempVect.y= 2.0f;
                            tempVect.z= LL.pos.y;

                            bx::Vec3 tempVect2=bx::Vec3(0);
                            tempVect2.x= elbowX;
                            tempVect2.y= legSegmentHeight;
                            tempVect2.z= elbowY;

                            //dde.moveTo(tempVect);
                           // dde.setColor(tempColor2);
                           // dde.lineTo(tempVect2);

                            dde.push();
                                Cylinder ray = { tempVect,
                                                 tempVect2 ,
                                                0.15f};
                               dde.setColor(tempColor2);
                               dde.setWireframe(false);
                               dde.setLod(1);

                                dde.draw(ray);

                            dde.pop();

                            dde.push();
                               Sphere sphere2 = {tempVect2, radiusLeg};
                               dde.setColor(tempColor1);
                               dde.setWireframe(false);
                               dde.setLod(1);
                               dde.draw(sphere2);

                             dde.pop();
                        }
                        {

                            bx::Vec3 tempVect=bx::Vec3(0);
                            tempVect.x= elbowX;
                            tempVect.y= legSegmentHeight;
                            tempVect.z= elbowY;

                            bx::Vec3 tempVect2=bx::Vec3(0);
                            tempVect2.x= LL.foot.x;
                            tempVect2.y= 2.0f;
                            tempVect2.z= LL.foot.y;

                            //dde.moveTo(tempVect);
                           // dde.setColor(tempColor2);
                           // dde.lineTo(tempVect2);

                            dde.push();
                                Cylinder ray = { tempVect,
                                                 tempVect2 ,
                                                 0.15f};
                               dde.setColor(tempColor2);
                               dde.setWireframe(false);
                               dde.setLod(1);

                                dde.draw(ray);

                            dde.pop();


                         }

                    }


                }



            }

          bgfx::frame();
          dde.end();

          int colorIndex=-1;
          int colorParticlesNumber=0;
          int colorParticlesNumberStore=0;


          tick++;

          return true;
       }

        return false;
    }

    entry::MouseState m_mouseState;

    uint32_t m_width;
    uint32_t m_height;
    uint32_t m_debug;
    uint32_t m_reset;

    bgfx::VertexBufferHandle m_vbh;
    bgfx::IndexBufferHandle  m_ibh;

    std::vector<bgfx::VertexBufferHandle> m_vbhList;
    std::vector<bgfx::IndexBufferHandle>  m_ibhList;

    bgfx::ProgramHandle m_program;
/*
    bgfx::UniformHandle s_texColor;
    bgfx::UniformHandle s_texNormal;
    bgfx::TextureHandle m_textureColor;
    std::vector<bgfx::TextureHandle> m_textureColorList;
    bgfx::TextureHandle m_textureNormal;
    */
    bgfx::UniformHandle u_lightPosRadius;
    bgfx::UniformHandle u_lightRgbInnerR;


    uint16_t m_numLights;
    bool m_instancingSupported;


    std::vector<bx::Vec3> scaleVector;

    int32_t m_pt;

    int numberVertex=0;
    int64_t m_timeOffset;

    int colorMesh=0;
    int meshSize=1;

    int tick=0;

    bool StartPausePhysics=false;
    bool stepGraphPress=false;

    template <typename T> int sign(T val) {
               return (T(0) < val) - (val < T(0));
           }


};

#endif // GRAPHMODULE_HPP
