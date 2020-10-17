#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    cout << "ofGetWidth: " << ofGetWidth() << endl;
    cout << "ofGetHeight: " << ofGetHeight() << endl;
    
    loadSettings();
    
    video.load("Sequence04.mp4");
    video.setLoopState(OF_LOOP_NORMAL);
    
    // listen on the given port
    cout << "listening for osc messages on port " << PORT << "\n";
    receiver.setup(PORT);
    
    portada.load("images/Krystallos001.jpeg");
    contador = 0;
    
    intro = false;
    videoState = false;
    
    fbo.allocate(1024,768, GL_RGBA);
    fbo.begin();
        ofClear(0,0,0);
    fbo.end();
    
}

//--------------------------------------------------------------
void ofApp::update(){
    
    if (!videoState) {
        drawIntro();
    } else {
        video.update();
        drawVideo();
    }
    
    // check for waiting messages
    while( receiver.hasWaitingMessages() ){
        // get the next message
        ofxOscMessage m;
        string mouseButtonState;
        receiver.getNextMessage(m);
        if( m.getAddress() == "/mouse/button" ){
            // the single argument is a string
            mouseButtonState = m.getArgAsString( 0 );
            if( mouseButtonState == "down" ) {
                cout << "osc message: down" << endl;
                if(!videoState){
                    videoState = true;
                    cout << "videoState: " << ofToString(videoState) << endl;
                    video.play();
                }
            } else if ( mouseButtonState == "up" ) {
                cout << "osc message: up" << endl;
                if(videoState){
                    videoState = false;
                    cout << "videoState: " << ofToString(videoState) << endl;
                    video.stop();
                }
            }
        }
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofSetColor(225);
    ofRotateZDeg(90);
    fbo.draw(0, 0, ofGetHeight(), -ofGetWidth());
}

//--------------------------------------------------------------
void ofApp::drawIntro(){
    contador ++;
    // cout << "Contador: " << ofToString(contador) << endl;
    if(contador>=525) {
        cout << "Intro ready" << endl;
        intro = true;
    } else if (contador>15) {
        fbo.begin();
            ofFill();
            ofSetColor(0,0,0,255);
            ofDrawRectangle(0,0,1024,768);
            ofSetColor(225);
        
            float height = 1024*portada.getHeight()/1920;
            float margin = (768 - height)/2;
            //cout << "height: " << ofToString(height) << endl;
            //cout << "margin: " << ofToString(margin) << endl;
        
            portada.draw(0,margin,1024,height);
            //verdana30.drawString("KRISTALLOS", 30, 92);
    
            ofNoFill();
            int num = 0;
            for(int i=0; i<=contador; i+=25) {
                ofDrawRectangle(num*16 + 328, 40, 10, 30);
                num++;
            }
        fbo.end();
    }
}

//--------------------------------------------------------------
void ofApp::drawVideo(){
    if(video.isLoaded()){
        fbo.begin();
            ofFill();
            ofSetColor(0,0,0,255);
            ofDrawRectangle(0,0,1024,768);
            ofSetColor(225);
            video.getTexturePtr()->draw(0,0,1024,768);
        fbo.end();
    }
}

//--------------------------------------------------------------
void ofApp::exit(){

}

//--------------------------------------------------------------
void ofApp::touchDown(ofTouchEventArgs & touch){
    
}

//--------------------------------------------------------------
void ofApp::touchMoved(ofTouchEventArgs & touch){

}

//--------------------------------------------------------------
void ofApp::touchUp(ofTouchEventArgs & touch){

}

//--------------------------------------------------------------
void ofApp::touchDoubleTap(ofTouchEventArgs & touch){
    if((video.isLoaded())&&(intro)) {
        if(videoState) {
            videoState = false;
            cout << "videoState: " << ofToString(videoState) << endl;
            video.stop();
        } else {
            videoState = true;
            cout << "videoState: " << ofToString(videoState) << endl;
            video.play();
        }
    }
}

//--------------------------------------------------------------
void ofApp::touchCancelled(ofTouchEventArgs & touch){
    
}

//--------------------------------------------------------------
void ofApp::lostFocus(){

}

//--------------------------------------------------------------
void ofApp::gotFocus(){

}

//--------------------------------------------------------------
void ofApp::gotMemoryWarning(){

}

//--------------------------------------------------------------
void ofApp::deviceOrientationChanged(int newOrientation){

}


//--------------------------------------------------------------
void ofApp::loadSettings(){
    string message;
    string value;
    
    //-----------
    //the string is printed at the top of the app
    //to give the user some feedback
    message = "loading mySettings.xml";

    //we load our settings file
    //initially we do this from the data/ folder
    //but when we save we are saving to the documents folder which is the only place we have write access to
    //thats why we check if the file exists in the documents folder.
    
    if( XML.loadFile(ofxiOSGetDocumentsDirectory() + "mySettings.xml") ){
        message = "mySettings.xml loaded from documents folder!";
    }else if( XML.loadFile("mySettings.xml") ){
        message = "mySettings.xml loaded from data folder!";
    }else{
        message = "unable to load mySettings.xml check data/ folder";
    }
    cout << message << endl;
    
}
