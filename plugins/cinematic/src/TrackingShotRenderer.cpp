/*
* TrackingShotRenderer.cpp
*
* Copyright (C) 2017 by VISUS (Universitaet Stuttgart).
* Alle Rechte vorbehalten.
*/

#include "stdafx.h"

#include "TrackingShotRenderer.h"


using namespace megamol;
using namespace megamol::core;
using namespace megamol::core::view;
using namespace megamol::core::utility;
using namespace megamol::cinematic;

using namespace vislib;


TrackingShotRenderer::TrackingShotRenderer(void) : Renderer3DModule_2(),
    rendererCallerSlot("renderer", "outgoing renderer"),
    keyframeKeeperSlot("keyframeKeeper", "Connects to the Keyframe Keeper."),
<<<<<<< HEAD
    interpolStepsParam("splineSubdivision", "Amount of interpolation steps between keyframes."),
=======
    stepsParam("splineSubdivision", "Amount of interpolation steps between keyframes."),
>>>>>>> 858bc042e1c54af29b2f4defd2b89d65e5c72dfd
    toggleManipulateParam("toggleManipulators", "Toggle different manipulators for the selected keyframe."),
    toggleHelpTextParam("helpText", "Show/hide help text for key assignments."),
    toggleManipOusideBboxParam("manipulatorsOutsideBBox", "Keep manipulators always outside of model bounding box."),

    theFont(megamol::core::utility::SDFFont::FontName::ROBOTO_SANS), 
    interpolSteps(20),
    toggleManipulator(0),
    manipOutsideModel(false),
    showHelpText(false),
    manipulators(), 
    manipulatorGrabbed(false),
    textureShader(),
    fbo(),
    mouseX(0.0f),
    mouseY(0.0f), 
    utils() {

    this->rendererCallerSlot.SetCompatibleCall<CallRender3D_2Description>();
    this->MakeSlotAvailable(&this->rendererCallerSlot);

    this->keyframeKeeperSlot.SetCompatibleCall<CallKeyframeKeeperDescription>();
    this->MakeSlotAvailable(&this->keyframeKeeperSlot);

    // init parameters
    this->interpolStepsParam.SetParameter(new param::IntParam((int)this->interpolSteps, 1));
    this->MakeSlotAvailable(&this->interpolStepsParam);

    this->toggleManipulateParam.SetParameter(new param::ButtonParam(core::view::Key::KEY_Q, core::view::Modifier::CTRL));
    this->MakeSlotAvailable(&this->toggleManipulateParam);

    this->toggleHelpTextParam.SetParameter(new param::ButtonParam(core::view::Key::KEY_H, core::view::Modifier::CTRL));
    this->MakeSlotAvailable(&this->toggleHelpTextParam);

    this->toggleManipOusideBboxParam.SetParameter(new param::ButtonParam(core::view::Key::KEY_W, core::view::Modifier::CTRL));
    this->MakeSlotAvailable(&this->toggleManipOusideBboxParam);

    // Load spline interpolation keyframes at startup
    this->interpolStepsParam.ForceSetDirty();
}


TrackingShotRenderer::~TrackingShotRenderer(void) {
	this->Release();
}


