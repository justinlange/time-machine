#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
    
    loadKinect = false;
    
    ofSetLogLevel( OF_LOG_VERBOSE );
	
	ofSetFrameRate(999);
	ofSetVerticalSync( false );
	
	//ofSetFrameRate( 60 );
	//ofSetVerticalSync( true );
    
	fontWorld.loadFont( "Fonts/DIN.otf", 18, true, false, true );
	//fontWorld.loadFont( "Fonts/DIN.otf", 18 );
    
	oculusRift.init( 1280, 800, 4 );
	oculusRift.setPosition( 0,-30,0 );
	
	lastUpdateTime = ofGetElapsedTimef();
    
        if(loadKinect){
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
            
        // zero the tilt on startup
        angle = 0;
        kinect.setCameraTiltAngle(angle);
        
        // start from the front
        bDrawPointCloud = false;
        mesh.setMode(OF_PRIMITIVE_POINTS);
        }

}

//--------------------------------------------------------------
void testApp::update(){
    
    ofBackground(100, 100, 100);

	if(loadKinect)	kinect.update();

	
#ifdef USE_TWO_KINECTS
	kinect2.update();
#endif
    
//--------oculus-----------------------------------------------
    
	float currTime = ofGetElapsedTimef();
	float frameDeltaTime = currTime - lastUpdateTime;
	lastUpdateTime = currTime;
	
	if(		 ofGetKeyPressed('i') ) { oculusRift.setInterOcularDistance( oculusRift.getInterOcularDistance() + 0.001f ); }
	else if( ofGetKeyPressed('o') ) { oculusRift.setInterOcularDistance( oculusRift.getInterOcularDistance() - 0.001f ); }
	else if( ofGetKeyPressed('k') ) { oculusRift.setInterOcularDistance( oculusRift.getInterOcularDistance() + 1.0f ); }
	else if( ofGetKeyPressed('l') ) { oculusRift.setInterOcularDistance( oculusRift.getInterOcularDistance() - 1.0f ); }
	
	if(	ofGetKeyPressed(OF_KEY_UP) )    { oculusRift.dolly(  30.0f * frameDeltaTime ); }
	if( ofGetKeyPressed(OF_KEY_DOWN) )  { oculusRift.dolly( -30.0f * frameDeltaTime ); }
	if( ofGetKeyPressed(OF_KEY_LEFT) )  { oculusRift.truck(  30.0f * frameDeltaTime ); }
	if( ofGetKeyPressed(OF_KEY_RIGHT) ) { oculusRift.truck( -30.0f * frameDeltaTime ); }
}

//--------------------------------------------------------------
void testApp::draw()
{
	oculusRift.beginRenderSceneLeftEye();
    drawSceneGeometry();
	oculusRift.endRenderSceneLeftEye();
	
	oculusRift.beginRenderSceneRightEye();
    drawSceneGeometry();
	oculusRift.endRenderSceneRightEye();
	
	ofSetColor( 255 );
	oculusRift.draw( ofVec2f(0,0), ofVec2f( ofGetWidth(), ofGetHeight() ) );
}

//--------------------------------------------------------------

void testApp::drawPointCloud() {
    mesh.clear();
	int w = 640;
	int h = 480;
//	ofMesh mesh;
//	mesh.setMode(OF_PRIMITIVE_POINTS);
	int step = 1;
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

///--------------------------------------------------------------
void testApp::drawSceneGeometry()
{
	ofSetColor(120);
	
	ofPushMatrix();
    ofRotate(90, 0, 0, -1);
    ofDrawGridPlane(500.0f, 40.0f, false );
	ofPopMatrix();
	
	ofSetColor( 255, 0, 0 );
	
	ofPushMatrix();
    ofTranslate( ofPoint(10,0,-80) );
    for( int i = 0; i < 20; i ++ )
    {
        ofBox( ofPoint(0,25,i * -100), 50);
    }
	ofPopMatrix();
	
	string tmpStr = "Do Warping: " + ofToString( oculusRift.getDoWarping() ) + "\n";
	tmpStr += "Inter Ocular Distance: "  + ofToString( oculusRift.getInterOcularDistance() ) + "\n";
	tmpStr += "Shader Scale Factor: "  + ofToString( oculusRift.getShaderScaleFactor() ) + "\n";
	
	ofSetColor( 255 );
	
	ofPushMatrix();
    ofTranslate( ofPoint(-60,280,-200) );
    ofRotateZ( 180 );
    ofRotateY( 180 );
    fontWorld.drawString( tmpStr, 0.0f, 0.0f );
	ofPopMatrix();
	
	ofSetColor(255);
}

//--------------------------------------------------------------

//--------------------------------------------------------------
void testApp::keyPressed(int key)
{
	if( key == 'f' )
	{
		ofToggleFullscreen();
	}
	if( key == 'w' )
	{
		oculusRift.setDoWarping( !oculusRift.getDoWarping() );
	}
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
    if(loadKinect){
	kinect.setCameraTiltAngle(0); // zero the tilt on exit
	kinect.close();
    }
	
#ifdef USE_TWO_KINECTS
	kinect2.close();
#endif
}
