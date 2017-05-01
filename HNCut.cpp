// Primary header
#include "HNCut.h"

#include <algorithm>
#include <cassert>
#include <sstream>

// DPTK headers
#include "Algorithm.h"
#include "Geometry.h"
#include "Global.h"
#include "Image.h"

// Poco header needed for the macros below 
#include <Poco/ClassLibrary.h>

// Matlab header
//#include "matrix.h"
//#include "mclcppclass.h"
//#include "HNCutAlgo.h"

int system_hidden(const char *cmdArgs)
{
	PROCESS_INFORMATION pinfo;
	STARTUPINFO sinfo;

	/*
	* Allocate and hide console window
	*/
	AllocConsole ();
	ShowWindow (GetConsoleWindow(), 0);

	memset (&sinfo, 0, sizeof (sinfo));
	sinfo.cb = sizeof (sinfo);
	CreateProcessA (NULL, (char*)cmdArgs,
		NULL, NULL, false,
		0,
		NULL, NULL, &sinfo, &pinfo);
	DWORD ret;
	while (1)
	{
		HANDLE array[1];
		array[0] = pinfo.hProcess;
		ret = MsgWaitForMultipleObjects (1, array, false, INFINITE,
			QS_ALLPOSTMESSAGE);
		if ((ret == WAIT_FAILED) || (ret == WAIT_OBJECT_0))
			break;
		/*
		* Don't block message loop
		*/
		MSG msg;
		while (PeekMessage (&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage (&msg);
			DispatchMessage (&msg);
		}
	}

	DWORD pret;
	GetExitCodeProcess (pinfo.hProcess, &pret);
	//    FreeConsole ();
	return pret;
}


// Declare that this object has AlgorithmBase subclasses
//  and declare each of those sub-classes
POCO_BEGIN_MANIFEST(sedeen::algorithm::AlgorithmBase)
POCO_EXPORT_CLASS(sedeen::algorithm::HNCut)
POCO_END_MANIFEST

namespace sedeen {
namespace algorithm {

HNCut::HNCut()
    : _result(){
}

void HNCut::run() {
  auto factory = image()->getFactory();
  auto largest_region = factory->getLevelRegion(0);
  auto largest_size = size(largest_region);
  image::tile::Compositor compositor(factory);
  auto size = compositor.getDimensions(0);
  auto img = compositor.getImage(0, Rect(Point(0,0),size));

  // create png result in windows temp folder
  TCHAR buf [MAX_PATH];
  GetTempPathA (MAX_PATH, buf);

  std::string tempPath(buf);
  std::string inputPath = tempPath + "HNCut.png";
  std::string outputPath = tempPath + "HNCut_mask.png";
  img.save(inputPath);
  
  std::string command = "HNCut " + inputPath + " " + outputPath;
  system_hidden(command.c_str());

  // Update results
  auto openner = image::createImageOpener();
  namespace sf = sedeen::file;
  auto result_img = openner->open(sf::Location(outputPath));

  const void *raw_ptr = result_img.get();
  const char *raw = (const char *)raw_ptr;
  auto factory2 = result_img->getFactory();
  sedeen::image::tile::Compositor compositor2(factory2);
  auto temp = compositor2.getImage(largest_region, largest_size);
  _result.update(temp,largest_region);

}

void HNCut::init(const image::ImageHandle& image) {
  if (isNull(image)) return;

  // Bind system parameter for current view
  _display_area = createDisplayAreaParameter(*this);

  // Bind result
  _result = createImageResult(*this, "Threshold Result");
}

} // namespace algorithm
} // namespace sedeen
