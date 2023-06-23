PER_CASE_DURATION = 5 -- seconds

RESOLUTIONS = {480, 720, 1080, 1440, 2160}

function render(seconds) 
    local wake_up_time = tonumber(os.clock() + seconds); 
    while (os.clock() < wake_up_time) do 
		mmRenderNextFrame()
    end 
end

-- Project init
mmCreateView("GraphEntry_1","View2DGL","::View2DGL_1")

mmCreateModule("ParallelCoordinatesRenderer2D","::ParallelCoordinatesRenderer2D_1")
mmCreateModule("CSVDataSource","::CSVDataSource_1")
mmCreateModule("TransferFunctionGL","::TransferFunction_1")
mmCreateModule("UniFlagStorageGL","::UniFlagStorageGL_1")

mmCreateCall("CallRender2DGL","::View2DGL_1::rendering","::ParallelCoordinatesRenderer2D_1::rendering")
mmCreateCall("TableDataCall","::ParallelCoordinatesRenderer2D_1::getData","::CSVDataSource_1::getData")
mmCreateCall("CallGetTransferFunctionGL","::ParallelCoordinatesRenderer2D_1::getTransferFunction","::TransferFunction_1::gettransferfunction")
mmCreateCall("FlagCallRead_GL","::ParallelCoordinatesRenderer2D_1::readFlagStorage","::UniFlagStorageGL_1::readFlags")
mmCreateCall("FlagCallWrite_GL","::ParallelCoordinatesRenderer2D_1::writeFlagStorage","::UniFlagStorageGL_1::writeFlags")

mmSetParamValue("::CSVDataSource_1::filename",[=[E:\B.Sc.Arbeit\megamol\build\install\examples\sampledata\csv_sequence\testspheres.0.csv]=])
mmSetParamValue("::CSVDataSource_1::colSep",[=[,]=])
mmSetParamValue("::CSVDataSource_1::decSep",[=[US (3.141)]=])

-- Additional config
--mmSetGuiVisible(false)

-- Setup file for timestamps
timestamp_header = "Test Case Name, Timestamp Meaning, Timestamp (ms)\n"
timestamp_file = timestamp_header


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
    test_case_name = string.format("%04d", height) -- expand for other parameters
	
	-- Optionally store a screenshot of the view (or window)
    --mmScreenshotEntryPoint( "::View2DGL_1", testname .. ".png")
    --mmScreenshot(testname .. ".png")
	
	-- Render a few dummy frames as warmup
    for i = 1, 10 do -- is this necessary in this case?
      mmRenderNextFrame()
    end
	
	-- Discard as many in-between-samples as possible
	mmSwapPowerlogBuffers()
	
	-- Run bechmark and store start/end timestamps (power logging happens within megamol)
	timestamp_file = timestamp_file .. test_case_name .. "," .. "start" .. "," .. mmGetPowerTimeStamp() .. "\n"
	render(PER_CASE_DURATION)
	timestamp_file = timestamp_file .. test_case_name .. "," .. "end" .. "," .. mmGetPowerTimeStamp() .. "\n"
	
	-- Flush sample buffer into powerlog file
	mmFlushPowerlog()
	
end

-- Write timestamp file
mmWriteTextFile(mmGetInstanceName() .. ".csv", timestamp_file)

-- Quit
mmQuit()