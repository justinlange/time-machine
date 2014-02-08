#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
    
    // enable depth->video image calibration
	kinect.setRegistration(true);
    
	kinect.init();
	//kinect.init(true); // shows infrared instead of RGB video image
	//kinect.init(false, false); // disable video image (faster fps)
	
	kinect.open();		// opens first available kinect
	//kinect.open(1);	// open a kinect by id, starting with 0 (sorted by serial # lexicographically))
	//kinect.open("A00362A08602047A");	// open a kinect using it's unique serial #
	
	// print the intrinsic IR sensor values
	if(kinect.isConnected()) {
		ofLogNotice() << "sensor-emitter dist: " << kinect.getSensorEmitterDistance() << "cm";
		ofLogNotice() << "sensor-camera dist:  " << kinect.getSensorCameraDistance() << "cm";
		ofLogNotice() << "zero plane pixel size: " << kinect.getZeroPlanePixelSize() << "mm";
		ofLogNotice() << "zero plane dist: " << kinect.getZeroPlaneDistance() << "mm";
	}
    
    nearThreshold = 230;
	farThreshold = 70;
	bThreshWithOpenCV = true;
	
	ofSetFrameRate(60);
	
	// zero the tilt on startup
	angle = 0;
	kinect.setCameraTiltAngle(angle);
	
	// start from the front
	bDrawPointCloud = false;

}

//--------------------------------------------------------------
void testApp::update(){
    
    ofBackground(100, 100, 100);
	
	kinect.update();

	
#ifdef USE_TWO_KINECTS
	kinect2.update();
#endif


}

//--------------------------------------------------------------
void testApp::draw(){
    
    ofSetColor(255, 255, 255);
	
		easyCam.begin();
		drawPointCloud();
		easyCam.end();

}

void testApp::drawPointCloud() {
	int w = 640;
	int h = 480;
	ofMesh mesh;
	mesh.setMode(OF_PRIMITIVE_POINTS);
	int step = 2;
	for(int y = 0; y < h; y += step) {
		for(int x = 0; x < w; x += step) {
			if(kinect.getDistanceAt(x, y) > 0) {
				mesh.addColor(kinect.getColorAt(x,y));
				mesh.addVertex(kinect.getWorldCoordinateAt(x, y));
			}
		}
	}
	glPointSize(3);
	ofPushMatrix();
	// the projected points are 'upside down' and 'backwards'
	ofScale(1, -1, -1);
	ofTranslate(0, 0, -1000); // center the points a bit
	ofEnableDepthTest();
	mesh.drawVertices();
	ofDisableDepthTest();
	ofPopMatrix();
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}

void testApp::exit() {
	kinect.setCameraTiltAngle(0); // zero the tilt on exit
	kinect.close();
	
#ifdef USE_TWO_KINECTS
	kinect2.close();
#endif
}
