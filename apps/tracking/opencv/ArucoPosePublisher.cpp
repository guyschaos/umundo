#include "ArucoPosePublisher.h"
#include "protobuf/generated/Pose.pb.h"

namespace umundo {

using namespace cv;
using namespace aruco;

ArucoPosePublisher::ArucoPosePublisher(const string& domain, const string& calibrationFile) {
	if (calibrationFile == "") {
		// default to iSight parameters, actual parameters are generated via opencv and a printed checkboard pattern:
		// $ bin/calibration -w 8 -h 5 -o camera.yml
		int w = 1280, h = 720;
		cv::Mat camera = cv::Mat::eye(3,3,CV_32FC1);
    cv::Mat dist = cv::Mat::zeros(5,1,CV_32FC1);

		camera.at<float>(0,0) = 9.8519920533248728e+02;
		camera.at<float>(0,2) = 6.3855446891811812e+02;
		camera.at<float>(1,1) = 9.8890679500708472e+02;
		camera.at<float>(1,2) = 3.6109373151816021e+02;
		dist.at<float>(0,0) = -1.8543589470473218e-01;
		dist.at<float>(1,0) = 5.9627971053004869e-01;
		dist.at<float>(2,0) = 3.3091593359972763e-03;
		dist.at<float>(3,0) = -1.3846777725374246e-03;
		dist.at<float>(4,0) = -7.7041303098132452e-01;
		
		_camParameters.setParams(camera, dist, cv::Size(w, h));
	} else {
		_camParameters.readFromXMLFile(calibrationFile);
	}

	_videoCapture.open(0);
  _videoCapture >> _inputImage;
  _camParameters.resize(_inputImage.size());

	_markerSize = 1024;

	_node = new Node(domain);
	_typedPub = new TypedPublisher("pose");
  _typedPub->registerType("Pose", new Pose());
	_node->addPublisher(_typedPub);
}

ArucoPosePublisher::~ArucoPosePublisher() {
	
}

void ArucoPosePublisher::run() {
		vector<Marker> markers;
	double thresParam1, thresParam2;
 _markerDetector.getThresholdParams(thresParam1, thresParam2);

	while(isStarted() && _videoCapture.grab()) {
    
		_videoCapture.retrieve(_inputImage);
		_markerDetector.detect(_inputImage, markers, _camParameters, _markerSize);

		for (unsigned int k = 0; k < markers.size(); k++) {
      
      // get rotation matrix from compact rotation vector
      cv::Mat Rot(3,3,CV_32FC1);
      cv::Rodrigues(markers[k].Rvec, Rot);

      // get axes to make code cleaner
      double axes[3][3];
      // x axis
      axes[0][0] = -Rot.at<float>(0,0);
      axes[0][1] = -Rot.at<float>(1,0);
      axes[0][2] = +Rot.at<float>(2,0);
      // y axis
      axes[1][0] = -Rot.at<float>(0,1);
      axes[1][1] = -Rot.at<float>(1,1);
      axes[1][2] = +Rot.at<float>(2,1);    
      // for z axis, we use cross product
      axes[2][0] = axes[0][1]*axes[1][2] - axes[0][2]*axes[1][1];
      axes[2][1] = - axes[0][0]*axes[1][2] + axes[0][2]*axes[1][0];
      axes[2][2] = axes[0][0]*axes[1][1] - axes[0][1]*axes[1][0];
      
      /*
       *  |
       *  | y
       *  |     x
       *  /-------
       * / z
       *
       * The values are calculated as if we were looking on an airplane from above.
       * Pitch and roll can only be between -PI/2,PI/2 as we would not see the pattern otherwise.
       * The formulaes below are dot product of the various axis with the normal vector
       * of the various geometric planes. As each vector's length is 1, we can just use acos.
       */
      
      double pitch = acos(-1 * axes[2][2]) - M_PI_2; // z to xy plane
      double roll = acos(-1 * axes[0][2]) - M_PI_2; // x to yx plane
      double yaw1 = acos(axes[0][0]); // x to yz plane
      double yaw2 = acos(axes[2][0]); // z to yz plane
      
      /* Clockwise rotation of pattern
       *
       * yaw1 | yaw2 => desired
       * PI   | PI/2 => 0
       * PI/2 | 0    => 1 PI/2
       * 0    | PI/2 => 2 PI/2
       * PI/2 | PI   => 3 PI/2
       */
      
      double yaw = -1;
      if (yaw1 < M_PI && yaw2 < M_PI_2)
        yaw = M_PI - yaw1;
      else {
        yaw = M_PI + yaw1;
      }
      
      std::cout << std::setw(5);
      std::cout << "Pitch: " << pitch << " ";
      std::cout << "Roll:  " << roll << " ";
      std::cout << "Yaw:   " << yaw << " [" << yaw1 << "/" << yaw2 << "]" << std::endl;
            
			Pose* pose = new Pose();
      pose->mutable_orientation()->set_pitch(pitch);
      pose->mutable_orientation()->set_roll(roll);
      pose->mutable_orientation()->set_yaw(yaw1);
			
      // TODO: This is most likely false, but we ignore the position for now anyway
      pose->mutable_position()->set_iswgs84(false);
      pose->mutable_position()->set_latitude(markers[k].Tvec.at<double>(0));
      pose->mutable_position()->set_longitude(markers[k].Tvec.at<double>(1));
      pose->mutable_position()->set_height(markers[k].Tvec.at<double>(2));
      
      _typedPub->sendObj("Pose", pose);
		}
	}
}

}