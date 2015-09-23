// Template for DeepPixelOps

#include "DDImage/Row.h"
#include "DDImage/Knobs.h"
#include "DDImage/DDMath.h"
#include "DDImage/DeepPixelOp.h"
#include "DDImage/Pixel.h"
#include "DDImage/RGB.h"

#include <stdio.h>

using namespace DD::Image;

class {{cookiecutter.plugin_name}} : public DeepPixelOp
{

	int id;
	Channel id_channel;
	ChannelSet mod_channels;

	float multiplier;

public:

  {{cookiecutter.plugin_name}}(Node* node) : DeepPixelOp(node)
  {
	  id_channel = findChannel("other.VRayMtlID");
	  mod_channels = Mask_RGB;
	  id = 1;
	  multiplier = 1.0f;
  }


  // Wrapper function to work around the "non-virtual thunk" issue on linux when symbol hiding is enabled.
  virtual bool doDeepEngine(DD::Image::Box box, const ChannelSet &channels, DeepOutputPlane &plane)
  {
    return DeepPixelOp::doDeepEngine(box, channels, plane);
  }


  // Wrapper function to work around the "non-virtual thunk" issue on linux when symbol hiding is enabled.
  virtual void getDeepRequests(DD::Image::Box box, const DD::Image::ChannelSet& channels, int count, std::vector<RequestData>& requests)
  {
    DeepPixelOp::getDeepRequests(box, channels, count, requests);
  }


  // We always need the rgb no matter what channels are asked for:
  void in_channels(int, ChannelSet& channels) const
  {
	channels += mod_channels;
	channels += id_channel;
	channels += Chan_Alpha;
  }

  // for menu listing and function controls
  virtual void knobs(Knob_Callback);

  static const Iop::Description d;
  const char* Class() const { return d.name; }

  virtual Op* op()
  {
    return this;
  }

  void _validate(bool);
  virtual void processSample(int y,
                             int x,
                             const DD::Image::DeepPixel& deepPixel,
                             size_t sampleNo,
                             const DD::Image::ChannelSet& channels,
                             DeepOutPixel& output) const;

  const char* node_help() const;
};


void {{cookiecutter.plugin_name}}::_validate(bool for_real)
{
  DeepPixelOp::_validate(for_real);
}

void {{cookiecutter.plugin_name}}::processSample(int y,
                                int x,
                                const DD::Image::DeepPixel& deepPixel,
                                size_t sampleNo,
                                const DD::Image::ChannelSet& channels,
                                DeepOutPixel& output) const
{
    foreach(z, channels) {		
		float in_id = deepPixel.getUnorderedSample(sampleNo, id_channel);
		float in_alpha = deepPixel.getUnorderedSample(sampleNo, Chan_Alpha);
		float in = deepPixel.getUnorderedSample(sampleNo, z);
		
		if (mod_channels.contains(z))  {
			in = in / in_alpha;
			if (in_id == id || !id_channel) {
				output.push_back(in * multiplier * in_alpha);
			} else {
				output.push_back(in * in_alpha);
			}
			
		} else {
			output.push_back(in);
		}
	}
    
}

void {{cookiecutter.plugin_name}}::knobs(Knob_Callback f)
{
  
	Input_ChannelSet_knob(f, &mod_channels, 0, "Channels");
	Input_Channel_knob(f, &id_channel, 1, 0, "ID Channel");

	//Int_knob(f, &matte_id, "Material ID Red");
	Int_knob(f, &id, "ID");

	Float_knob(f, &multiplier, IRange(0, 1), "Multiplier");

}

const char* {{cookiecutter.plugin_name}}::node_help() const
{
  return
    "Basic sample.\n"
    "Multiline!";
}

static Op* build(Node* node) { return new {{cookiecutter.plugin_name}}(node); }
const Op::Description {{cookiecutter.plugin_name}}::d("{{cookiecutter.plugin_name}}", 0, build);