bool TrackingShotRenderer::create(void) {

    vislib::graphics::gl::ShaderSource vert, frag;

    const char *shaderName = "TrackingShotShader_Render2Texturer";

    try {
        if (!megamol::core::Module::instance()->ShaderSourceFactory().MakeShaderSource("TrackingShotShader::vertex", vert)) {
            return false;
        }
        if (!megamol::core::Module::instance()->ShaderSourceFactory().MakeShaderSource("TrackingShotShader::fragment", frag)) {
            return false;
        }
        if (!this->textureShader.Create(vert.Code(), vert.Count(), frag.Code(), frag.Count())) {
            vislib::sys::Log::DefaultLog.WriteMsg(vislib::sys::Log::LEVEL_ERROR, "Unable to compile %s: Unknown error\n", shaderName);
            return false;
        }
    }
    catch (vislib::graphics::gl::AbstractOpenGLShader::CompileException ce) {
        vislib::sys::Log::DefaultLog.WriteMsg(vislib::sys::Log::LEVEL_ERROR,
            "Unable to compile %s shader (@%s): %s\n", shaderName,
            vislib::graphics::gl::AbstractOpenGLShader::CompileException::CompileActionName(ce.FailedAction()), ce.GetMsgA());
        return false;
    }
    catch (vislib::Exception e) {
        vislib::sys::Log::DefaultLog.WriteMsg(vislib::sys::Log::LEVEL_ERROR,
            "Unable to compile %s shader: %s\n", shaderName, e.GetMsgA());
        return false;
    }
    catch (...) {
        vislib::sys::Log::DefaultLog.WriteMsg(vislib::sys::Log::LEVEL_ERROR,
            "Unable to compile %s shader: Unknown exception\n", shaderName);
        return false;
    }

    // initialise font
    if (!this->theFont.Initialise(this->GetCoreInstance())) {
        vislib::sys::Log::DefaultLog.WriteWarn("[TIMELINE RENDERER] [Render] Couldn't initialize the font.");
        return false;
    }

    this->utils.InitPrimitiveRendering(megamol::core::Module::instance()->ShaderSourceFactory());
    vislib::StringA shortfilename = "arrow.png";
    auto fullfilename = megamol::core::utility::ResourceWrapper::getFileName(this->GetCoreInstance()->Configuration(), shortfilename);
    this->utils.LoadTextureFromFile(std::wstring(fullfilename.PeekBuffer()), this->texture);

	return true;
}


void TrackingShotRenderer::release(void) {

    this->textureShader.Release();

    if (this->fbo.IsEnabled()) {
        this->fbo.Disable();
    }
    this->fbo.Release();
}


bool TrackingShotRenderer::GetExtents(megamol::core::view::CallRender3D_2& call) {

    auto cr3d_in = &call;
    if (cr3d_in == nullptr) return false;

    // Propagate changes made in GetExtents() from outgoing CallRender3D_2 (cr3d_out) to incoming  CallRender3D_2 (cr3d_in).
    auto cr3d_out = this->rendererCallerSlot.CallAs<view::CallRender3D_2>();

    if ((cr3d_out != nullptr) && (*cr3d_out)(core::view::AbstractCallRender::FnGetExtents)) {
        CallKeyframeKeeper *ccc = this->keyframeKeeperSlot.CallAs<CallKeyframeKeeper>();
        if (ccc == nullptr) return false;
        if (!(*ccc)(CallKeyframeKeeper::CallForGetUpdatedKeyframeData)) return false;

        // Compute bounding box including spline (in world space) and object (in world space).
        vislib::math::Cuboid<float> bbox = cr3d_out->AccessBoundingBoxes().BoundingBox();
        // Set bounding box center of model
        ccc->setBboxCenter(P2G(cr3d_out->AccessBoundingBoxes().BoundingBox().CalcCenter()));

        // Grow bounding box to manipulators and get information of bbox of model
<<<<<<< HEAD
        this->manipulators.SetExtents(&bbox);
=======
        this->manipulator.SetExtents(bbox);
>>>>>>> 858bc042e1c54af29b2f4defd2b89d65e5c72dfd

        vislib::math::Cuboid<float> cbox = cr3d_out->AccessBoundingBoxes().ClipBox();

        // Get bounding box of spline.
        auto bboxCCC = ccc->getBoundingBox();
        if (bboxCCC == nullptr) {
            vislib::sys::Log::DefaultLog.WriteWarn("[CINEMATIC RENDERER] [Get Extents] Pointer to boundingbox array is nullptr.");
            return false;
        }

        bbox.Union(*bboxCCC);
        cbox.Union(*bboxCCC); // use boundingbox to get new clipbox

        // Set new bounding box center of slave renderer model (before applying keyframe bounding box)
        ccc->setBboxCenter(P2G(cr3d_out->AccessBoundingBoxes().BoundingBox().CalcCenter()));
        if (!(*ccc)(CallKeyframeKeeper::CallForSetSimulationData)) return false;

        // Propagate changes made in GetExtents() from outgoing CallRender3D_2 (cr3d_out) to incoming  CallRender3D_2 (cr3d_in).
        // => Bboxes and times.

        unsigned int timeFramesCount = cr3d_out->TimeFramesCount();
        cr3d_in->SetTimeFramesCount((timeFramesCount > 0) ? (timeFramesCount) : (1));
        cr3d_in->SetTime(cr3d_out->Time());
        cr3d_in->AccessBoundingBoxes() = cr3d_out->AccessBoundingBoxes();

        // Apply modified boundingbox 
        cr3d_in->AccessBoundingBoxes().SetBoundingBox(bbox);
        cr3d_in->AccessBoundingBoxes().SetClipBox(cbox);
    }

	return true;
}


