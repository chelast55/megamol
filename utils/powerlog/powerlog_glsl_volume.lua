GENERAL_TEST_CASE_NAME = "glsl_volume"
PER_CASE_DURATION = 5 -- seconds

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
mmCreateView("RaycastVolumeExample","View3DGL","::RaycastVolumeExample::View3DGL1")

mmCreateModule("BoundingBoxRenderer","::RaycastVolumeExample::BoundingBoxRenderer1")
mmCreateModule("RaycastVolumeRenderer","::RaycastVolumeExample::RaycastVolumeRenderer1")
mmCreateModule("TransferFunctionGL","::RaycastVolumeExample::TransferFunction1")
mmCreateModule("ScreenShooter","::RaycastVolumeExample::ScreenShooter1")
mmCreateModule("ParticlesToDensity","::RaycastVolumeExample::ParticlesToDensity_1")
mmCreateModule("MMPLDDataSource","::RaycastVolumeExample::MMPLDDataSource_1")

mmCreateCall("CallRender3DGL","::RaycastVolumeExample::View3DGL1::rendering","::RaycastVolumeExample::BoundingBoxRenderer1::rendering")
mmCreateCall("CallRender3DGL","::RaycastVolumeExample::BoundingBoxRenderer1::chainRendering","::RaycastVolumeExample::RaycastVolumeRenderer1::rendering")
mmCreateCall("VolumetricDataCall","::RaycastVolumeExample::RaycastVolumeRenderer1::getData","::RaycastVolumeExample::ParticlesToDensity_1::outData")
mmCreateCall("CallGetTransferFunctionGL","::RaycastVolumeExample::RaycastVolumeRenderer1::getTransferFunction","::RaycastVolumeExample::TransferFunction1::gettransferfunction")
mmCreateCall("MultiParticleDataCall","::RaycastVolumeExample::ParticlesToDensity_1::inData","::RaycastVolumeExample::MMPLDDataSource_1::getdata")

