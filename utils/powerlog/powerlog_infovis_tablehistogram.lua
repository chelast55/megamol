GENERAL_TEST_CASE_NAME = "infovis_tablehistogram"
PER_CASE_DURATION = 5 -- seconds

DATASETS = {
	[=[T:/schmidtm/daten/iris.mmft]=],
	[=[T:/schmidtm/daten/Balken.mmft]=],
	[=[T:/schmidtm/daten/Drops.mmft]=]
}
RESOLUTIONS = {480, 720, 1080, 1440, 2160}

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

mmCheckVersion("708876776a2dacf9-dirty")
mmCreateView("infovis","View2DGL","::View2DGL_1")

mmCreateModule("TableHistogramRenderer2D","::TableHistogramRenderer2D_1")
mmCreateModule("TransferFunctionGL","::TransferFunction_1")
mmCreateModule("UniFlagStorageGL","::UniFlagStorageGL_1")
mmCreateModule("MMFTDataSource","::MMFTDataSource_1")

mmCreateCall("CallRender2DGL","::View2DGL_1::rendering","::TableHistogramRenderer2D_1::rendering")
mmCreateCall("CallGetTransferFunctionGL","::TableHistogramRenderer2D_1::getTransferFunction","::TransferFunction_1::gettransferfunction")
mmCreateCall("TableDataCall","::TableHistogramRenderer2D_1::getData","::MMFTDataSource_1::getData")
mmCreateCall("FlagCallRead_GL","::TableHistogramRenderer2D_1::readFlagStorage","::UniFlagStorageGL_1::readFlags")
mmCreateCall("FlagCallWrite_GL","::TableHistogramRenderer2D_1::writeFlagStorage","::UniFlagStorageGL_1::writeFlags")

-- Additional config
mmSetGUIVisible(false)

-- Setup file for timestamps
timestamp_header = "Sensor Name,Sample Timestamp (ms),Num. Frames Rendered\n"
timestamp_file = timestamp_header

-- Loop over dataset parameters
for _, dataset_path in ipairs(DATASETS) do
	mmSetParamValue("::MMFTDataSource_1::filename", dataset_path)
	mmRenderNextFrame()
		
	-- Loop over resolution parameter
	for _, height in ipairs(RESOLUTIONS) do

		-- Set resolution to view framebuffer and window
		width = math.ceil(height * (16/9))
		mmSetViewFramebufferSize("::View2DGL_1", width, height)
		mmSetWindowFramebufferSize(width, height)
		mmRenderNextFrame() -- Always render a dummy frame to apply changes
		  
		-- View reset to center screen after resolution change
		mmSetParamValue("::View2DGL_1::view::resetView", [=[true]=])
		mmRenderNextFrame()
		
		-- create a unique filename for the current parameter set
		split_path = splitString(dataset_path, "/")
		dataset_name = splitString(split_path[#split_path], ".")[1]
		test_case_name = GENERAL_TEST_CASE_NAME .. "|" .. dataset_name .. "|" .. height .. "p|"
		
		-- Optionally store a screenshot of the view (or window)
		--mmScreenshotEntryPoint( "::View2DGL_1", test_case_name .. ".png")
		--mmScreenshot(test_case_name .. ".png")
		
		-- Render a few dummy frames as warmup
		for i = 1, 10 do
		  mmRenderNextFrame()
		end
		
		-- Discard as many in-between-samples as possible
		--mmSwapPowerlogBuffers()
		
		-- Run bechmark and store start/end timestamps (power logging happens within megamol)
		timestamp_file = timestamp_file .. test_case_name .. "|" .. "start" .. "," .. mmGetPowerTimeStamp() .. ",\n"
		num_frames_rendered = render(PER_CASE_DURATION)
		timestamp_file = timestamp_file .. test_case_name .. "|" .. "end" .. "," .. mmGetPowerTimeStamp() .. "," .. num_frames_rendered .. "\n"
		
		-- Flush sample buffer into powerlog file
		mmFlushPowerlog()

		print(num_frames_rendered .. " frames rendered")
		
	end
end

-- Write timestamp file
mmWriteTextFile(mmGetInstanceName() .. "-" .. GENERAL_TEST_CASE_NAME .. ".csv", timestamp_file)

-- Quit
mmQuit()
