#ifndef POSEPUBLISHER_H_N39C3O1T
#define POSEPUBLISHER_H_N39C3O1T

#include <umundo/core.h>
#include <umundo/s11n.h>
#include <aruco/aruco.h>
#include <aruco/cvdrawingutils.h>

namespace umundo {

using namespace cv;
using namespace aruco;

class ArucoPosePublisher : public Thread {
public:
	ArucoPosePublisher(const string&, const string&);
	virtual ~ArucoPosePublisher();
	
	void run();

protected:
	TypedPublisher* _typedPub;
	Node* _node;
	
  Mat _inputImage;
	VideoCapture _videoCapture;
	MarkerDetector _markerDetector;
	CameraParameters _camParameters;
	float _markerSize;
};

}

#endif /* end of include guard: POSEPUBLISHER_H_N39C3O1T */
