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

#include <windows.h>

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

  // create tif image in windows temp folder
  TCHAR buf [MAX_PATH];
  GetTempPathA (MAX_PATH, buf);

  std::string tempPath(buf);
  std::string inputPath = tempPath + "HNCut.png";
  std::string outputPath = tempPath + "HNCut_mask.png";
  img.save(inputPath);
  
  PROCESS_INFORMATION ProcessInfo;

  STARTUPINFO StartupInfo;
  std::string cmd = "HNCut.exe " + inputPath + " " + outputPath;
  char* cmdArgs = (char*)cmd.c_str();

  ZeroMemory(&StartupInfo, sizeof(StartupInfo));
  StartupInfo.cb = sizeof StartupInfo;

  if (CreateProcessA(NULL, cmdArgs, NULL, NULL, FALSE, 0, NULL, NULL, &StartupInfo, &ProcessInfo))
  {
	  WaitForSingleObject(ProcessInfo.hProcess, INFINITE);
	  CloseHandle(ProcessInfo.hThread);
	  CloseHandle(ProcessInfo.hProcess);
  }

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