mmSetParamValue("::RaycastVolumeExample::View3DGL1::cam::position",[=[300;600;2949.79053]=])
mmSetParamValue("::RaycastVolumeExample::View3DGL1::cam::orientation",[=[0;-0;-0;1]=])
mmSetParamValue("::RaycastVolumeExample::View3DGL1::cam::nearplane",[=[2349.290527]=])
mmSetParamValue("::RaycastVolumeExample::View3DGL1::cam::farplane",[=[2950.290527]=])
mmSetParamValue("::RaycastVolumeExample::View3DGL1::cam::halfaperturedegrees",[=[28.647890]=])
mmSetParamValue("::RaycastVolumeExample::RaycastVolumeRenderer1::ray step ratio",[=[0.100000]=])
mmSetParamValue("::RaycastVolumeExample::TransferFunction1::TransferFunction",[=[{"IgnoreProjectRange": true,
"Interpolation": "LINEAR",
"Nodes": [
[
0.03565552830696106,
0.18527190387248993,
0.3888853192329407,
0.0,
0.0,
0.05000000074505806
],
[
0.03565552830696106,
0.18527190387248993,
0.3888853192329407,
0.2898470163345337,
0.02343749813735485,
0.05000000074505806
],
[
0.03853987902402878,
0.18783864378929138,
0.3910495638847351,
0.2961876392364502,
0.0351562537252903,
0.05000000074505806
],
[
0.047192931175231934,
0.19553887844085693,
0.39754238724708557,
0.3786161541938782,
0.0585937537252903,
0.05000000074505806
],
[
0.05151945352554321,
0.1993889957666397,
0.4007887840270996,
0.42934131622314453,
0.0703125074505806,
0.05000000074505806
],
[
0.058328256011009216,
0.20478036999702454,
0.40460970997810364,
0.4483633041381836,
0.078125,
0.05000000074505806
],
[
0.06173265725374222,
0.20747606456279755,
0.40652018785476685,
0.4673852324485779,
0.0859375,
0.05000000074505806
],
[
0.06343485414981842,
0.20882391929626465,
0.40747541189193726,
0.49274784326553345,
0.0937499925494194,
0.05000000074505806
],
[
0.07093638181686401,
0.2138247787952423,
0.4098735451698303,
0.5117697715759277,
0.1093749850988388,
0.05000000074505806
],
[
0.07996006309986115,
0.21937012672424316,
0.4119184911251068,
0.5434730648994446,
0.1132812425494194,
0.05000000074505806
],
[
0.08447190374135971,
0.2221427857875824,
0.41294097900390625,
0.5561543703079224,
0.1171875074505806,
0.05000000074505806
],
[
0.08672782778739929,
0.2235291302204132,
0.4134522080421448,
0.5561543703079224,
0.1210937649011612,
0.05000000074505806
],
[
0.08785578608512878,
0.2242223024368286,
0.41370782256126404,
0.5751763582229614,
0.125,
0.05000000074505806
],
[
0.0989656075835228,
0.23065871000289917,
0.4155274033546448,
0.600538969039917,
0.13671875,
0.05000000074505806
],
[
0.10994687676429749,
0.23701661825180054,
0.4172220826148987,
0.6259015798568726,
0.1484374850988388,
0.05000000074505806
],
[
0.1171632707118988,
0.24120095372200012,
0.41822394728660583,
0.6512641906738281,
0.1601562649011612,
0.05000000074505806
],
[
0.13112597167491913,
0.24932539463043213,
0.41965198516845703,
0.6893080472946167,
0.171875,
0.05000000074505806
],
[
0.1480960249900818,
0.2592284679412842,
0.42125266790390015,
0.695648729801178,
0.17578125,
0.05000000074505806
],
[
0.1679694950580597,
0.27090680599212646,
0.4230397939682007,
0.7019894123077393,
0.18359375,
0.05000000074505806
],
[
0.17790621519088745,
0.2767459750175476,
0.42393338680267334,
0.72735196352005,
0.1953125149011612,
0.05000000074505806
],
[
0.1922745704650879,
0.28536394238471985,
0.42514804005622864,
0.7463738918304443,
0.21875,
0.05000000074505806
],
[
0.20842798054218292,
0.2952839732170105,
0.4264482855796814,
0.7653958797454834,
0.2421875,
0.05000000074505806
],
[
0.23444315791130066,
0.3114660084247589,
0.4284842610359192,
0.7717365026473999,
0.25,
0.05000000074505806
],
[
0.26518014073371887,
0.3309333324432373,
0.4308653175830841,
0.7780771851539612,
0.2773437201976776,
0.05000000074505806
],
[
0.2930384874343872,
0.349168598651886,
0.4330628514289856,
0.7844178676605225,
0.29296875,
0.05000000074505806
],
[
0.33194732666015625,
0.37528949975967407,
0.43617570400238037,
0.790758490562439,
0.328125,
0.05000000074505806
],
[
0.36726856231689453,
0.39976710081100464,
0.4388873279094696,
0.7970991134643555,
0.34375,
0.05000000074505806
],
[
0.4166627824306488,
0.43484023213386536,
0.4425535798072815,
0.809780478477478,
0.3945312201976776,
0.05000000074505806
],
[
0.44135987758636475,
0.4523767828941345,
0.44438669085502625,
0.809780478477478,
0.41796875,
0.05000000074505806
],
[
0.4665502905845642,
0.47104161977767944,
0.4456326365470886,
0.809780478477478,
0.4335937201976776,
0.05000000074505806
],
[
0.47914549708366394,
0.4803740382194519,
0.446255624294281,
0.809780478477478,
0.4375,
0.05000000074505806
],
[
0.485443115234375,
0.48504024744033813,
0.4465671181678772,
0.809780478477478,
0.4453125,
0.05000000074505806
],
[
0.48859190940856934,
0.48737335205078125,
0.4467228651046753,
0.8034397959709167,
0.4609375,
0.05000000074505806
],
[
0.5158500075340271,
0.5083329677581787,
0.44745951890945435,
0.7717365026473999,
0.48046875,
0.05000000074505806
],
[
0.5590306520462036,
0.5419169664382935,
0.44789010286331177,
0.72735196352005,
0.515625,
0.05000000074505806
],
[
0.5806210041046143,
0.5587089657783508,
0.4481053948402405,
0.6893080472946167,
0.546875,
0.05000000074505806
],
[
0.5914161801338196,
0.5671049356460571,
0.44821304082870483,
0.6766268014907837,
0.55859375,
0.05000000074505806
],
[
0.6060336828231812,
0.5785655975341797,
0.44789910316467285,
0.6385828256607056,
0.59765625,
0.05000000074505806
],
[
0.6199184656143188,
0.5895025730133057,
0.44717299938201904,
0.61322021484375,
0.625,
0.05000000074505806
],
[
0.6340808868408203,
0.6007249355316162,
0.44575491547584534,
0.5941982865333557,
0.6640625,
0.05000000074505806
],
[
0.6443846225738525,
0.6089141368865967,
0.44438302516937256,
0.5688356757164001,
0.703125,
0.05000000074505806
],
[
0.6559814214706421,
0.6181647181510925,
0.4423714280128479,
0.5561543703079224,
0.7265625,
0.05000000074505806
],
[
0.6742902398109436,
0.6328353881835938,
0.4380340874195099,
0.5498137474060059,
0.7499999403953552,
0.05000000074505806
],
[
0.683444619178772,
0.6401707530021667,
0.4358654022216797,
0.5498137474060059,
0.7578125596046448,
0.05000000074505806
],
[
0.6880218386650085,
0.6438384056091309,
0.4347810745239258,
0.5434730648994446,
0.7773438096046448,
0.05000000074505806
],
[
0.7128841876983643,
0.6638539433479309,
0.426920086145401,
0.5371323823928833,
0.8046875,
0.05000000074505806
],
[
0.7253153920173645,
0.6738617420196533,
0.4229896068572998,
0.5307917594909668,
0.83203125,
0.05000000074505806
],
[
0.7594679594039917,
0.7014753818511963,
0.4093831181526184,
0.5307917594909668,
0.8515625,
0.05000000074505806
],
[
0.7879766225814819,
0.7245451807975769,
0.3975624144077301,
0.518110454082489,
0.875,
0.05000000074505806
],
[
0.8209182024002075,
0.7512613534927368,
0.38247108459472656,
0.4990885257720947,
0.921875,
0.05000000074505806
],
[
0.8555595874786377,
0.7794239521026611,
0.3649483621120453,
0.48640722036361694,
0.9570313096046448,
0.05000000074505806
],
[
0.8555595874786377,
0.7794239521026611,
0.3649483621120453,
0.40397876501083374,
1.0,
0.05000000074505806
]
],
"TextureSize": 256,
"ValueRange": [
0.0,
1.0
]
}]=])
mmSetParamValue("::RaycastVolumeExample::ScreenShooter1::view",[=[::RaycastVolumeExample::View3DGL1]=])
mmSetParamValue("::RaycastVolumeExample::ScreenShooter1::filename",[=[RaycastVolumeRenderer.png]=])
mmSetParamValue("::RaycastVolumeExample::ParticlesToDensity_1::sizex",[=[64]=])
mmSetParamValue("::RaycastVolumeExample::ParticlesToDensity_1::sizey",[=[256]=])
mmSetParamValue("::RaycastVolumeExample::ParticlesToDensity_1::sizez",[=[64]=])