bool TrackingShotRenderer::Render(megamol::core::view::CallRender3D_2& call) {

    auto cr3d_in = &call;
    if (cr3d_in == nullptr) return false;

    auto cr3d_out = this->rendererCallerSlot.CallAs<CallRender3D_2>();
    if (cr3d_out == nullptr) return false;

    CallKeyframeKeeper *ccc = this->keyframeKeeperSlot.CallAs<CallKeyframeKeeper>();
    if (ccc == nullptr) return false;
    // Updated data from cinematic camera call
    if (!(*ccc)(CallKeyframeKeeper::CallForGetUpdatedKeyframeData)) return false;

    // Update parameter
    if (this->interpolStepsParam.IsDirty()) {
        this->interpolSteps = this->interpolStepsParam.Param<param::IntParam>()->Value();
        ccc->setInterpolationSteps(this->interpolSteps);
        if (!(*ccc)(CallKeyframeKeeper::CallForGetInterpolCamPositions)) return false;
        this->interpolStepsParam.ResetDirty();
    }
    if (this->toggleManipulateParam.IsDirty()) {
        // There are currently two different manipulator groups ...
        this->toggleManipulator = (this->toggleManipulator + 1) % 2; 
        this->toggleManipulateParam.ResetDirty();
    }
    if (this->toggleHelpTextParam.IsDirty()) {
        this->showHelpText = !this->showHelpText;
        this->toggleHelpTextParam.ResetDirty();
    }
    if (this->toggleManipOusideBboxParam.IsDirty()) {
        this->manipOutsideModel = !this->manipOutsideModel;
        this->toggleManipOusideBboxParam.ResetDirty();
    }

    // Set total simulation time of call
    float totalSimTime = static_cast<float>(cr3d_out->TimeFramesCount());
    ccc->setTotalSimTime(totalSimTime);
    if (!(*ccc)(CallKeyframeKeeper::CallForSetSimulationData)) return false;

    Keyframe skf = ccc->getSelectedKeyframe();

    // Set simulation time based on selected keyframe ('disables'/ignores animation via view3d)
    float simTime = skf.GetSimTime();
    cr3d_in->SetTime(simTime * totalSimTime);

    // Get the foreground color (inverse background color)
    float bgColor[4];
    float fgColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
    float white[4]   = { 1.0f, 1.0f, 1.0f, 1.0f };
    float yellow[4]  = { 1.0f, 1.0f, 0.0f, 1.0f };
    float menu[4]    = { 0.0f, 0.0f, 0.3f, 1.0f };
    glGetFloatv(GL_COLOR_CLEAR_VALUE, bgColor);
    for (unsigned int i = 0; i < 3; i++) {
        fgColor[i] -= bgColor[i];
    }
    // COLORS
    float sColor[4] = { 0.4f, 0.4f, 1.0f, 1.0f }; // Color for SPLINE
    // Adapt colors depending on  Lightness
    float L = (vislib::math::Max(bgColor[0], vislib::math::Max(bgColor[1], bgColor[2])) + vislib::math::Min(bgColor[0], vislib::math::Min(bgColor[1], bgColor[2]))) / 2.0f;
    if (L < 0.5f) {
        // not used so far
    }

    // Get current viewport
    int vp[4];
    glGetIntegerv(GL_VIEWPORT, vp);
    unsigned int vpWidth  = vp[2];
    unsigned int vpHeight = vp[3];

    // Get pointer to keyframes array
    auto keyframes = ccc->getKeyframes();
    if (keyframes == nullptr) {
        vislib::sys::Log::DefaultLog.WriteWarn("[CINEMATIC RENDERER] [Render] Pointer to keyframe array is nullptr.");
        return false;
    }

    // Get pointer to interpolated keyframes array
    auto interpolKeyframes = ccc->getInterpolCamPositions();
    if (interpolKeyframes == nullptr) {
        vislib::sys::Log::DefaultLog.WriteWarn("[CINEMATIC RENDERER] [Render] Pointer to interpolated camera positions array is nullptr.");
        return false;
    }

    // Draw slave renderer stuff ----------------------------------------------

// Suppress TRACE output of fbo.Enable() and fbo.Create()
#if defined(DEBUG) || defined(_DEBUG)
    unsigned int otl = vislib::Trace::GetInstance().GetLevel();
    vislib::Trace::GetInstance().SetLevel(0);
#endif // DEBUG || _DEBUG 

    if (this->fbo.IsValid()) {
        if ((this->fbo.GetWidth() != vpWidth) || (this->fbo.GetHeight() != vpHeight)) {
            this->fbo.Release();
        }
    }
    if (!this->fbo.IsValid()) {
        if (!this->fbo.Create(vpWidth, vpHeight, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, vislib::graphics::gl::FramebufferObject::ATTACHMENT_TEXTURE, GL_DEPTH_COMPONENT24)) {
            throw vislib::Exception("[CINEMATIC RENDERER] [render] Unable to create image framebuffer object.", __FILE__, __LINE__);
            return false;
        }
    }
    if (this->fbo.Enable() != GL_NO_ERROR) {
        throw vislib::Exception("[CINEMATIC RENDERER] [render] Cannot enable Framebuffer object.", __FILE__, __LINE__);
        return false;
    }

// Reset TRACE output level
#if defined(DEBUG) || defined(_DEBUG)
    vislib::Trace::GetInstance().SetLevel(otl);
#endif // DEBUG || _DEBUG 

    glClearColor(bgColor[0], bgColor[1], bgColor[2], bgColor[3]);
    glClearDepth(1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    // Set data of outgoing cr3d to data of incoming cr3d
    *cr3d_out = *cr3d_in;

    // Set output buffer for override call (otherwise render call is overwritten in Base::Render(context))
    cr3d_out->SetOutputBuffer(&this->fbo);

    // Call render function of slave renderer
    (*cr3d_out)(core::view::AbstractCallRender::FnRender);

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    // Disable fbo
    if (this->fbo.IsEnabled()) {
        this->fbo.Disable();
    }

    // Draw textures ------------------------------------------------------
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDisable(GL_CULL_FACE);
    glDisable(GL_LIGHTING);
    glDisable(GL_BLEND);

    // DRAW DEPTH ---------------------------------------------------------
    glEnable(GL_DEPTH_TEST);

    this->textureShader.Enable();

    glUniform1f(this->textureShader.ParameterLocation("vpW"), (float)(vpWidth));
    glUniform1f(this->textureShader.ParameterLocation("vpH"), (float)(vpHeight));
    glUniform1i(this->textureShader.ParameterLocation("depthtex"), 0);

    this->fbo.DrawDepthTexture();

    this->textureShader.Disable();

    // DRAW COLORS --------------------------------------------------------
    glDisable(GL_DEPTH_TEST);

    this->fbo.DrawColourTexture();

    // Draw cinematic renderer stuff -------------------------------------------
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    GLfloat tmpLw;
    glGetFloatv(GL_LINE_WIDTH, &tmpLw);
    GLfloat tmpPs;
    glGetFloatv(GL_POINT_SIZE, &tmpPs);

    // MANIPULATORS
    if (keyframes->size() > 0) {

        // Update manipulator data only if currently no manipulator is grabbed
<<<<<<< HEAD
        if (!this->manipulatorGrabbed) {

            // Available manipulators
            vislib::Array<KeyframeManipulator::manipType> availManip;
            availManip.Clear();
            availManip.Add(KeyframeManipulator::manipType::KEYFRAME_POS);
            // Keyframe position (along XYZ) manipulators, spline control point
            if (this->toggleManipulator == 0) { 
                availManip.Add(KeyframeManipulator::manipType::SELECTED_KF_POS_X);
                availManip.Add(KeyframeManipulator::manipType::SELECTED_KF_POS_Y);
                availManip.Add(KeyframeManipulator::manipType::SELECTED_KF_POS_Z);
                availManip.Add(KeyframeManipulator::manipType::CTRL_POINT_POS_X);
                availManip.Add(KeyframeManipulator::manipType::CTRL_POINT_POS_Y);
                availManip.Add(KeyframeManipulator::manipType::CTRL_POINT_POS_Z);
            }
            else { 
                //if (this->toggleManipulator == 1) { 
                // Keyframe position (along lookat), lookat and up manipulators
                availManip.Add(KeyframeManipulator::manipType::SELECTED_KF_UP);
                availManip.Add(KeyframeManipulator::manipType::SELECTED_KF_LOOKAT_X);
                availManip.Add(KeyframeManipulator::manipType::SELECTED_KF_LOOKAT_Y);
                availManip.Add(KeyframeManipulator::manipType::SELECTED_KF_LOOKAT_Z);
                availManip.Add(KeyframeManipulator::manipType::SELECTED_KF_POS_LOOKAT);
            }

            // Update manipulators with current data.
            this->manipulators.Update(availManip, keyframes, skf, (float)(vpHeight), (float)(vpWidth), modelViewProjMatrix,
                (cr3d_in->GetCameraParameters()->Position().operator vislib::math::Vector<vislib::graphics::SceneSpaceType, 3U>()) -
                (cr3d_in->GetCameraParameters()->LookAt().operator vislib::math::Vector<vislib::graphics::SceneSpaceType, 3U>()),
                (cr3d_in->GetCameraParameters()->Position().operator vislib::math::Vector<vislib::graphics::SceneSpaceType, 3U>()) -
                (ccc->getBboxCenter().operator vislib::math::Vector<vislib::graphics::SceneSpaceType, 3U>()),
                this->manipOutsideModel, ccc->getStartControlPointPosition(), ccc->getEndControlPointPosition());
=======
		if (!this->manipulatorGrabbed) {

			// Available manipulators
			std::vector<KeyframeManipulator::manipType> availManip;
			availManip.clear();
			availManip.emplace_back(KeyframeManipulator::manipType::KEYFRAME_POS);

			if (this->toggleManipulator == 0) { // Keyframe position (along XYZ) manipulators, spline control point
				availManip.emplace_back(KeyframeManipulator::manipType::SELECTED_KF_POS_X);
				availManip.emplace_back(KeyframeManipulator::manipType::SELECTED_KF_POS_Y);
				availManip.emplace_back(KeyframeManipulator::manipType::SELECTED_KF_POS_Z);
				availManip.emplace_back(KeyframeManipulator::manipType::CTRL_POINT_POS_X);
				availManip.emplace_back(KeyframeManipulator::manipType::CTRL_POINT_POS_Y);
				availManip.emplace_back(KeyframeManipulator::manipType::CTRL_POINT_POS_Z);
			}
			else { //if (this->toggleManipulator == 1) { // Keyframe position (along lookat), lookat and up manipulators
				availManip.emplace_back(KeyframeManipulator::manipType::SELECTED_KF_UP);
				availManip.emplace_back(KeyframeManipulator::manipType::SELECTED_KF_LOOKAT_X);
				availManip.emplace_back(KeyframeManipulator::manipType::SELECTED_KF_LOOKAT_Y);
				availManip.emplace_back(KeyframeManipulator::manipType::SELECTED_KF_LOOKAT_Z);
				availManip.emplace_back(KeyframeManipulator::manipType::SELECTED_KF_POS_LOOKAT);
			}

			// Get current Model-View-Projection matrix for world space to screen space projection of keyframe camera position for mouse selection
			view::Camera_2 cam;
			cr3d_in->GetCamera(cam);
			cam_type::snapshot_type snapshot;
			cam_type::matrix_type viewTemp, projTemp;
			// Generate complete snapshot and calculate matrices
			cam.calc_matrices(snapshot, viewTemp, projTemp, thecam::snapshot_content::all);
			glm::vec4 CamPos = snapshot.position;
			glm::vec4 CamView = snapshot.view_vector;
			glm::mat4 MVP = projTemp * viewTemp;
			glm::vec4 BboxCenter = { ccc->getBboxCenter().x, ccc->getBboxCenter().y, ccc->getBboxCenter().z, 1.0f};
		
            this->manipulator.Update(availManip, keyframes, skf, (float)(vpHeight), (float)(vpWidth), MVP, (CamPos - CamView),(CamPos - BboxCenter), 
				this->manipOutsideModel, ccc->getStartControlPointPosition(), ccc->getEndControlPointPosition());
>>>>>>> 858bc042e1c54af29b2f4defd2b89d65e5c72dfd
        }

        // Draw manipulators
        this->manipulators.Draw();
    }

    // Draw spline    
    glColor4fv(sColor);
    glLineWidth(2.0f);
    glBegin(GL_LINE_STRIP);
    for (unsigned int i = 0; i < interpolKeyframes->size(); i++) {
        glVertex3fv(glm::value_ptr(interpolKeyframes->operator[](i)));
    }
    glEnd();






    glm::mat4 mat_modelview;
    glGetFloatv(GL_MODELVIEW_MATRIX, glm::value_ptr(mat_modelview));
    glm::mat4 mat_projection;
    glGetFloatv(GL_PROJECTION_MATRIX, glm::value_ptr(mat_projection));
    glm::mat4 mvp = mat_projection * mat_modelview;

    auto cam_param_pos = cr3d_in->GetCameraParameters()->Position();
    glm::vec3 cam_pos = { (float)cam_param_pos.GetX(), (float)cam_param_pos.GetY(), (float)cam_param_pos.GetZ() };


    this->utils.ClearAllQueues();
    this->utils.Smoothing(true);

    //this->utils.PushPointPrimitive(glm::vec3(2.0f, 2.0f, 10.0f), 2.0f, cam_pos, CinematicUtils::Color(CinematicUtils::Colors::KEYFRAME)); 
    //this->utils.PushLinePrimitive(glm::vec3(0.0f, 0.0f, 20.0f), glm::vec3(0.0f, 5.0f, 20.0f), 0.05f, cam_pos, CinematicUtils::Color(CinematicUtils::Colors::FONT_HIGHLIGHT));
    //this->utils.PushQuadPrimitive(glm::vec3(0.0f, 2.0f, 22.0f), 3.0f, 5.0f, cam_pos, cam_up, CinematicUtils::Color(CinematicUtils::Colors::FONT_HIGHLIGHT));
    //this->utils.PushQuadPrimitive(glm::vec3(0.0f, 0.0f, 20.0f), glm::vec3(0.0f, 10.0f, 20.0f), glm::vec3(10.0f, 10.0f, 20.0f), glm::vec3(10.0f, 0.0f, 20.0f), CinematicUtils::Color(CinematicUtils::Colors::FONT_HIGHLIGHT));

    this->utils.Push2DTexture(this->texture, glm::vec3(0.0f, 0.0f, 20.0f), glm::vec3(0.0f, 2.0f, 20.0f), glm::vec3(2.0f, 2.0f, 20.0f), glm::vec3(2.0f, 0.0f, 20.0f), true, CinematicUtils::Color(CinematicUtils::Colors::KEYFRAME));


    this->utils.DrawAllPrimitives(mvp);








    // DRAW MENU --------------------------------------------------------------
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0.0f, (float)(vpWidth), 0.0f, (float)(vpHeight), 0.0f, 1.0f);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    float vpH = (float)(vpHeight);
    float vpW = (float)(vpWidth);

    vislib::StringA leftLabel  = " TRACKING SHOT ";
    vislib::StringA midLabel   = "";
    vislib::StringA rightLabel = " [Ctrl+h] Show Help Text ";
    if (this->showHelpText) {
        rightLabel = " [Ctrl+h] Hide Help Text ";
    }

    float lbFontSize        = (CC_MENU_HEIGHT); 
    float leftLabelWidth    = this->theFont.LineWidth(lbFontSize, leftLabel);
    float midleftLabelWidth = this->theFont.LineWidth(lbFontSize, midLabel);
    float rightLabelWidth   = this->theFont.LineWidth(lbFontSize, rightLabel);

    // Adapt font size if height of menu text is greater than menu height
    float vpWhalf = vpW / 2.0f;
    while (((leftLabelWidth + midleftLabelWidth/2.0f) > vpWhalf) || ((rightLabelWidth + midleftLabelWidth / 2.0f) > vpWhalf)) {
        lbFontSize -= 0.5f;
        leftLabelWidth    = this->theFont.LineWidth(lbFontSize, leftLabel);
        midleftLabelWidth = this->theFont.LineWidth(lbFontSize, midLabel);
        rightLabelWidth   = this->theFont.LineWidth(lbFontSize, rightLabel);
    }

    // Draw menu background
    glColor4fv(menu);
    glBegin(GL_QUADS);
        glVertex2f(0.0f, vpH);
        glVertex2f(0.0f, vpH - (CC_MENU_HEIGHT));
        glVertex2f(vpW,  vpH - (CC_MENU_HEIGHT));
        glVertex2f(vpW,  vpH);
    glEnd();

    // Draw menu labels
    float labelPosY = vpH - (CC_MENU_HEIGHT) / 2.0f + lbFontSize / 2.0f;
    this->theFont.DrawString(white, 0.0f, labelPosY, lbFontSize, false, leftLabel, megamol::core::utility::AbstractFont::ALIGN_LEFT_TOP);
    this->theFont.DrawString(yellow, (vpW - midleftLabelWidth) / 2.0f, labelPosY, lbFontSize, false, midLabel, megamol::core::utility::AbstractFont::ALIGN_LEFT_TOP);
    this->theFont.DrawString(white, (vpW - rightLabelWidth), labelPosY, lbFontSize, false, rightLabel, megamol::core::utility::AbstractFont::ALIGN_LEFT_TOP);

    // Draw help text 
    if (this->showHelpText) {
        vislib::StringA helpText = "";
        helpText += "-----[ GLOBAL ]-----\n";
        helpText += "[Ctrl+a] Apply current settings to selected/new keyframe. \n";
        helpText += "[Ctrl+d] Delete selected keyframe. \n";
        helpText += "[Ctrl+s] Save keyframes to file. \n";
        helpText += "[Ctrl+l] Load keyframes from file. \n";
        helpText += "[Ctrl+z] Undo keyframe changes. \n";
        helpText += "[Ctrl+y] Redo keyframe changes. \n";
        helpText += "-----[ TRACKING SHOT ]----- \n";
        helpText += "[Ctrl+q] Toggle different manipulators for the selected keyframe. \n";
        helpText += "[Ctrl+w] Show manipulators inside/outside of model bounding box. \n";
        helpText += "[Ctrl+u] Reset look-at vector of selected keyframe. \n";
        helpText += "-----[ CINEMATIC ]----- \n";
        helpText += "[Ctrl+r] Start/Stop rendering complete animation. \n";
        helpText += "[Ctrl+Space] Start/Stop animation preview. \n";
        helpText += "-----[ TIMELINE ]----- \n";
        helpText += "[Ctrl+Right/Left Arrow] Move selected keyframe on animation time axis. \n";
        helpText += "[Ctrl+f] Snap all keyframes to animation frames. \n";
        helpText += "[Ctrl+g] Snap all keyframes to simulation frames. \n";
        helpText += "[Ctrl+t] Linearize simulation time between two keyframes. \n";
        //helpText += "[Ctrl+v] Set same velocity between all keyframes (Experimental).\n"; // Calcualation is not correct yet ...
        helpText += "[Ctrl+p] Reset shifted and scaled time axes. \n";
        helpText += "[Left Mouse Button] Select keyframe. \n";
        helpText += "[Middle Mouse Button] Axes scaling in mouse direction. \n";
        helpText += "[Right Mouse Button] Drag & drop keyframe / pan axes. \n";

        float htNumOfRows = 24.0f; // Number of rows the help text has

        float htFontSize  = vpW*0.027f; // max % of viewport width
        float htStrHeight = this->theFont.LineHeight(htFontSize);
        float htX         = 5.0f;
        float htY         = htX + htStrHeight;
        // Adapt font size if height of help text is greater than viewport height
        while ((htStrHeight*htNumOfRows + htX + this->theFont.LineHeight(lbFontSize)) >vpH) {
            htFontSize -= 0.5f;
            htStrHeight = this->theFont.LineHeight(htFontSize);
        }

        float htStrWidth = this->theFont.LineWidth(htFontSize, helpText);
        htStrHeight      = this->theFont.LineHeight(htFontSize);
        htY              = htX + htStrHeight*htNumOfRows;
        // Draw background colored quad
        glColor4fv(bgColor);
        glBegin(GL_QUADS);
            glVertex2f(htX,              htY);
            glVertex2f(htX,              htY - (htStrHeight*htNumOfRows));
            glVertex2f(htX + htStrWidth, htY - (htStrHeight*htNumOfRows));
            glVertex2f(htX + htStrWidth, htY);
        glEnd();
        // Draw help text
        this->theFont.DrawString(fgColor, htX, htY, htFontSize, false, helpText, megamol::core::utility::AbstractFont::ALIGN_LEFT_TOP);
    }

    // ------------------------------------------------------------------------
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    // Reset opengl
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glLineWidth(tmpLw);
    glPointSize(tmpPs);

    return true;
}


bool TrackingShotRenderer::OnMouseButton(megamol::core::view::MouseButton button, megamol::core::view::MouseButtonAction action, megamol::core::view::Modifiers mods) {

    auto* cr = this->rendererCallerSlot.CallAs<view::CallRender3D_2>();
    if (cr) {
        InputEvent evt;
        evt.tag = InputEvent::Tag::MouseButton;
        evt.mouseButtonData.button = button;
        evt.mouseButtonData.action = action;
        evt.mouseButtonData.mods = mods;
        cr->SetInputEvent(evt);
        if ((*cr)(view::CallRender3D_2::FnOnMouseButton)) return true;
    }

    CallKeyframeKeeper *ccc = this->keyframeKeeperSlot.CallAs<CallKeyframeKeeper>();
    if (ccc == nullptr) return false;
    auto keyframes = ccc->getKeyframes();
    if (keyframes == nullptr) {
        vislib::sys::Log::DefaultLog.WriteWarn("[CINEMATIC RENDERER] [MouseEvent] Pointer to keyframe array is nullptr.");
        return false;
    }

    bool consumed = false;

    bool down = (action == core::view::MouseButtonAction::PRESS);
    if (button == MouseButton::BUTTON_LEFT) {
        if (down) {
            // Check if manipulator is selected
            if (this->manipulators.CheckManipulatorHit(this->mouseX, this->mouseY)) {
                this->manipulatorGrabbed = true;
                consumed = true;
                //vislib::sys::Log::DefaultLog.WriteWarn("[CINEMATIC RENDERER] [MouseEvent] MANIPULATOR SELECTED.");
            }
            else {
                // Check if new keyframe position is selected
                int index = this->manipulators.CheckKeyframePositionHit(this->mouseX, this->mouseY);
                if (index >= 0) {
                    ccc->setSelectedKeyframeTime((*keyframes)[index].GetAnimTime());
                    if (!(*ccc)(CallKeyframeKeeper::CallForGetSelectedKeyframeAtTime)) return false;
                    consumed = true;
                    //vislib::sys::Log::DefaultLog.WriteWarn("[CINEMATIC RENDERER] [MouseEvent] KEYFRAME SELECT.");
                }
            }
        }
        else {
            // Apply changes of selected manipulator and control points
            if (this->manipulators.ProcessManipulatorHit(this->mouseX, this->mouseY)) {

                ccc->setSelectedKeyframe(this->manipulators.GetManipulatedKeyframe());
                if (!(*ccc)(CallKeyframeKeeper::CallForSetSelectedKeyframe)) return false;

                ccc->setControlPointPosition(this->manipulators.GetFirstControlPointPosition(), this->manipulators.GetLastControlPointPosition());
                if (!(*ccc)(CallKeyframeKeeper::CallForSetCtrlPoints)) return false;

                consumed = true;
                //vislib::sys::Log::DefaultLog.WriteWarn("[CINEMATIC RENDERER] [MouseEvent] MANIPULATOR CHANGED.");
            }
            // ! Mode MUST alwasy be reset on left button 'up', if MOUSE moves out of viewport during manipulator is grabbed !
            this->manipulatorGrabbed = false;
        }
    }

    return consumed;
}


bool TrackingShotRenderer::OnMouseMove(double x, double y) {

    auto* cr = this->rendererCallerSlot.CallAs<view::CallRender3D_2>();
    if (cr) {
        InputEvent evt;
        evt.tag = InputEvent::Tag::MouseMove;
        evt.mouseMoveData.x = x;
        evt.mouseMoveData.y = y;
        cr->SetInputEvent(evt);
        if ((*cr)(view::CallRender3D_2::FnOnMouseMove))  return true;
    }

    // Just store current mouse position
    this->mouseX = (float)static_cast<int>(x);
    this->mouseY = (float)static_cast<int>(y);

    // Update position of grabbed manipulator
    if (!(this->manipulatorGrabbed && this->manipulators.ProcessManipulatorHit(this->mouseX, this->mouseY))) {
        return false;
    }

    return true;
}
