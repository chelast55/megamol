GENERAL_TEST_CASE_NAME = "glsl_spheres"
PER_CASE_DURATION = 5 -- seconds

DATASETS = {
	[=[T:/schmidtm/daten/expl30m_bin_fix_a-90.mmpld]=]
}
RESOLUTIONS = {480, 720, 1080, 1440, 2160}
CAMERA_ANGLE_COUNT = 5

function splitString(input_string, separator)
	local split_string = {}
	for substring in string.gmatch(input_string, "([^"..separator.."]+)") do
		table.insert(split_string, substring)
	end
	return split_string
end

function render(seconds) 
    local wake_up_time = tonumber(os.clock() + seconds); 
    while (os.clock() < wake_up_time) do 
		mmRenderNextFrame()
    end 
end

mmCheckVersion("9a28163a595e88b4-dirty") 
mmCreateView("testspheres","View3DGL","::view")

mmCreateModule("BoundingBoxRenderer","::bbox")
mmCreateModule("DistantLight","::distantlight")
mmCreateModule("SphereRenderer","::renderer")
mmCreateModule("MMPLDDataSource","::data")

mmCreateCall("CallRender3DGL","::view::rendering","::bbox::rendering")
mmCreateCall("CallRender3DGL","::bbox::chainRendering","::renderer::rendering")
mmCreateCall("MultiParticleDataCall","::renderer::getdata","::data::getdata")
mmCreateCall("CallLight","::renderer::lights","::distantlight::deployLightSlot")


mmSetGUIState([=[{"GraphStates":{"Project":{"Modules":{"::bbox":{"graph_position":[275.0,64.0]},"::data":{"graph_position":[790.0,49.0]},"::distantlight":{"graph_position":[797.0,160.0]},"::renderer":{"graph_position":[540.0,64.0]},"::view":{"graph_position":[64.0,64.0]}},"call_coloring_map":0,"call_coloring_mode":0,"canvas_scrolling":[0.0,0.0],"canvas_zooming":1.0,"param_extended_mode":false,"parameter_sidebar_width":300.0,"profiling_bar_height":300.0,"project_name":"Project_2","show_call_label":true,"show_call_slots_label":false,"show_grid":false,"show_module_label":true,"show_parameter_sidebar":false,"show_profiling_bar":false,"show_slot_label":false}},"ParameterStates":{"::bbox::boundingBoxColor":{"gui_presentation_mode":8,"gui_read-only":false,"gui_visibility":true},"::bbox::enableBoundingBox":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":true},"::bbox::enableViewCube":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":true},"::bbox::smoothLines":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":true},"::bbox::viewCubePosition":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":true},"::bbox::viewCubeSize":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":true},"::data::filename":{"gui_presentation_mode":16,"gui_read-only":false,"gui_visibility":true},"::data::limitMemory":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":true},"::data::limitMemorySize":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":true},"::data::overrideLocalBBox":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":true},"::distantlight::AngularDiameter":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":true},"::distantlight::Color":{"gui_presentation_mode":8,"gui_read-only":false,"gui_visibility":true},"::distantlight::Direction":{"gui_presentation_mode":512,"gui_read-only":false,"gui_visibility":true},"::distantlight::EyeDirection":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":true},"::distantlight::Intensity":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":true},"::renderer::ambient occlusion::apex":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":true},"::renderer::ambient occlusion::coneLength":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":true},"::renderer::ambient occlusion::enableLighting":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":true},"::renderer::ambient occlusion::highPrecisionTexture":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":true},"::renderer::ambient occlusion::offset":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":true},"::renderer::ambient occlusion::strength":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":true},"::renderer::ambient occlusion::useGsProxies":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":true},"::renderer::ambient occlusion::volumeSize":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":true},"::renderer::flag storage::selectedColor":{"gui_presentation_mode":8,"gui_read-only":false,"gui_visibility":true},"::renderer::flag storage::softSelectedColor":{"gui_presentation_mode":8,"gui_read-only":false,"gui_visibility":true},"::renderer::forceTime":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":true},"::renderer::outline::width":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":true},"::renderer::renderMode":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":true},"::renderer::scaling":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":true},"::renderer::splat::alphaScaling":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":true},"::renderer::splat::attenuateSubpixel":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":true},"::renderer::ssbo::staticData":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":true},"::renderer::useLocalBbox":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":true},"::view::anim::SpeedDown":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":true},"::view::anim::SpeedUp":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":true},"::view::anim::play":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":true},"::view::anim::speed":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":true},"::view::anim::time":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":true},"::view::anim::togglePlay":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":true},"::view::backCol":{"gui_presentation_mode":8,"gui_read-only":false,"gui_visibility":true},"::view::cam::farplane":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":true},"::view::cam::halfaperturedegrees":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":true},"::view::cam::nearplane":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":true},"::view::cam::orientation":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":true},"::view::cam::position":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":true},"::view::cam::projectiontype":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":true},"::view::camstore::autoLoadSettings":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":true},"::view::camstore::autoSaveSettings":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":true},"::view::camstore::overrideSettings":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":true},"::view::camstore::restorecam":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":true},"::view::camstore::settings":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":true},"::view::camstore::storecam":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":true},"::view::resetViewOnBBoxChange":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":true},"::view::showLookAt":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":true},"::view::view::cubeOrientation":{"gui_presentation_mode":2,"gui_read-only":true,"gui_visibility":false},"::view::view::defaultOrientation":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":true},"::view::view::defaultView":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":true},"::view::view::resetView":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":true},"::view::view::showViewCube":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":true},"::view::viewKey::AngleStep":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":true},"::view::viewKey::FixToWorldUp":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":true},"::view::viewKey::MouseSensitivity":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":true},"::view::viewKey::MoveStep":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":true},"::view::viewKey::RotPoint":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":true},"::view::viewKey::RunFactor":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":true}}}]=])
mmRenderNextFrame()