mmSetGUIState([=[{"ConfiguratorState":{"module_list_sidebar_width":250.0,"show_module_list_sidebar":false},"GUIState":{"font_file_name":"Roboto-Regular.ttf","font_size":12.0,"global_win_background_alpha":1.0,"imgui_settings":"[Window][Configurator     F11]\nPos=481,18\nSize=1439,1062\nCollapsed=0\nDockId=0x00000004,0\n\n[Window][Parameters     F10]\nPos=0,18\nSize=479,1062\nCollapsed=0\nDockId=0x00000003,0\n\n[Window][Log Console     F9]\nCollapsed=0\nDockId=0x00000002\n\n[Window][DockSpaceViewport_11111111]\nPos=0,18\nSize=1920,1062\nCollapsed=0\n\n[Window][Debug##Default]\nPos=60,60\nSize=400,400\nCollapsed=0\n\n[Window][###fbw148]\nPos=760,290\nSize=400,500\nCollapsed=0\n\n[Window][###fbw24958]\nPos=760,290\nSize=400,500\nCollapsed=0\n\n[Window][###fbw27768]\nPos=760,290\nSize=400,500\nCollapsed=0\n\n[Window][###fbw24847]\nPos=760,290\nSize=400,500\nCollapsed=0\n\n[Docking][Data]\nDockSpace     ID=0x8B93E3BD Window=0xA787BDB4 Pos=0,18 Size=1920,1062 Split=Y\n  DockNode    ID=0x00000001 Parent=0x8B93E3BD SizeRef=1920,808 Split=X\n    DockNode  ID=0x00000003 Parent=0x00000001 SizeRef=479,808 Selected=0x75D4D3D8\n    DockNode  ID=0x00000004 Parent=0x00000001 SizeRef=1439,808 CentralNode=1 Selected=0xAE7400BF\n  DockNode    ID=0x00000002 Parent=0x8B93E3BD SizeRef=1920,270\n\n","menu_visible":true,"style":2},"GraphStates":{"Project":{"Modules":{"::RaycastVolumeExample::BoundingBoxRenderer1":{"graph_position":[332.0,112.0]},"::RaycastVolumeExample::MMPLDDataSource_1":{"graph_position":[1097.0,113.0]},"::RaycastVolumeExample::ParticlesToDensity_1":{"graph_position":[853.0,89.0]},"::RaycastVolumeExample::RaycastVolumeRenderer1":{"graph_position":[603.0,112.0]},"::RaycastVolumeExample::ScreenShooter1":{"graph_position":[96.0,216.0]},"::RaycastVolumeExample::TransferFunction1":{"graph_position":[933.0,208.0]},"::RaycastVolumeExample::View3DGL1":{"graph_position":[96.0,112.0]}},"call_coloring_map":0,"call_coloring_mode":0,"canvas_scrolling":[0.0,0.0],"canvas_zooming":1.0,"param_extended_mode":false,"parameter_sidebar_width":300.0,"profiling_bar_height":300.0,"project_name":"Project_2","show_call_label":true,"show_call_slots_label":false,"show_grid":false,"show_module_label":true,"show_parameter_sidebar":false,"show_profiling_bar":false,"show_slot_label":false}},"ParameterStates":{"::RaycastVolumeExample::BoundingBoxRenderer1::boundingBoxColor":{"gui_presentation_mode":8,"gui_read-only":false,"gui_visibility":true},"::RaycastVolumeExample::BoundingBoxRenderer1::enableBoundingBox":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":true},"::RaycastVolumeExample::BoundingBoxRenderer1::enableViewCube":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":true},"::RaycastVolumeExample::BoundingBoxRenderer1::smoothLines":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":true},"::RaycastVolumeExample::BoundingBoxRenderer1::viewCubePosition":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":true},"::RaycastVolumeExample::BoundingBoxRenderer1::viewCubeSize":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":true},"::RaycastVolumeExample::MMPLDDataSource_1::filename":{"gui_presentation_mode":16,"gui_read-only":false,"gui_visibility":true},"::RaycastVolumeExample::MMPLDDataSource_1::limitMemory":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":true},"::RaycastVolumeExample::MMPLDDataSource_1::limitMemorySize":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":true},"::RaycastVolumeExample::MMPLDDataSource_1::overrideLocalBBox":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":true},"::RaycastVolumeExample::ParticlesToDensity_1::aggregator":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":true},"::RaycastVolumeExample::ParticlesToDensity_1::cyclX":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":true},"::RaycastVolumeExample::ParticlesToDensity_1::cyclY":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":true},"::RaycastVolumeExample::ParticlesToDensity_1::cyclZ":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":true},"::RaycastVolumeExample::ParticlesToDensity_1::forSurfaceReconstruction":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":true},"::RaycastVolumeExample::ParticlesToDensity_1::normalize":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":true},"::RaycastVolumeExample::ParticlesToDensity_1::sigma":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":true},"::RaycastVolumeExample::ParticlesToDensity_1::sizex":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":true},"::RaycastVolumeExample::ParticlesToDensity_1::sizey":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":true},"::RaycastVolumeExample::ParticlesToDensity_1::sizez":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":true},"::RaycastVolumeExample::RaycastVolumeRenderer1::isovalue":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":false},"::RaycastVolumeExample::RaycastVolumeRenderer1::lighting::ambient color":{"gui_presentation_mode":8,"gui_read-only":false,"gui_visibility":true},"::RaycastVolumeExample::RaycastVolumeRenderer1::lighting::ka":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":true},"::RaycastVolumeExample::RaycastVolumeRenderer1::lighting::kd":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":true},"::RaycastVolumeExample::RaycastVolumeRenderer1::lighting::ks":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":true},"::RaycastVolumeExample::RaycastVolumeRenderer1::lighting::material color":{"gui_presentation_mode":8,"gui_read-only":false,"gui_visibility":true},"::RaycastVolumeExample::RaycastVolumeRenderer1::lighting::shininess":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":true},"::RaycastVolumeExample::RaycastVolumeRenderer1::lighting::specular color":{"gui_presentation_mode":8,"gui_read-only":false,"gui_visibility":true},"::RaycastVolumeExample::RaycastVolumeRenderer1::lighting::use lighting":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":true},"::RaycastVolumeExample::RaycastVolumeRenderer1::mode":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":true},"::RaycastVolumeExample::RaycastVolumeRenderer1::opacity":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":false},"::RaycastVolumeExample::RaycastVolumeRenderer1::opacity threshold":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":true},"::RaycastVolumeExample::RaycastVolumeRenderer1::ray step ratio":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":true},"::RaycastVolumeExample::ScreenShooter1::anim::addTime2Fname":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":true},"::RaycastVolumeExample::ScreenShooter1::anim::from":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":true},"::RaycastVolumeExample::ScreenShooter1::anim::makeAnim":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":true},"::RaycastVolumeExample::ScreenShooter1::anim::paramname":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":true},"::RaycastVolumeExample::ScreenShooter1::anim::step":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":true},"::RaycastVolumeExample::ScreenShooter1::anim::to":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":true},"::RaycastVolumeExample::ScreenShooter1::background":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":true},"::RaycastVolumeExample::ScreenShooter1::closeAfter":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":false},"::RaycastVolumeExample::ScreenShooter1::disableCompressionSlot":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":true},"::RaycastVolumeExample::ScreenShooter1::filename":{"gui_presentation_mode":16,"gui_read-only":false,"gui_visibility":true},"::RaycastVolumeExample::ScreenShooter1::imgHeight":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":true},"::RaycastVolumeExample::ScreenShooter1::imgWidth":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":true},"::RaycastVolumeExample::ScreenShooter1::tileHeight":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":false},"::RaycastVolumeExample::ScreenShooter1::tileWidth":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":false},"::RaycastVolumeExample::ScreenShooter1::trigger":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":true},"::RaycastVolumeExample::ScreenShooter1::view":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":true},"::RaycastVolumeExample::TransferFunction1::TransferFunction":{"gui_presentation_mode":32,"gui_read-only":false,"gui_visibility":true},"::RaycastVolumeExample::View3DGL1::ParameterGroup::anim":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":true},"::RaycastVolumeExample::View3DGL1::ParameterGroup::view":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":true},"::RaycastVolumeExample::View3DGL1::anim::SpeedDown":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":true},"::RaycastVolumeExample::View3DGL1::anim::SpeedUp":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":true},"::RaycastVolumeExample::View3DGL1::anim::play":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":true},"::RaycastVolumeExample::View3DGL1::anim::speed":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":true},"::RaycastVolumeExample::View3DGL1::anim::time":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":true},"::RaycastVolumeExample::View3DGL1::anim::togglePlay":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":true},"::RaycastVolumeExample::View3DGL1::backCol":{"gui_presentation_mode":8,"gui_read-only":false,"gui_visibility":true},"::RaycastVolumeExample::View3DGL1::cam::farplane":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":true},"::RaycastVolumeExample::View3DGL1::cam::halfaperturedegrees":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":true},"::RaycastVolumeExample::View3DGL1::cam::nearplane":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":true},"::RaycastVolumeExample::View3DGL1::cam::orientation":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":true},"::RaycastVolumeExample::View3DGL1::cam::position":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":true},"::RaycastVolumeExample::View3DGL1::cam::projectiontype":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":true},"::RaycastVolumeExample::View3DGL1::camstore::autoLoadSettings":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":true},"::RaycastVolumeExample::View3DGL1::camstore::autoSaveSettings":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":true},"::RaycastVolumeExample::View3DGL1::camstore::overrideSettings":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":true},"::RaycastVolumeExample::View3DGL1::camstore::restorecam":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":true},"::RaycastVolumeExample::View3DGL1::camstore::settings":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":true},"::RaycastVolumeExample::View3DGL1::camstore::storecam":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":true},"::RaycastVolumeExample::View3DGL1::resetViewOnBBoxChange":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":true},"::RaycastVolumeExample::View3DGL1::showLookAt":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":true},"::RaycastVolumeExample::View3DGL1::view::cubeOrientation":{"gui_presentation_mode":2,"gui_read-only":true,"gui_visibility":false},"::RaycastVolumeExample::View3DGL1::view::defaultOrientation":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":true},"::RaycastVolumeExample::View3DGL1::view::defaultView":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":true},"::RaycastVolumeExample::View3DGL1::view::resetView":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":true},"::RaycastVolumeExample::View3DGL1::view::showViewCube":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":true},"::RaycastVolumeExample::View3DGL1::viewKey::AngleStep":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":true},"::RaycastVolumeExample::View3DGL1::viewKey::FixToWorldUp":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":true},"::RaycastVolumeExample::View3DGL1::viewKey::MouseSensitivity":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":true},"::RaycastVolumeExample::View3DGL1::viewKey::MoveStep":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":true},"::RaycastVolumeExample::View3DGL1::viewKey::RotPoint":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":true},"::RaycastVolumeExample::View3DGL1::viewKey::RunFactor":{"gui_presentation_mode":2,"gui_read-only":false,"gui_visibility":true}},"WindowConfigurations":{"Configurator":{"win_callback":6,"win_collapsed":false,"win_flags":1032,"win_hotkey":[300,0],"win_position":[481.0,18.0],"win_reset_position":[0.0,0.0],"win_reset_size":[750.0,500.0],"win_show":true,"win_size":[1439.0,1062.0]},"Hotkey Editor":{"hotkey_list":[[{"key":293,"mods":4,"name":"_hotkey_gui_exit","parent":"","parent_type":2}],[{"key":83,"mods":2,"name":"_hotkey_gui_save_project","parent":"","parent_type":2}],[{"key":76,"mods":2,"name":"_hotkey_gui_load_project","parent":"","parent_type":2}],[{"key":301,"mods":0,"name":"_hotkey_gui_menu","parent":"","parent_type":2}],[{"key":292,"mods":0,"name":"_hotkey_gui_toggle_graph_entry","parent":"","parent_type":2}],[{"key":291,"mods":0,"name":"_hotkey_gui_trigger_screenshot","parent":"","parent_type":2}],[{"key":71,"mods":2,"name":"_hotkey_gui_show-hide","parent":"","parent_type":2}],[{"key":300,"mods":0,"name":"_hotkey_gui_window_Configurator","parent":"17030246060415721360","parent_type":3}],[{"key":77,"mods":3,"name":"_hotkey_gui_configurator_module_search","parent":"","parent_type":4}],[{"key":80,"mods":3,"name":"_hotkey_gui_configurator_param_search","parent":"","parent_type":4}],[{"key":261,"mods":0,"name":"_hotkey_gui_configurator_delete_graph_entry","parent":"","parent_type":4}],[{"key":83,"mods":3,"name":"_hotkey_gui_configurator_save_project","parent":"","parent_type":4}],[{"key":299,"mods":0,"name":"_hotkey_gui_window_Parameters","parent":"2021973547876601465","parent_type":3}],[{"key":80,"mods":2,"name":"_hotkey_gui_parameterlist_param_search","parent":"","parent_type":4}],[{"key":298,"mods":0,"name":"_hotkey_gui_window_Log Console","parent":"3923344468212134616","parent_type":3}],[{"key":297,"mods":0,"name":"_hotkey_gui_window_Transfer Function Editor","parent":"6166565350214160899","parent_type":3}],[{"key":296,"mods":0,"name":"_hotkey_gui_window_Performance Metrics","parent":"4920642770320252208","parent_type":3}],[{"key":295,"mods":0,"name":"_hotkey_gui_window_Hotkey Editor","parent":"529232018636216348","parent_type":3}],[{"key":67,"mods":5,"name":"::RaycastVolumeExample::View3DGL1_camstore::storecam","parent":"::::RaycastVolumeExample::View3DGL1::camstore::storecam","parent_type":1}],[{"key":67,"mods":4,"name":"::RaycastVolumeExample::View3DGL1_camstore::restorecam","parent":"::::RaycastVolumeExample::View3DGL1::camstore::restorecam","parent_type":1}],[{"key":268,"mods":0,"name":"::RaycastVolumeExample::View3DGL1_view::resetView","parent":"::::RaycastVolumeExample::View3DGL1::view::resetView","parent_type":1}],[{"key":32,"mods":0,"name":"::RaycastVolumeExample::View3DGL1_anim::togglePlay","parent":"::::RaycastVolumeExample::View3DGL1::anim::togglePlay","parent_type":1}],[{"key":77,"mods":0,"name":"::RaycastVolumeExample::View3DGL1_anim::SpeedUp","parent":"::::RaycastVolumeExample::View3DGL1::anim::SpeedUp","parent_type":1}],[{"key":78,"mods":0,"name":"::RaycastVolumeExample::View3DGL1_anim::SpeedDown","parent":"::::RaycastVolumeExample::View3DGL1::anim::SpeedDown","parent_type":1}],[{"key":83,"mods":4,"name":"::RaycastVolumeExample::ScreenShooter1_trigger","parent":"::::RaycastVolumeExample::ScreenShooter1::trigger","parent_type":1}]],"win_callback":4,"win_collapsed":false,"win_flags":0,"win_hotkey":[295,0],"win_position":[0.0,0.0],"win_reset_position":[0.0,0.0],"win_reset_size":[0.0,0.0],"win_show":false,"win_size":[0.0,0.0]},"Log Console":{"log_force_open":true,"log_level":100,"win_callback":7,"win_collapsed":false,"win_flags":3072,"win_hotkey":[298,0],"win_position":[0.0,0.0],"win_reset_position":[0.0,0.0],"win_reset_size":[500.0,50.0],"win_show":false,"win_size":[500.0,50.0]},"Parameters":{"param_extended_mode":false,"param_modules_list":[],"win_callback":1,"win_collapsed":false,"win_flags":8,"win_hotkey":[299,0],"win_position":[0.0,18.0],"win_reset_position":[0.0,0.0],"win_reset_size":[400.0,500.0],"win_show":true,"win_size":[479.0,1062.0]},"Performance Metrics":{"fpsms_max_value_count":20,"fpsms_mode":0,"fpsms_refresh_rate":2.0,"fpsms_show_options":false,"win_callback":3,"win_collapsed":false,"win_flags":2097217,"win_hotkey":[296,0],"win_position":[0.0,0.0],"win_reset_position":[0.0,0.0],"win_reset_size":[0.0,0.0],"win_show":false,"win_size":[0.0,0.0]},"Transfer Function Editor":{"tfe_active_param":"","tfe_view_minimized":false,"tfe_view_vertical":false,"win_callback":5,"win_collapsed":false,"win_flags":64,"win_hotkey":[297,0],"win_position":[0.0,0.0],"win_reset_position":[0.0,0.0],"win_reset_size":[0.0,0.0],"win_show":false,"win_size":[0.0,0.0]}}}]=])
mmRenderNextFrame()

