#pragma once

#include "ofMain.h"
#include "ofxAssimpModelLoader.h"
#include "ofxGui.h"
#include "ofxCameraSaveLoad.h"
#include "ofxOscParameterSync.h"
#include "VHPcontrast.h"
#include "ofxXmlSettings.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		
        void setupParameters();
        void setupSettings();
    
        void setupAudio();
        void updateAudio();
        void drawAudioFFT();
    
        void setup3D();
        void update3D();
        void draw3D();
    
        void setupCelda();
        void updateCelda();
    
        void setupLight();
        void updateLight();
    
        void setupMaterials();
        void updateMaterials();
    
        void addFace(ofMesh& mesh, ofVec3f a, ofVec3f b, ofVec3f c);
        void addFace(ofMesh& mesh, ofVec3f a, ofVec3f b, ofVec3f c, ofVec3f d);
        
        void setupAtoms();
        void atomNoise();
        void atomMovement();
    
        void setupModels();
        void updateModels();
        void modelNoise();
    
        void setupVideo();
        void updateVideo();
    
        // Settings
        ofxXmlSettings XML;
    
        // Krystallos
        ofMesh celda;
        vector <ofVec3f> Oatom;
        vector <ofVec3f> Catom;
        vector <ofVec3f> Hatom;
        vector <ofVec3f> CUatom;
    
        // 3D
        ofEasyCam       cam;
    
        ofLight         light;
        ofLight         directionalLight;
        ofLight         ambientLight;
        
        ofVec3f         renderPosition;
    
        ofMaterial      material_01;
        ofMaterial      material_02;
        ofMaterial      material_03;
    
        // Models
        ofxAssimpModelLoader model_01;
        ofxAssimpModelLoader model_02;
        ofxAssimpModelLoader model_03;
        ofxAssimpModelLoader model_04;
        ofxAssimpModelLoader model_05;
    
        float rotationX;
        float rotationY;
        float rotationZ;
    
        // FFT
        ofSoundPlayer           krystallos;
        string                  audioFile;
        int                     duration;
        int *                   sections;
        static constexpr size_t nBandsToGet = 64;
        std::array<float, nBandsToGet> fftSmoothed{{0}};
    
        float   media;
        float   offset;
        float   maximo;
        float   maximo_alpha;
        float   alpha;
        bool    play;
    
        // Image
        ofImage img;
        string  imgFile;
        ofImage marcas;
    
        // Video
        ofFbo           fbo;
        ofFbo           buffer;
        ofFbo           tempBuffer;
        ofTexture       tempTex;
        ofTexture       bufferTex;
        ofPixels        tempPix;
    
        // contrast C
        ofParameter<ofColor>    EC;
        ofParameter<ofColor>    FC;
        ofShader                shaderContrast;
        VHPcontrast             contrast;
    
        ofShader        shaderSaturation;
        ofShader        shaderMixture;
    
        ofxOscParameterSync sync;
        ofxPanel gui;
        ofParameterGroup parameters;
        ofParameter<float> renderX;
        ofParameter<float> renderY;
        ofParameter<float> renderZ;
    
        ofParameter<ofColor> ambient;
        ofParameter<ofColor> diffuse;
        ofParameter<ofColor> specular;
    
        ofParameter<float>  lightAngle;
        ofParameter<float>  lightConentration;
        ofParameter<float> lightX;
        ofParameter<float> lightY;
        ofParameter<float> lightZ;
    
        ofParameter<float>  mixture;
        ofParameter<int>    thresholdS;
        ofParameter<int>    stela;
    
        ofParameter<ofColor> ambient2;
        ofParameter<ofColor> diffuse2;
        ofParameter<ofColor> specular2;
    
        bool showGui;
        bool effects;
        int mode;
};
