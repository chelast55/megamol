GENERAL_TEST_CASE_NAME = "glsl_spheres"
PER_CASE_DURATION = 5 -- seconds
NVPERF = true -- disable for other manufacturers to save time

DATASET_DIRS = {
	[=[E:/B.Sc.Arbeit/daten/]=]
}
RESOLUTIONS = {720, 1080, 1440, 2160}
CAMERA_ANGLE_COUNT = 8
THINNING_FACTORS = {
	{
		--[[{1, "expl30m_thin1.mmpld"},
		{2, "expl30m_thin2.mmpld"},
		{4, "expl30m_thin4.mmpld"},
		{8, "expl30m_thin8.mmpld"},
		{16, "expl30m_thin16.mmpld"}]]--
		{1, "expl30m_bin_fix_a-90.mmpld"},
		{2, "expl30m_bin_fix_a-90.mmpld"},
		{4, "expl30m_bin_fix_a-90.mmpld"},
		{8, "expl30m_bin_fix_a-90.mmpld"},
		{16, "expl30m_bin_fix_a-90.mmpld"}
	}
}
RENDER_METHODS = {"Simple", "SSBO_Stream"}
SPHERE_RADII = {1, 2}

function splitString(input_string, separator)
	local split_string = {}
	for substring in string.gmatch(input_string, "([^"..separator.."]+)") do
		table.insert(split_string, substring)
	end
	return split_string
end

function render(seconds) 
	local frame_counter = 0
    local wake_up_time = tonumber(os.clock() + seconds); 
    while (os.clock() < wake_up_time) do 
		mmRenderNextFrame()
		frame_counter = frame_counter + 1
	end 
	return frame_counter
end

function runNvPerf(nvperf_path)
	mmNVPerfInit("./" .. nvperf_path:gsub(":", "_"):gsub("|", "_") ..  "/")
	while mmNVPerfIsCollecting() do
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

mmRenderNextFrame()

-- Additional config
mmSetGUIVisible(false)

-- Setup file for timestamps
timestamp_header = "Test Case Type|Dataset|Resolution|Camera Angle|Thinning Factor|Sphere Radius,Sample Timestamp (ms),Num. Frames Rendered\n"
timestamp_file = timestamp_header

-- Run all test cases once with powerlog and once with nvperf
for test_cases_iteration = 0, 1 do
	if(not NVPERF)
	then
		test_cases_iteration = 1
	end

	-- Loop over dataset parameters
	for dataset_index = 1, #DATASET_DIRS do
		dataset_dir = DATASET_DIRS[dataset_index]
		print("Dataset: " .. dataset_dir)

		-- Loop over thinning factors
		for _, thinning_factor in ipairs(THINNING_FACTORS[dataset_index]) do
			mmSetParamValue("::data::filename", dataset_dir .. thinning_factor[2])
			mmRenderNextFrame()
			print("Thinning Factor: " .. thinning_factor[1])

			-- Loop over render methods
			for _, render_method in ipairs(RENDER_METHODS) do
				mmSetParamValue("::renderer::renderMode", render_method)
				if(render_method == "SSBO_Stream")
				then
					mmSetParamValue("::renderer::ssbo::staticData", [=[true]=])
				end
				print("Render Method: " .. render_method)

				-- Loop over sphere radii
				for _, sphere_radius in ipairs(SPHERE_RADII) do
					mmSetParamValue("::renderer::scaling", sphere_radius)
					print("Sphere Radius: " .. sphere_radius)

					-- Loop over resolution parameter
					for _, height in ipairs(RESOLUTIONS) do

						-- Set resolution to view framebuffer and window
						width = math.ceil(height * (16/9))
						mmSetViewFramebufferSize("::view", width, height)
						mmSetWindowFramebufferSize(width, height)
						mmRenderNextFrame() -- Always render a dummy frame to apply changes
						print("Resolution: " .. width .. "x" .. height)
						
						-- View reset to center screen after resolution change
						mmSetParamValue("::view::view::resetView", [=[true]=])
						mmRenderNextFrame()

						-- generate camera angles
						camera_angles = splitString(mmGenerateCameraScenes("::view", "orbit", CAMERA_ANGLE_COUNT), "}")
						for i = 1,CAMERA_ANGLE_COUNT do
							camera_angles[i] = string.sub(camera_angles[i], 2)
							camera_angles[i] = camera_angles[i] .. "}"
						end
						
						-- Loop for different camera angles
						for i=1,CAMERA_ANGLE_COUNT do
							print("Camera Angle Nr. " .. i)
						
							-- create a unique filename for the current parameter set
							split_path = splitString(dataset_dir, "/")
							dataset_name = split_path[#split_path]
							test_case_name = GENERAL_TEST_CASE_NAME .. "|" .. dataset_name .. "|" .. height .. "p" .. "|" .. "cam" .. i .. "|" .. thinning_factor[1] .. "|" .. sphere_radius
							
							-- set camera angle
							mmSetParamValue("::view::camstore::settings", camera_angles[i])
							mmSetParamValue("::view::camstore::restorecam", [=[true]=])
							mmRenderNextFrame()
							
							-- Optionally store a screenshot of the view (or window)
							--mmScreenshotEntryPoint( "::view", test_case_name .. ".png")
							--mmScreenshot(test_case_name .. ".png")
							
							-- Render a few dummy frames as warmup
							for i = 1, 10 do
							mmRenderNextFrame()
							end
							
							if(test_cases_iteration == 1)
							then
								-- Run bechmark and store start/end timestamps (power logging happens within megamol)
								timestamp_file = timestamp_file .. test_case_name .. "|" .. "start" .. "," .. mmGetPowerTimeStamp() .. ",\n"
								num_frames_rendered = render(PER_CASE_DURATION)
								timestamp_file = timestamp_file .. test_case_name .. "|" .. "end" .. "," .. mmGetPowerTimeStamp() .. "," .. num_frames_rendered .. "\n"

								-- Flush sample buffer into powerlog file
								mmFlushPowerlog()

								print(num_frames_rendered .. " frames rendered")
							else
								runNvPerf(test_case_name)

								-- Flush to prevent it from filling up before power test cases
								mmFlushPowerlog()
							end

						end
					end
				end
			end
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
mmWriteTextFile(mmGetInstanceName() .. "-" .. GENERAL_TEST_CASE_NAME .. "_" .. "camera_angles" .. ".csv", angle_file)

-- Quit
mmQuit()