-- Additional config
mmSetGUIVisible(false)

-- Setup file for timestamps
timestamp_header = "Sensor Name,Sample Timestamp (ms),Momentary Power Comsumption (W)\n"
timestamp_file = timestamp_header

-- Loop over dataset parameters
for _, dataset_path in ipairs(DATASETS) do
	mmSetParamValue("::data::filename", dataset_path)
	mmRenderNextFrame()

	-- Loop over resolution parameter
	for _, height in ipairs(RESOLUTIONS) do

		-- Set resolution to view framebuffer and window
		width = math.ceil(height * (16/9))
		mmSetViewFramebufferSize("::view", width, height)
		mmSetWindowFramebufferSize(width, height)
		mmRenderNextFrame() -- Always render a dummy frame to apply changes
		  
		-- View reset to center screen after resolution change
		mmSetParamValue("::view::view::resetView", [=[true]=])
		mmRenderNextFrame()

		-- generate camera angles
		camera_angles = splitString(mmGenerateCameraScenes("::view", "orbit", 5), "}")
		for i = 1,CAMERA_ANGLE_COUNT do
			camera_angles[i] = string.sub(camera_angles[i], 2)
			camera_angles[i] = camera_angles[i] .. "}"
		end
		
		-- Loop for different camera angles
		for i=1,CAMERA_ANGLE_COUNT do
		
			-- create a unique filename for the current parameter set
			split_path = splitString(dataset_path, "/")
			dataset_name = splitString(split_path[#split_path], ".")[1]
			test_case_name = GENERAL_TEST_CASE_NAME .. "_" .. dataset_name .. "_" .. height .. "p" .. "_" .. "cam" .. i
			
			-- set camera angle
			mmSetParamValue("::view::camstore::settings", camera_angles[i])
			mmSetParamValue("::view::camstore::restorecam", [=[true]=])
			mmRenderNextFrame()
			
			-- Optionally store a screenshot of the view (or window)
			mmScreenshotEntryPoint( "::view", test_case_name .. ".png")
			mmScreenshot(test_case_name .. ".png")
			
			-- Render a few dummy frames as warmup
			for i = 1, 10 do
			  mmRenderNextFrame()
			end
			
			-- Discard as many in-between-samples as possible
			mmSwapPowerlogBuffers()
			
			-- Run bechmark and store start/end timestamps (power logging happens within megamol)
			timestamp_file = timestamp_file .. test_case_name .. "|" .. "start" .. "," .. mmGetPowerTimeStamp() .. ",\n"
			render(PER_CASE_DURATION)
			timestamp_file = timestamp_file .. test_case_name .. "|" .. "end" .. "," .. mmGetPowerTimeStamp() .. ",\n"
			
			-- Flush sample buffer into powerlog file
			mmFlushPowerlog()

		end
	end
end

-- Write timestamp file
mmWriteTextFile(mmGetInstanceName() .. "-" .. GENERAL_TEST_CASE_NAME .. ".csv", timestamp_file)

-- Generate and write angle file
angle_header = "Camera Angle;Camera Settings (JSON)\n"
angle_file = angle_header
for i = 1,CAMERA_ANGLE_COUNT do
	angle_file = angle_file .. "cam" .. i .. ";" .. camera_angles[i] .. "\n"
end
mmWriteTextFile(mmGetInstanceName() .. "-" .. GENERAL_TEST_CASE_NAME .. "_" .. "camera_angles" .. "_" .. ".csv", angle_file)

-- Quit
mmQuit()
