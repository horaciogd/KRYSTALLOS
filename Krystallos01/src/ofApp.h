#pragma once

#include "ofxiOS.h"
#include "ofxOsc.h"
#include "ofxXmlSettings.h"

#define PORT 12345

class ofApp : public ofxiOSApp {
    
    public:
        void setup();
        void update();
        void draw();
        void drawIntro();
        void drawVideo();
        void exit();
    
        void touchDown(ofTouchEventArgs & touch);
        void touchMoved(ofTouchEventArgs & touch);
        void touchUp(ofTouchEventArgs & touch);
        void touchDoubleTap(ofTouchEventArgs & touch);
        void touchCancelled(ofTouchEventArgs & touch);

        void lostFocus();
        void gotFocus();
        void gotMemoryWarning();
        void deviceOrientationChanged(int newOrientation);
    
    
        void loadSettings();
    
        ofxiOSVideoPlayer video;
    
        ofxOscReceiver receiver;
        ofxOscSender senderA;
        ofxOscSender senderB;
        ofxOscSender senderC;
    
        ofxXmlSettings XML;

        // Images
        ofImage portada;
    
        // Control
        int contador;
        bool intro;
        bool videoState;
        
        // video
        ofFbo       fbo;
    
};