-- Additional config
--mmSetGuiVisible(true)

-- Setup file for timestamps
timestamp_header = "Sensor Name,Sample Timestamp (ms),Momentary Power Comsumption (W)\n"
timestamp_file = timestamp_header

-- generate camera angles
camera_angles = splitString(mmGenerateCameraScenes("::RaycastVolumeExample::View3DGL1", "orbit", 5), "}")
for i = 1,CAMERA_ANGLE_COUNT do
	camera_angles[i] = string.sub(camera_angles[i], 2)
	camera_angles[i] = camera_angles[i] .. "}"
end

-- Loop over resolution parameter
for _, height in ipairs(RESOLUTIONS) do

	-- Set resolution to view framebuffer and window
	width = math.ceil(height * (16/9))
	mmSetViewFramebufferSize("::RaycastVolumeExample::View3DGL1", width, height)
	mmSetWindowFramebufferSize(width, height)
	mmRenderNextFrame() -- Always render a dummy frame to apply changes
	  
	-- View reset to center screen after resolution change
	mmSetParamValue("::RaycastVolumeExample::View3DGL1::view::resetView", [=[true]=])
	mmRenderNextFrame()
	
	-- Loop for different camera angles
	for i=1,CAMERA_ANGLE_COUNT do
		-- create a unique filename for the current parameter set
		test_case_name = GENERAL_TEST_CASE_NAME .. "_" .. height .. "p" .. "_" .. "cam" .. i
		
		-- set camera angle
		mmSetParamValue("::RaycastVolumeExample::View3DGL1::camstore::settings", camera_angles[i])
		mmSetParamValue(":::RaycastVolumeExample::View3DGL1::camstore::restorecam", [=[true]=])
		
		-- Optionally store a screenshot of the view (or window)
		--mmScreenshotEntryPoint( "::RaycastVolumeExample::View3DGL1", test_case_name .. ".png")
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

-- Generate and write angle file
angle_header = "Camera Angle;Camera Settings (JSON)\n"
angle_file = angle_header
for i = 1,CAMERA_ANGLE_COUNT do
	angle_file = angle_file .. "cam" .. i .. ";" .. camera_angles[i] .. "\n"
end
mmWriteTextFile(mmGetInstanceName() .. "-" .. GENERAL_TEST_CASE_NAME .. "_" .. "camera_angles" .. "_" .. ".csv", angle_file)

-- Quit
mmQuit()
