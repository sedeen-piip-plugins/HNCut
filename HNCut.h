#ifndef SEDEEN_SRC_PLUGINS_HNCUT_HNCUT_H
#define SEDEEN_SRC_PLUGINS_HNCUT_HNCUT_H

// DPTK headers - a minimal set 
#include "algorithm/AlgorithmBase.h"
#include "algorithm/Parameters.h"
#include "algorithm/Results.h"

namespace sedeen {
namespace algorithm {

/// HNCut

class HNCut : public algorithm::AlgorithmBase {
public:
	HNCut();

private:
	virtual void run();
	virtual void init(const image::ImageHandle& image);

	void setOutput(std::string output_path);

	DisplayAreaParameter _display_area;
	ImageResult _result;

};

} // namespace algorithm
} // namespace sedeen

#endif

