GENERAL_TEST_CASE_NAME = "infovis"
PER_CASE_DURATION = 5 -- seconds

DATASETS = {
	[=[E:/B.Sc.Arbeit/daten/iris.mmft]=],
	[=[E:/B.Sc.Arbeit/daten/Balken.mmft]=],
	[=[E:/B.Sc.Arbeit/daten/Drops.mmft]=]
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
    local wake_up_time = tonumber(os.clock() + seconds); 
    while (os.clock() < wake_up_time) do 
		mmRenderNextFrame()
    end 
end

mmCheckVersion("708876776a2dacf9-dirty")
mmCreateView("infovis","SplitViewGL","::SplitViewGL_1")

mmCreateModule("SplitViewGL","::SplitViewGL_2")
mmCreateModule("View2DGL","::View2DGL_1")
mmCreateModule("View2DGL","::View2DGL_2")
mmCreateModule("View2DGL","::View2DGL_3")
mmCreateModule("ScatterplotMatrixRenderer2D","::ScatterplotMatrixRenderer2D_1")
mmCreateModule("ParallelCoordinatesRenderer2D","::ParallelCoordinatesRenderer2D_1")
mmCreateModule("TableHistogramRenderer2D","::TableHistogramRenderer2D_1")
mmCreateModule("TransferFunctionGL","::TransferFunction_1")
mmCreateModule("UniFlagStorageGL","::UniFlagStorageGL_1")
mmCreateModule("MMFTDataSource","::MMFTDataSource_1")

mmCreateCall("CallRenderViewGL","::SplitViewGL_1::render1","::View2DGL_1::render")
mmCreateCall("CallRenderViewGL","::SplitViewGL_1::render2","::SplitViewGL_2::render")
mmCreateCall("CallRenderViewGL","::SplitViewGL_2::render1","::View2DGL_2::render")
mmCreateCall("CallRenderViewGL","::SplitViewGL_2::render2","::View2DGL_3::render")
mmCreateCall("CallRender2DGL","::View2DGL_1::rendering","::ScatterplotMatrixRenderer2D_1::rendering")
mmCreateCall("CallRender2DGL","::View2DGL_2::rendering","::ParallelCoordinatesRenderer2D_1::rendering")
mmCreateCall("CallRender2DGL","::View2DGL_3::rendering","::TableHistogramRenderer2D_1::rendering")
mmCreateCall("TableDataCall","::ScatterplotMatrixRenderer2D_1::ftIn","::MMFTDataSource_1::getData")
mmCreateCall("CallGetTransferFunctionGL","::ScatterplotMatrixRenderer2D_1::tfIn","::TransferFunction_1::gettransferfunction")
mmCreateCall("FlagCallRead_GL","::ScatterplotMatrixRenderer2D_1::readFlags","::UniFlagStorageGL_1::readFlags")
mmCreateCall("FlagCallWrite_GL","::ScatterplotMatrixRenderer2D_1::writeFlags","::UniFlagStorageGL_1::writeFlags")
mmCreateCall("TableDataCall","::ParallelCoordinatesRenderer2D_1::getData","::MMFTDataSource_1::getData")
mmCreateCall("CallGetTransferFunctionGL","::ParallelCoordinatesRenderer2D_1::getTransferFunction","::TransferFunction_1::gettransferfunction")
mmCreateCall("FlagCallRead_GL","::ParallelCoordinatesRenderer2D_1::readFlagStorage","::UniFlagStorageGL_1::readFlags")
mmCreateCall("FlagCallWrite_GL","::ParallelCoordinatesRenderer2D_1::writeFlagStorage","::UniFlagStorageGL_1::writeFlags")
mmCreateCall("CallGetTransferFunctionGL","::TableHistogramRenderer2D_1::getTransferFunction","::TransferFunction_1::gettransferfunction")
mmCreateCall("TableDataCall","::TableHistogramRenderer2D_1::getData","::MMFTDataSource_1::getData")
mmCreateCall("FlagCallRead_GL","::TableHistogramRenderer2D_1::readFlagStorage","::UniFlagStorageGL_1::readFlags")
mmCreateCall("FlagCallWrite_GL","::TableHistogramRenderer2D_1::writeFlagStorage","::UniFlagStorageGL_1::writeFlags")

mmSetParamValue("::SplitViewGL_2::split.orientation",[=[Vertical]=])

-- Additional config
--mmSetGuiVisible(true)

-- Setup file for timestamps
timestamp_header = "Sensor Name,Sample Timestamp (ms),Momentary Power Comsumption (W)\n"
timestamp_file = timestamp_header

-- Loop over dataset parameters
for _, dataset_path in ipairs(DATASETS) do
	mmSetParamValue("::MMFTDataSource_1::filename", dataset_path)
	mmRenderNextFrame()
		
	-- Loop over resolution parameter
	for _, height in ipairs(RESOLUTIONS) do

		-- Set resolution to view framebuffer and window
		width = math.ceil(height * (16/9))
		mmSetViewFramebufferSize("::SplitViewGL_1", width, height)
		mmSetWindowFramebufferSize(width, height)
		mmRenderNextFrame() -- Always render a dummy frame to apply changes
		  
		-- View reset to center screen after resolution change
		mmSetParamValue("::SplitViewGL_1::view::resetView", [=[true]=])
		mmRenderNextFrame()
		
		-- create a unique filename for the current parameter set
		split_path = splitString(dataset_path, "/")
		dataset_name = splitString(split_path[#split_path], ".")[1]
		test_case_name = GENERAL_TEST_CASE_NAME .. "_" .. dataset_name .. "_" .. height .. "p"
		
		-- Optionally store a screenshot of the view (or window)
		--mmScreenshotEntryPoint( "::SplitViewGL_1", test_case_name .. ".png")
		--mmScreenshot(test_case_name .. ".png")
		
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

-- Write timestamp file
mmWriteTextFile(mmGetInstanceName() .. "-" .. GENERAL_TEST_CASE_NAME .. ".csv", timestamp_file)

-- Quit
mmQuit()
