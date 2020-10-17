#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    /*
    ofEnableDepthTest();
    ofEnableAntiAliasing();
    ofEnableAlphaBlending();
    */
    
    ofSetVerticalSync(true);
    
    showGui = false;
    effects = true;
    play = false;
    
    setupParameters();
    setupSettings();
    setupAudio();
    setup3D();
    setupCelda();
    setupAtoms();
    setupVideo();
    
    img.load(imgFile);
    marcas.load("marcas-referencia.png");
}

//--------------------------------------------------------------
void ofApp::update(){
    sync.update();
    if (play) {
        updateAudio();
        updateModels();
        updateMaterials();
        updateLight();
        
        fbo.begin();
            ofFill();
            ofSetColor(0,0,0,stela.get());
            ofDrawRectangle(0,0,2048,1536);
            draw3D();
        fbo.end();
        
        buffer.begin();
            
            //shaderSaturation.begin();
            //shaderSaturation.setUniform1f("threshold", thresholdS.get()/255.0);
        
            shaderContrast.begin();
                shaderContrast.setUniformTexture("texEr", contrast.texture[(int)EC.get().r], 1);
                shaderContrast.setUniformTexture("texEg", contrast.texture[(int)EC.get().g], 2);
                shaderContrast.setUniformTexture("texEb", contrast.texture[(int)EC.get().b], 3);
                shaderContrast.setUniform1f("fr", FC.get().r);
                shaderContrast.setUniform1f("fg", FC.get().g);
                shaderContrast.setUniform1f("fb", FC.get().b);
        
                shaderContrast.setUniform1f("mixture", mixture.get()/100.0);
        
                fbo.draw(0, 0, 2048, 1536);
        
            shaderContrast.end();
            //shaderSaturation.end();
         
        buffer.end();
        
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    if (play) {
        ofBackground(0);
        float fboAlpha = 255.0;
        if (effects) fboAlpha = alpha;
        if(alpha>=maximo_alpha){
            maximo_alpha = alpha;
            //cout << "maximo_alpha: " << ofToString(maximo_alpha) << endl;
        }
        //fboAlpha = ofClamp(fboAlpha+60.0, 0.0, 255.0);
        fboAlpha = 255.0;
        ofSetColor(255, fboAlpha);
        
        switch (mode) {
            case 0:
                buffer.draw(0, 0, 1024, 768);
                break;
            case 1:
                tempBuffer.draw(0, 0, 1024, 768);
                break;
            case 2:
                fbo.draw(0, 0, 1024, 768);
                break;
        }
        
        //ofSetColor(255, 255.0-fboAlpha);
        //fbo.draw(0, 0, 1024, 768);
        
        ofSetColor(255);
        img.draw(0, 0, 1024, 768);
        if(showGui) {
            marcas.draw(0, 0, 1024, 768);
            gui.draw();
            drawAudioFFT();
        }
        if(!krystallos.isPlaying()) play = false;
    } else {
        ofBackground(255,0,0);
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    switch(key) {
        case ' ':
            play = true;
            krystallos.play();
            break;
        case 'c':
            cam.getMouseInputEnabled() ? cam.disableMouseInput() : cam.enableMouseInput();
            break;
        case OF_KEY_UP:
            offset += 0.01;
            cout << "offset: " << ofToString(offset) << endl;
            break;
        case OF_KEY_DOWN:
            offset -= 0.01;
            cout << "offset: " << ofToString(offset) << endl;
            break;
        case 'n':
            showGui = !showGui;
            cout << "showGui: " << ofToString(showGui) << endl;
            break;
        case 's':
            gui.saveToFile("settings.xml");
            ofxSaveCamera(cam, "ofEasyCamSettings");
            break;
        case 'l':
            gui.loadFromFile("settings.xml");
            ofxLoadCamera(cam, "ofEasyCamSettings");
            renderPosition.set(renderX,renderY,renderZ);
            break;
        case 'e':
            effects = !effects;
            break;
        case 'f':
            ofToggleFullscreen();
            break;
        case 'q':
            renderZ += 0.5;
            renderPosition.set(renderPosition.x,renderPosition.y,renderZ);
            break;
        case 'a':
            renderZ -= 0.5;
            renderPosition.set(renderPosition.x,renderPosition.y,renderZ);
            break;
        case '1':
            mode = 0;
            break;
        case '2':
            mode = 1;
            break;
        case '3':
            mode = 2;
            break;
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    if(!cam.getMouseInputEnabled()){
        renderPosition.set((x*300.0/ofGetWidth())-150.0, (y*300.0/ofGetHeight())-150.0, renderPosition.z);
    }
    
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    
    cout << "getOrientationQuat: " << ofToString(cam.getGlobalOrientation()) << endl;
    cout << "getDistance: " << ofToString(cam.getDistance()) << endl;
    cout << "getPosition: " << ofToString(cam.getPosition()) << endl;
    
    renderX = (float)renderPosition.x;
    renderY = (float)renderPosition.y;
    renderZ = (float)renderPosition.z;
     
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}

//--------------------------------------------------------------
void ofApp::setupParameters(){
    
    parameters.setName("parameters");
    parameters.add(renderX.set( "renderX", 0, -1000.0, 1000.0 ));
    parameters.add(renderY.set( "renderY", 0, -1000.0, 1000.0 ));
    parameters.add(renderZ.set( "renderZ", 0, -1000.0, 1000.0 ));
    parameters.add(ambient.set("ambient",ofColor(127),ofColor(0,0),ofColor(255)));
    parameters.add(diffuse.set("diffuse",ofColor(127),ofColor(0,0),ofColor(255)));
    parameters.add(specular.set("specular",ofColor(127),ofColor(0,0),ofColor(255)));
    
    parameters.add(thresholdS.set("thresholdS", 255, 0, 255));
    parameters.add(mixture.set( "mixture", 50.0, 0.0, 100.0));
    parameters.add(stela.set( "stela", 50.0, 0.0, 255.0));
    
    parameters.add(lightAngle.set( "lightAngle", 90.0, 0.0, 90.0 ));
    parameters.add(lightConentration.set( "lightConentration", 5.0, 0.0, 128.0));
    parameters.add(lightX.set( "lightX", 0, -20.0, 20.0 ));
    parameters.add(lightY.set( "lightY", 0, -20.0, 20.0 ));
    parameters.add(lightZ.set( "lightZ", 30, -100.0, 100.0 ));
    
    parameters.add(ambient2.set("ambient2",ofColor(127),ofColor(0,0),ofColor(255)));
    parameters.add(diffuse2.set("diffuse2",ofColor(127),ofColor(0,0),ofColor(255)));
    parameters.add(specular2.set("specular2",ofColor(127),ofColor(0,0),ofColor(255)));
    
    // contrast C
    parameters.add(EC.set("EC",ofColor(127),ofColor(0,0),ofColor(255)));
    parameters.add(FC.set("FC",ofColor(127),ofColor(0,0),ofColor(255)));
    contrast.setup();
    
    gui.setup(parameters);
    sync.setup((ofParameterGroup&)gui.getParameter(),6666,"localhost",6667);
    
}

//--------------------------------------------------------------
void ofApp::setupSettings(){
    
    if ( XML.loadFile("krystallos.xml") ){
        cout << "mySettings.xml loaded from data folder!" << endl;
    } else {
        cout << "unable to load mySettings.xml check data/ folder" << endl;
    }
    
    string module = XML.getValue("settings:module", "krystallos_00");
    string part = XML.getValue("settings:part", "inicio");
    
    audioFile = XML.getValue("settings:audio:"+ module +":"+ part +":file", "audio.wav");
    cout << "Audio file: " << audioFile << endl;
    
    sections = new int[5];
    sections[0] = XML.getValue("settings:audio:"+ module +":"+ part +":structure:section_0", 0);
    sections[1] = XML.getValue("settings:audio:"+ module +":"+ part +":structure:section_1", 0);
    sections[2] = XML.getValue("settings:audio:"+ module +":"+ part +":structure:section_2", 0);
    sections[3] = XML.getValue("settings:audio:"+ module +":"+ part +":structure:section_3", 0);
    sections[4] = XML.getValue("settings:audio:"+ module +":"+ part +":structure:section_4", 0);
    cout << "Sections: " << ofToString(sections[0]) << " " << ofToString(sections[1]) << " " << ofToString(sections[2]) << " " << ofToString(sections[3]) << " " << ofToString(sections[4]) << endl;
    
    imgFile = XML.getValue("settings:mask", "img.png");
    cout << "Image file: " << imgFile << endl;
    
    duration = XML.getValue("settings:duration", 0);
    cout << "Duration: " << ofToString(duration) << endl;
    
}

//--------------------------------------------------------------
void ofApp::setupAudio(){
    // load in sounds:
    krystallos.load("Cuc10_cheda_final_stereo2.wav");
    cout << "loading Cuc10_cheda_final_stereo2.wav" <<endl;
    krystallos.setVolume(1.0);
    krystallos.setLoop(false);
    //krystallos.play();
    maximo = 0.00001;
    maximo_alpha = 0.00001;
    media = 0.0;
    offset = 0.0;
}

//--------------------------------------------------------------
void ofApp::updateAudio(){
    // update the sound playing system:
    ofSoundUpdate();
    
    float   media;
    
    float old_media = media;
    // (5) grab the fft, and put in into a "smoothed" array,
    //        by taking maximums, as peaks and then smoothing downward
    float * val = ofSoundGetSpectrum(nBandsToGet);        // request 128 values for fft
    for (int i = 0;i < nBandsToGet; i++){
        
        // let the smoothed value sink to zero:
        //fftSmoothed[i] *= 0.99f;
        media += fftSmoothed[i];
        // take the max, either the smoothed or the incoming:
        //if (fftSmoothed[i] < val[i])
        
        fftSmoothed[i] *= 0.65;
        fftSmoothed[i] += 0.35 * val[i];
        
    }
    media /= nBandsToGet;
    // cout << "media: " << ofToString(media) << endl;
    if(media>=maximo){
        maximo = media;
        //cout << "maximo: " << ofToString(maximo) << endl;
    }
    // Cheda
    if(media<=old_media) media = media * 0.005 + old_media * 0.995;
    //if(media<=old_media) media = media * 0.05 + old_media * 0.95;
    
    // alpha *= 0.45;
    // alfa Cholesteryl
    // alpha += 0.55 * ofMap(media, 0.0, 0.05, 100.0, 255.0);
    // alpha = ofMap(media, 0.0, 0.06, 50.0, 255.0);
    // alfa Cheda
    alpha *= 0.85;
    alpha += 0.15 * ofMap(media, 0.0, 0.06, 120.0, 255.0);
    
    //mixture.set(ofMap(media, 0.0, 0.07, 1.0, 80.0));
    mixture.set(ofMap(media, 0.0, 0.07, 0.0, 255.0));
    thresholdS.set(ofMap(media, 0.0, 0.07, 0.0, 255.0));
    //thresholdS.set(ofMap(media, 0.0, 0.07, 1.0, 255.0));
    //stela.set(ofMap(media, 0.0, 0.06, 4.5, 35.0));
    
    /*
    //float fboAlpha = 255.0;
    
    // Black alpha
    // alfa Cholesteryl
    //if (effects) fboAlpha = ofMap(media, 0.0, 0.06, 20.0, 0.9);
    //if (effects) fboAlpha = ofMap(media, 0.0, 0.06, 30.0, 0.5);
    //if (effects) fboAlpha = ofMap(media, 0.0, 0.06, 10.0, 3.5);
    
    // alfa Cheda
    //if (effects) fboAlpha = ofMap(media, 0.0, 0.06, 35.0, 0.01);
    */
    
}

//--------------------------------------------------------------
void ofApp::drawAudioFFT(){
    // draw the fft resutls:
    ofSetColor(255,255,255,255);
    
    float width = (float)(5*128) / nBandsToGet;
    for (int i = 0;i < nBandsToGet; i++){
        // (we use negative height here, because we want to flip them
        // because the top corner is 0,0)
        ofDrawRectangle(100+i*width,ofGetHeight()-100,width,-(fftSmoothed[i] * 200));
    }
    
    // draw player position:
    ofDrawRectangle(100, ofGetHeight() - 94, 5*128*krystallos.getPosition(), 3);
}

//--------------------------------------------------------------
void ofApp::setup3D(){
    
    renderPosition.set(0.0, 0.0, 0.0);
    
    cam.disableMouseInput();
    cam.setDistance(100);
    cam.setNearClip(1);
    cam.setFarClip(10000);
    
    setupLight();
    
    setupMaterials();
    
    setupModels();
    
    gui.loadFromFile("settings.xml");
    ofxLoadCamera(cam, "ofEasyCamSettings");
    renderPosition.set(renderX,renderY,renderZ);
    
}
//--------------------------------------------------------------
void ofApp::update3D(){
   
}

//--------------------------------------------------------------
void ofApp::draw3D(){
    
    //ofBackground(0);
    ofSetColor(255);
    
    light.enable();
    cam.begin();
    //cam.begin(ofRectangle(0, ofGetHeight()-fbo.getHeight(), fbo.getWidth(), fbo.getHeight()) );
    
    
    
    
    
    ofPushMatrix();
    
    
        rotationZ += 0.1;
        /*
        rotationX += 0.2;
        rotationY += 0.5;
        */
        ofRotateXDeg(rotationX);
        ofRotateZDeg(rotationZ);
        ofRotateYDeg(rotationY);
    
        ofTranslate(renderPosition);
    
        // celda.draw();
    
    
        material_02.begin();
            modelNoise();
        material_02.end();
    
        material_01.begin();
            //atomNoise();
            atomMovement();
        material_01.end();
    
        //model_01.drawWireframe();
    /*
        model_02.drawFaces();
        model_03.drawFaces();
        model_04.drawFaces();
        model_05.drawFaces();
    */
    
    ofPopMatrix();
    
        if(showGui) ofDrawGrid(10,5,true,true,true,true);
    
    cam.end();
    light.disable();
    ofDisableLighting();
    
    
}

//--------------------------------------------------------------
void ofApp::setupMaterials(){
    
    material_01.setAmbientColor(ofFloatColor(0.8f,0.0f,0.0f, 0.95f));
    // Cholesteryl
    // material_01.setDiffuseColor(ofFloatColor(0.0f,0.0f,0.8f, 0.95f));
    // Cheda
    material_01.setDiffuseColor(ofFloatColor(0.0f,0.45f,0.8f, 0.95f));
    
    //material_01.setEmissiveColor(ofFloatColor(0.0f,0.8f,0.0f, 0.95f));
    //material_01.setShininess(0.4f);
    
    // Cholesteryl
    // material_01.setSpecularColor(ofFloatColor(0.8f,0.8f,0.0f, 0.95f));
    // Cheda
    material_01.setSpecularColor(ofFloatColor(0.45f,0.65f,0.2f, 0.85f));
    
    
    
    //material_01.setAmbientColor(ofFloatColor(0.8f,0.0f,0.0f, 0.95f));
    material_02.setAmbientColor(ofFloatColor(0.8f,0.0f,0.0f, 0.95f));
    // Cholesteryl
    // material_02.setDiffuseColor(ofFloatColor(0.0f,0.0f,0.8f, 0.95f));
    // Cheda
    material_02.setDiffuseColor(ofFloatColor(0.3f,0.45f,0.25f, 0.85f));
    
    //material_02.setEmissiveColor(ofFloatColor(0.0f,0.8f,0.0f, 0.95f));
    //material_02.setShininess(0.4f);
    
    // Cholesteryl
    // material_02.setSpecularColor(ofFloatColor(0.8f,0.8f,0.0f, 0.95f));
    // Cheda
    material_02.setSpecularColor(ofFloatColor(0.15f,0.2f,0.5f, 0.65f));
    
    material_03.setAmbientColor(ofFloatColor(0.8f,0.0f,0.0f, 0.95f));
    material_03.setDiffuseColor(ofFloatColor(0.3f,0.45f,0.25f, 0.85f));
    material_03.setSpecularColor(ofFloatColor(0.15f,0.2f,0.5f, 0.65f));
    
}
//--------------------------------------------------------------
void ofApp::updateMaterials(){
    ofColor actual;
    
    actual = ambient;
    material_01.setAmbientColor(ofFloatColor(actual.r/255.0,actual.g/255.0,actual.b/255.0, actual.a/255.0));
    
    actual = diffuse;
    /*
    // 246 - 117
    //float new_alphaD = ofMap(media, 0.0, 0.04, 17.0, 255.0);
    //float value = ofMap(media, 0.0, 0.06, alphaD/255.0, 0.0);
    float new_alphaD;
    if (media<=0.006) {
        new_alphaD = 17.0;
    } else if (media<=0.02) {
        new_alphaD = ofMap(media, 0.006, 0.02, 17.0, 246.0);
    } else {
        new_alphaD = 246.0;
    }
    float value = ofMap(media, 0.0, 0.06, new_alphaD/255.0, 0.0);
    //cout << "media:" << ofToString(media) << endl;
    //float value = ofMap(media, 0.0, 0.06, new_alphaD/255.0, 0.0);
    material_01.setDiffuseColor(ofFloatColor(actual.r/255.0,actual.g/255.0,actual.b/255.0, value));
    */
    material_01.setDiffuseColor(ofFloatColor(actual.r/255.0,actual.g/255.0,actual.b/255.0, actual.a/255.0));
    
    actual = specular;
    material_01.setSpecularColor(ofFloatColor(actual.r/255.0,actual.g/255.0,actual.b/255.0, actual.a/255.0));

    actual = ambient2;
    material_02.setAmbientColor(ofFloatColor(actual.r/255.0,actual.g/255.0,actual.b/255.0, actual.a/255.0));
    material_03.setAmbientColor(ofFloatColor(actual.r/255.0,actual.g/255.0,actual.b/255.0, actual.a/255.0));
    actual = diffuse2;
    material_02.setDiffuseColor(ofFloatColor(actual.r/255.0,actual.g/255.0,actual.b/255.0, actual.a/255.0));
    material_03.setDiffuseColor(ofFloatColor(actual.r/255.0,actual.g/255.0,actual.b/255.0, actual.a/510.0));
    actual = specular2;
    material_02.setSpecularColor(ofFloatColor(actual.r/255.0,actual.g/255.0,actual.b/255.0, actual.a/255.0));
    material_03.setSpecularColor(ofFloatColor(actual.r/255.0,actual.g/255.0,actual.b/255.0, actual.a/255.0));

}

//--------------------------------------------------------------
void ofApp::setupCelda(){
    
    
    celda.setMode(OF_PRIMITIVE_TRIANGLES);
    //celda.enableColors();
    celda.enableIndices();
    
    
    ofVec3f point_0 = ofVec3f(0.0, 0.0, 0.0);
    ofVec3f point_1 = ofVec3f(15.13400, 0.0, 0.0);
    ofVec3f point_2 = ofVec3f(15.13400, 7.40300, 0.0);
    ofVec3f point_3 = ofVec3f(0, 7.40300, 0.0);
    
    ofVec3f point_4 = ofVec3f(-4.5425, 0, 16.6619);
    ofVec3f point_5 = ofVec3f(10.5915, 0, 16.6619);
    ofVec3f point_6 = ofVec3f(10.5915, 7.40300, 16.6619);
    ofVec3f point_7 = ofVec3f(-4.5425, 7.40300, 16.6619);
    
    addFace(celda, point_0, point_1, point_2, point_3);
    addFace(celda, point_0, point_1, point_5, point_4);
    addFace(celda, point_2, point_3, point_7, point_6);
    addFace(celda, point_4, point_5, point_6, point_7);
    addFace(celda, point_0, point_3, point_7, point_4);
    addFace(celda, point_1, point_2, point_6, point_5);
    
}

//--------------------------------------------------------------
void ofApp::updateCelda(){
   
}

//--------------------------------------------------------------
void ofApp::setupLight(){
    
    light.setPosition(lightX, lightY, lightZ);
    light.setSpotlight();
    light.setOrientation(ofQuaternion(180, ofVec3f(0,0,1)));
    // size of the cone of emitted light, angle between light axis and side of cone //
    // angle range between 0 - 90 in degrees //
    light.setSpotlightCutOff(lightAngle.get());
    // rate of falloff, illumitation decreases as the angle from the cone axis increases //
    // range 0 - 128, zero is even illumination, 128 is max falloff //
    light.setSpotConcentration(lightConentration.get());
    
    
    // Directional Lights emit light based on their orientation, regardless of their position //
    directionalLight.setDiffuseColor(ofColor(0.f, 0.f, 255.f));
    directionalLight.setSpecularColor(ofColor(255.f, 255.f, 255.f));
    directionalLight.setDirectional();
    // set the direction of the light
    directionalLight.setOrientation( ofVec3f(90, 0, 0) );
    
    ambientLight.setPosition(lightX, lightY, lightZ);
    ambientLight.setAmbientColor(ofFloatColor(0.4f,0.1f,0.4f));
    
}
//--------------------------------------------------------------
void ofApp::updateLight(){
    light.setPosition(lightX, lightY, lightZ);
    light.setSpotlightCutOff(lightAngle.get());
    light.setSpotConcentration(ofMap(media, 0.0, 0.06, lightConentration.get(), 0.0));
}

void ofApp::addFace(ofMesh& _mesh, ofVec3f _a, ofVec3f _b, ofVec3f _c) {
    ofVec3f normal = ((_b - _a).cross(_c - _a)).normalize();
    _mesh.addNormal(normal);
    _mesh.addVertex(_a);
    _mesh.addNormal(normal);
    _mesh.addVertex(_b);
    _mesh.addNormal(normal);
    _mesh.addVertex(_c);
}

//--------------------------------------------------------------
void ofApp::addFace(ofMesh& _mesh, ofVec3f _a, ofVec3f _b, ofVec3f _c, ofVec3f _d) {
    addFace(_mesh, _a, _b, _c);
    addFace(_mesh, _a, _c, _d);
}

//--------------------------------------------------------------
void ofApp::setupAtoms(){
    
    float a = 15.13400;
    float b = 7.40300;
    float c = 17.27000;
    float alpha = 90.0000;
    float beta = 105.2500;
    float gamma = 90.0000;
    
    
    /*
    // FULL Cholesteryl benzoate
    Oatom.push_back(ofVec3f(-1.871555, 1.916637, 10.262050));
    Oatom.push_back(ofVec3f(12.463006, 5.618136, 6.399827));
    Oatom.push_back(ofVec3f(-2.065716, 2.243109, 8.054351));
    Oatom.push_back(ofVec3f(12.657166, 5.944609, 8.607526));
    Oatom.push_back(ofVec3f( 4.235507, 2.430405, 8.905773));
    Oatom.push_back(ofVec3f( 6.355943, 6.131905, 7.756104));
    Catom.push_back(ofVec3f( 9.637327, 2.835349, 10.763573));
    Catom.push_back(ofVec3f( 0.954124, 6.536849, 5.898304));
    Catom.push_back(ofVec3f(11.114754, 3.050036, 10.440332));
    Catom.push_back(ofVec3f(-0.523303, 6.751536, 6.221544));
    Catom.push_back(ofVec3f(11.828935, 1.732302, 10.385348));
    Catom.push_back(ofVec3f(-1.237484, 5.433802, 6.276529));
    Catom.push_back(ofVec3f(11.637461, 0.932778, 11.664980));
    Catom.push_back(ofVec3f(-1.046010, 4.634278, 4.996897));
    Catom.push_back(ofVec3f(10.172132, 0.799524, 12.054868));
    Catom.push_back(ofVec3f( 0.419319, 4.501024, 4.607008));
    Catom.push_back(ofVec3f( 9.678114, 7.025447, 12.368111));
    Catom.push_back(ofVec3f( 0.913337, 3.323947, 4.293766));
    Catom.push_back(ofVec3f( 8.273008, 6.795954, 12.831311));
    Catom.push_back(ofVec3f( 2.318443, 3.094454, 3.830565));
    Catom.push_back(ofVec3f( 7.548034, 0.681076, 13.214535));
    Catom.push_back(ofVec3f( 3.043416, 4.382576, 3.447342));
    Catom.push_back(ofVec3f( 7.860090, 1.787084, 12.203158));
    Catom.push_back(ofVec3f( 2.731361, 5.488584, 4.458718));
    Catom.push_back(ofVec3f( 9.378803, 2.095049, 12.078195));
    Catom.push_back(ofVec3f( 1.212646, 5.796549, 4.583682));
    Catom.push_back(ofVec3f( 7.017544, 3.042633, 12.451421));
    Catom.push_back(ofVec3f( 3.573906, 6.744133, 4.210456));
    Catom.push_back(ofVec3f( 5.490647, 2.768722, 12.656362));
    Catom.push_back(ofVec3f( 5.100804, 6.470222, 4.005516));
    Catom.push_back(ofVec3f( 5.261815, 1.717496, 13.767709));
    Catom.push_back(ofVec3f( 5.329636, 5.418996, 2.894168));
    Catom.push_back(ofVec3f( 6.057170, 0.481195, 13.326169));
    Catom.push_back(ofVec3f( 4.534281, 4.182695, 3.335708));
    Catom.push_back(ofVec3f( 5.526596, 6.788551, 14.234241));
    Catom.push_back(ofVec3f( 5.064856, 3.087051, 2.427635));
    Catom.push_back(ofVec3f( 4.044343, 7.121686, 14.430852));
    Catom.push_back(ofVec3f( 6.547108, 3.420186, 2.231025));
    Catom.push_back(ofVec3f( 3.838600, 1.140062, 13.864347));
    Catom.push_back(ofVec3f( 6.752851, 4.841562, 2.797529));
    Catom.push_back(ofVec3f( 5.730628, 2.250512, 15.112322));
    Catom.push_back(ofVec3f( 4.860822, 5.952012, 1.549554));
    Catom.push_back(ofVec3f( 9.866405, 2.953797, 13.276184));
    Catom.push_back(ofVec3f( 0.725047, 6.655297, 3.385693));
    Catom.push_back(ofVec3f( 2.759587, 1.924780, 14.652454));
    Catom.push_back(ofVec3f( 7.831863, 5.626280, 2.009422));
    Catom.push_back(ofVec3f( 2.603315, 3.368365, 14.204250));
    Catom.push_back(ofVec3f( 7.988135, 7.069865, 2.457626));
    Catom.push_back(ofVec3f( 1.395879, 1.214092, 14.530823));
    Catom.push_back(ofVec3f( 9.195573, 4.915592, 2.131054));
    Catom.push_back(ofVec3f( 0.354217, 1.650869, 15.520538));
    Catom.push_back(ofVec3f(10.237233, 5.352369, 1.141338));
    Catom.push_back(ofVec3f(-0.953040, 0.917972, 15.397240));
    Catom.push_back(ofVec3f(11.544491, 4.619472, 1.264637));
    Catom.push_back(ofVec3f(-2.003789, 1.273316, 16.436941));
    Catom.push_back(ofVec3f(12.595241, 4.974816, 0.224935));
    Catom.push_back(ofVec3f(-2.450692, 2.716901, 16.405283));
    Catom.push_back(ofVec3f(13.042143, 6.418401, 0.256593));
    Catom.push_back(ofVec3f(-3.205711, 0.340538, 16.293650));
    Catom.push_back(ofVec3f(13.797161, 4.042038, 0.368227));
    Catom.push_back(ofVec3f(-1.369690, 2.124661, 9.037402));
    Catom.push_back(ofVec3f(11.961141, 5.826161, 7.624475));
    Catom.push_back(ofVec3f( 0.104514, 2.257915, 9.025739));
    Catom.push_back(ofVec3f(10.486937, 5.959415, 7.636138));
    Catom.push_back(ofVec3f( 0.884358, 2.191288, 10.178741));
    Catom.push_back(ofVec3f( 9.707093, 5.892788, 6.483137));
    Catom.push_back(ofVec3f( 2.244763, 2.265318, 10.107094));
    Catom.push_back(ofVec3f( 8.346687, 5.966818, 6.554782));
    Catom.push_back(ofVec3f( 2.878444, 2.383766, 8.887445));
    Catom.push_back(ofVec3f( 7.713006, 6.085266, 7.774432));
    Catom.push_back(ofVec3f( 2.114189, 2.465199, 7.732777));
    Catom.push_back(ofVec3f( 8.477262, 6.166699, 8.929100));
    Catom.push_back(ofVec3f( 0.739706, 2.398572, 7.822751));
    Catom.push_back(ofVec3f( 9.851744, 6.100072, 8.839126));
    Catom.push_back(ofVec3f( 4.946074, 2.442990, 7.659464));
    Catom.push_back(ofVec3f( 5.645378, 6.144490, 9.002412));
    Catom.push_back(ofVec3f( 6.410104, 2.413378, 7.896063));
    Catom.push_back(ofVec3f( 4.181347, 6.114878, 8.765814));
    Catom.push_back(ofVec3f( 7.189686, 2.354154, 6.613099));
    Catom.push_back(ofVec3f( 3.401766, 6.055654, 10.048778));
    Catom.push_back(ofVec3f( 8.684903, 2.272721, 6.763056));
    Catom.push_back(ofVec3f( 1.906548, 5.974221, 9.898821));
    Catom.push_back(ofVec3f( 9.402139, 2.191288, 5.436770));
    Catom.push_back(ofVec3f( 1.189312, 5.892788, 11.225106));
    Catom.push_back(ofVec3f(10.894040, 2.028422, 5.493421));
    Catom.push_back(ofVec3f(-0.302590, 5.729922, 11.168456));
    Catom.push_back(ofVec3f(11.537418, 1.932183, 4.193795));
    Catom.push_back(ofVec3f(-0.945968, 5.633683, 12.468081));
    Catom.push_back(ofVec3f(12.026339, 1.021614, 3.543981));
    Catom.push_back(ofVec3f(-1.434888, 4.723114, 13.117895));
    Hatom.push_back(ofVec3f( 9.248477, 2.132064, 10.097097));
    Hatom.push_back(ofVec3f( 1.342974, 5.833564, 6.564779));
    Hatom.push_back(ofVec3f( 9.237768, 3.649679, 10.913528));
    Hatom.push_back(ofVec3f( 1.353682, 7.351179, 5.748348));
    Hatom.push_back(ofVec3f(11.535076, 3.664485, 11.146796));
    Hatom.push_back(ofVec3f(-0.943626, 7.365985, 5.515080));
    Hatom.push_back(ofVec3f(11.284059, 3.568246, 9.680550));
    Hatom.push_back(ofVec3f(-0.692608, 7.269746, 6.981327));
    Hatom.push_back(ofVec3f(11.539830, 1.103047, 9.630565));
    Hatom.push_back(ofVec3f(-0.948380, 4.804547, 7.031312));
    Hatom.push_back(ofVec3f(-2.925637, 1.576839, 12.396437));
    Hatom.push_back(ofVec3f(13.517088, 5.278339, 4.265440));
    Hatom.push_back(ofVec3f(-3.161614, 7.277149, 11.596665));
    Hatom.push_back(ofVec3f(11.972385, 7.277149, 11.596665));
    Hatom.push_back(ofVec3f(-1.380935, 3.575649, 5.065211));
    Hatom.push_back(ofVec3f(13.753065, 3.575649, 5.065211));
    Hatom.push_back(ofVec3f(10.163767, 6.225923, 12.346451));
    Hatom.push_back(ofVec3f( 0.427684, 2.524423, 4.315426));
    Hatom.push_back(ofVec3f( 8.237041, 6.196311, 13.529444));
    Hatom.push_back(ofVec3f( 2.354409, 2.494811, 3.132433));
    Hatom.push_back(ofVec3f( 7.627327, 6.351774, 12.213156));
    Hatom.push_back(ofVec3f( 2.964123, 2.650274, 4.448721));
    Hatom.push_back(ofVec3f( 7.970608, 0.895763, 14.062624));
    Hatom.push_back(ofVec3f( 2.620842, 4.597263, 2.599253));
    Hatom.push_back(ofVec3f( 7.474621, 1.406570, 11.163458));
    Hatom.push_back(ofVec3f( 3.116829, 5.108070, 5.498419));
    Hatom.push_back(ofVec3f( 7.170341, 3.716306, 11.779947));
    Hatom.push_back(ofVec3f( 3.421109, 0.014806, 4.881929));
    Hatom.push_back(ofVec3f( 7.359293, 3.509022, 13.362824));
    Hatom.push_back(ofVec3f( 3.232157, 7.210522, 3.299052));
    Hatom.push_back(ofVec3f( 5.142385, 2.228303, 11.779947));
    Hatom.push_back(ofVec3f( 5.449065, 5.929803, 4.881929));
    Hatom.push_back(ofVec3f( 5.090771, 3.671888, 12.912954));
    Hatom.push_back(ofVec3f( 5.500680, 7.373388, 3.748923));
    Hatom.push_back(ofVec3f( 5.700744, 0.259105, 12.396437));
    Hatom.push_back(ofVec3f( 4.890707, 3.960605, 4.265440));
    Hatom.push_back(ofVec3f( 5.965224, 6.847775, 14.979027));
    Hatom.push_back(ofVec3f( 4.626226, 3.146275, 1.682850));
    Hatom.push_back(ofVec3f( 5.621849, 6.011236, 13.796034));
    Hatom.push_back(ofVec3f( 4.969602, 2.309736, 2.865843));
    Hatom.push_back(ofVec3f( 3.841875, 7.010641, 15.328927));
    Hatom.push_back(ofVec3f( 6.749576, 3.309141, 1.332950));
    Hatom.push_back(ofVec3f( 3.406189, 6.536849, 14.095948));
    Hatom.push_back(ofVec3f( 7.185262, 2.835349, 2.565929));
    Hatom.push_back(ofVec3f( 3.445683, 0.984599, 13.062911));
    Hatom.push_back(ofVec3f( 7.145767, 4.686099, 3.598966));
    Hatom.push_back(ofVec3f( 5.645787, 1.495406, 15.762135));
    Hatom.push_back(ofVec3f( 4.945664, 5.196906, 0.899742));
    Hatom.push_back(ofVec3f( 6.712816, 2.257915, 15.178970));
    Hatom.push_back(ofVec3f( 3.878635, 5.959415, 1.482907));
    Hatom.push_back(ofVec3f( 5.247807, 2.976006, 15.445560));
    Hatom.push_back(ofVec3f( 5.343644, 6.677506, 1.216317));
    Hatom.push_back(ofVec3f( 9.494599, 2.546632, 14.079287));
    Hatom.push_back(ofVec3f( 1.096851, 6.248132, 2.582590));
    Hatom.push_back(ofVec3f(10.793205, 2.887170, 13.312840));
    Hatom.push_back(ofVec3f(-0.201754, 6.588670, 3.349037));
    Hatom.push_back(ofVec3f( 9.562833, 3.886575, 13.162883));
    Hatom.push_back(ofVec3f( 1.028619, 0.185075, 3.498994));
    Hatom.push_back(ofVec3f( 2.988274, 2.006213, 15.628840));
    Hatom.push_back(ofVec3f( 7.603176, 5.707713, 1.033036));
    Hatom.push_back(ofVec3f( 2.352999, 3.190693, 13.129559));
    Hatom.push_back(ofVec3f( 8.238451, 6.892193, 3.532318));
    Hatom.push_back(ofVec3f( 1.785274, 3.768127, 14.545818));
    Hatom.push_back(ofVec3f( 8.806176, 0.066627, 2.116058));
    Hatom.push_back(ofVec3f( 3.356221, 3.723709, 14.279228));
    Hatom.push_back(ofVec3f( 7.235229, 0.022209, 2.382648));
    Hatom.push_back(ofVec3f( 1.617248, 0.103642, 14.829070));
    Hatom.push_back(ofVec3f( 8.974203, 3.805142, 1.832807));
    Hatom.push_back(ofVec3f( 1.052911, 1.302928, 13.679400));
    Hatom.push_back(ofVec3f( 9.538540, 5.004428, 2.982476));
    Hatom.push_back(ofVec3f( 0.200606, 2.628065, 15.528869));
    Hatom.push_back(ofVec3f(10.390845, 6.329565, 1.133008));
    Hatom.push_back(ofVec3f( 0.707466, 1.450988, 16.445272));
    Hatom.push_back(ofVec3f( 9.883985, 5.152488, 0.216605));
    Hatom.push_back(ofVec3f(-1.312653, 1.332540, 14.529156));
    Hatom.push_back(ofVec3f(11.904103, 5.034040, 2.132720));
    Hatom.push_back(ofVec3f(-0.711953, 7.314164, 15.378912));
    Hatom.push_back(ofVec3f(11.303403, 3.612664, 1.282965));
    Hatom.push_back(ofVec3f( 2.920768, 0.999405, 0.666474));
    Hatom.push_back(ofVec3f( 7.670683, 4.700905, 15.995402));
    Hatom.push_back(ofVec3f(-3.007802, 2.220900, 15.528869));
    Hatom.push_back(ofVec3f(13.599253, 5.922400, 1.133008));
    Hatom.push_back(ofVec3f(-1.718516, 3.257320, 16.461933));
    Hatom.push_back(ofVec3f(12.309966, 6.958820, 0.199943));
    Hatom.push_back(ofVec3f( 1.310562, 2.813140, 0.299914));
    Hatom.push_back(ofVec3f( 9.280889, 6.514640, 16.361963));
    Hatom.push_back(ofVec3f( 0.528137, 0.370150, 0.283251));
    Hatom.push_back(ofVec3f(10.063313, 4.071650, 16.378626));
    Hatom.push_back(ofVec3f(-2.960988, 6.662700, 16.245331));
    Hatom.push_back(ofVec3f(13.552439, 2.961200, 0.416547));
    Hatom.push_back(ofVec3f(-3.548037, 0.370150, 15.178970));
    Hatom.push_back(ofVec3f(14.139488, 4.071650, 1.482907));
    Hatom.push_back(ofVec3f( 0.420695, 2.035825, 10.946853));
    Hatom.push_back(ofVec3f(10.170755, 5.737325, 5.715024));
    Hatom.push_back(ofVec3f( 2.864847, 2.206094, 10.863544));
    Hatom.push_back(ofVec3f( 7.726603, 5.907594, 5.798333));
    Hatom.push_back(ofVec3f( 2.521872, 2.465199, 6.848032));
    Hatom.push_back(ofVec3f( 8.069579, 6.166699, 9.813845));
    Hatom.push_back(ofVec3f( 0.156402, 2.472602, 7.031312));
    Hatom.push_back(ofVec3f(10.435049, 6.174102, 9.630565));
    Hatom.push_back(ofVec3f( 4.720798, 3.360962, 7.164607));
    Hatom.push_back(ofVec3f( 5.870654, 7.062462, 9.497270));
    Hatom.push_back(ofVec3f( 4.639079, 1.673078, 7.131283));
    Hatom.push_back(ofVec3f( 5.952372, 5.374578, 9.530594));
    Hatom.push_back(ofVec3f( 6.571524, 1.710093, 8.314277));
    Hatom.push_back(ofVec3f( 4.019927, 5.411593, 8.347600));
    Hatom.push_back(ofVec3f( 6.707706, 3.168484, 8.480895));
    Hatom.push_back(ofVec3f( 3.883744, 6.869984, 8.180982));
    Hatom.push_back(ofVec3f( 7.003161, 3.168484, 6.064923));
    Hatom.push_back(ofVec3f( 3.588290, 6.869984, 10.596954));
    Hatom.push_back(ofVec3f( 6.915369, 1.613854, 6.164895));
    Hatom.push_back(ofVec3f( 3.676082, 5.315354, 10.496983));
    Hatom.push_back(ofVec3f( 9.041519, 1.495406, 7.414535));
    Hatom.push_back(ofVec3f( 1.549931, 5.196906, 9.247342));
    Hatom.push_back(ofVec3f( 8.887191, 3.109260, 7.147945));
    Hatom.push_back(ofVec3f( 1.704260, 6.810760, 9.513931));
    Hatom.push_back(ofVec3f( 9.147811, 2.976006, 4.915254));
    Hatom.push_back(ofVec3f( 1.443640, 6.677506, 11.746624));
    Hatom.push_back(ofVec3f( 9.112999, 1.391764, 4.931916));
    Hatom.push_back(ofVec3f( 1.478452, 5.093264, 11.729961));
    Hatom.push_back(ofVec3f(11.069687, 1.391764, 5.914966));
    Hatom.push_back(ofVec3f(-0.478236, 5.093264, 10.746910));
    Hatom.push_back(ofVec3f(11.305758, 2.753916, 6.048261));
    Hatom.push_back(ofVec3f(-0.714308, 6.455416, 10.613615));
    Hatom.push_back(ofVec3f(11.616346, 2.813140, 3.632289));
    Hatom.push_back(ofVec3f(-1.024896, 6.514640, 13.029588));
    Hatom.push_back(ofVec3f(12.421625, 0.592240, 2.565929));
    Hatom.push_back(ofVec3f(-1.830174, 4.293740, 14.095948));
    Hatom.push_back(ofVec3f(11.808475, 0.000000, 4.148807));
    Hatom.push_back(ofVec3f(11.808475, 7.403000, 4.148807));
    Hatom.push_back(ofVec3f(-1.217025, 3.701500, 12.513069));
    Catom.push_back(ofVec3f(-3.305065, 1.732302, 10.385348));
    Catom.push_back(ofVec3f(13.896516, 5.433802, 6.276529));
    Oatom.push_back(ofVec3f(13.262444, 1.916637, 10.262050));
    Oatom.push_back(ofVec3f(-2.670994, 5.618136, 6.399827));
    Hatom.push_back(ofVec3f(11.972385, -0.125851, 11.596665));
    Hatom.push_back(ofVec3f(12.208364, 1.576839, 12.396437));
    Hatom.push_back(ofVec3f(-1.616912, 5.278339, 4.265440));
    Catom.push_back(ofVec3f( 9.678114, -0.377553, 12.368111));
    Catom.push_back(ofVec3f(10.172132, 8.202524, 12.054868));
    Catom.push_back(ofVec3f( 7.548034, 8.084076, 13.214535));
    Catom.push_back(ofVec3f( 8.273008, -0.607046, 12.831311));
    Hatom.push_back(ofVec3f( 3.421109, 7.417806, 4.881929));
    Catom.push_back(ofVec3f( 5.526596, -0.614449, 14.234241));
    Catom.push_back(ofVec3f( 6.057170, 7.884195, 13.326169));
    Catom.push_back(ofVec3f( 3.838600, 8.543062, 13.864347));
    Catom.push_back(ofVec3f( 4.044343, -0.281314, 14.430852));
    Hatom.push_back(ofVec3f( 1.028619, 7.588075, 3.498994));
    Hatom.push_back(ofVec3f( 7.235229, 7.425209, 2.382648));
    Hatom.push_back(ofVec3f( 8.806176, 7.469627, 2.116058));
    Hatom.push_back(ofVec3f(-0.711953, -0.088836, 15.378912));
    Hatom.push_back(ofVec3f(-1.621781, 0.999405, 17.328352));
    Hatom.push_back(ofVec3f(12.213232, 4.700905, -0.666474));
    Hatom.push_back(ofVec3f(-3.231987, 2.813140, 16.961790));
    Hatom.push_back(ofVec3f(13.823438, 6.514640, -0.299913));
    Hatom.push_back(ofVec3f(-2.960988, -0.740300, 16.245331));
    Hatom.push_back(ofVec3f(-4.014412, 0.370150, 16.945127));
    Hatom.push_back(ofVec3f(14.605863, 4.071650, -0.283251));
    Hatom.push_back(ofVec3f(-3.594170, 1.103047, 9.630565));
    Catom.push_back(ofVec3f(-3.496539, 0.932778, 11.664980));
    Catom.push_back(ofVec3f(-4.019246, 3.050036, 10.440332));
    Catom.push_back(ofVec3f(14.087990, 4.634278, 4.996897));
    Hatom.push_back(ofVec3f(14.185620, 4.804547, 7.031312));
    Catom.push_back(ofVec3f(14.610697, 6.751536, 6.221544));
    Catom.push_back(ofVec3f(13.764309, 2.124661, 9.037402));
    Catom.push_back(ofVec3f(-3.172859, 5.826161, 7.624475));
    Hatom.push_back(ofVec3f(10.163767, -1.177077, 12.346451));
    Catom.push_back(ofVec3f( 9.378803, 9.498049, 12.078195));
    Catom.push_back(ofVec3f(11.637461, 8.335778, 11.664980));
    Catom.push_back(ofVec3f( 7.860090, 9.190084, 12.203158));
    Hatom.push_back(ofVec3f( 7.970608, 8.298763, 14.062624));
    Hatom.push_back(ofVec3f( 7.627327, -1.051226, 12.213156));
    Hatom.push_back(ofVec3f( 8.237041, -1.206689, 13.529444));
    Hatom.push_back(ofVec3f( 5.621849, -1.391764, 13.796034));
    Hatom.push_back(ofVec3f( 5.965224, -0.555225, 14.979027));
    Hatom.push_back(ofVec3f( 5.700744, 7.662105, 12.396437));
    Catom.push_back(ofVec3f( 5.261815, 9.120496, 13.767709));
    Hatom.push_back(ofVec3f( 3.445683, 8.387599, 13.062911));
    Catom.push_back(ofVec3f( 2.759587, 9.327780, 14.652454));
    Hatom.push_back(ofVec3f( 3.406189, -0.866151, 14.095948));
    Hatom.push_back(ofVec3f( 3.841875, -0.392359, 15.328927));
    Catom.push_back(ofVec3f(-4.961868, 0.799524, 12.054868));
    Hatom.push_back(ofVec3f(-3.161614, -0.125851, 11.596665));
    Catom.push_back(ofVec3f(-5.496673, 2.835349, 10.763573));
    Hatom.push_back(ofVec3f(-3.849941, 3.568246, 9.680550));
    Hatom.push_back(ofVec3f(-3.598924, 3.664485, 11.146796));
    Catom.push_back(ofVec3f(15.553319, 4.501024, 4.607008));
    Hatom.push_back(ofVec3f(14.190374, 7.365985, 5.515080));
    Hatom.push_back(ofVec3f(14.441392, 7.269746, 6.981327));
    Catom.push_back(ofVec3f(16.088123, 6.536849, 5.898304));
    Oatom.push_back(ofVec3f(13.068284, 2.243109, 8.054351));
    Catom.push_back(ofVec3f(15.238514, 2.257915, 9.025739));
    Catom.push_back(ofVec3f(-4.647063, 5.959415, 7.636138));
    Oatom.push_back(ofVec3f(-2.476833, 5.944609, 8.607526));
    Catom.push_back(ofVec3f( 9.637327, 10.238349, 10.763573));
    Catom.push_back(ofVec3f( 9.866405, 10.356796, 13.276184));
    Catom.push_back(ofVec3f(11.828935, 9.135302, 10.385348));
    Hatom.push_back(ofVec3f(12.208364, 8.979839, 12.396437));
    Hatom.push_back(ofVec3f( 7.474621, 8.809570, 11.163458));
    Catom.push_back(ofVec3f( 7.017544, 10.445633, 12.451421));
    Catom.push_back(ofVec3f( 5.490647, 10.171722, 12.656362));
    Catom.push_back(ofVec3f( 5.730628, 9.653512, 15.112322));
    Catom.push_back(ofVec3f( 1.395879, 8.617092, 14.530823));
    Catom.push_back(ofVec3f( 2.603315, 10.771365, 14.204250));
    Hatom.push_back(ofVec3f( 2.988274, 9.409213, 15.628840));
    Catom.push_back(ofVec3f(-5.755197, 2.095049, 12.078195));
    Catom.push_back(ofVec3f(-5.455886, -0.377553, 12.368111));
    Hatom.push_back(ofVec3f(-5.885523, 2.132064, 10.097097));
    Hatom.push_back(ofVec3f(-5.896232, 3.649679, 10.913528));
    Catom.push_back(ofVec3f(16.047337, 3.323947, 4.293766));
    Catom.push_back(ofVec3f(16.346645, 5.796549, 4.583682));
    Hatom.push_back(ofVec3f(16.487682, 7.351179, 5.748348));
    Hatom.push_back(ofVec3f(16.476974, 5.833564, 6.564779));
    Catom.push_back(ofVec3f(15.873706, 2.398572, 7.822751));
    Catom.push_back(ofVec3f(16.018358, 2.191288, 10.178741));
    Catom.push_back(ofVec3f(-5.426907, 5.892788, 6.483137));
    Catom.push_back(ofVec3f(-5.282256, 6.100072, 8.839126));
    Hatom.push_back(ofVec3f( 9.248477, 9.535064, 10.097097));
    Hatom.push_back(ofVec3f( 9.237768, 11.052679, 10.913528));
    Catom.push_back(ofVec3f(11.114754, 10.453035, 10.440332));
    Hatom.push_back(ofVec3f( 9.494599, 9.949632, 14.079287));
    Hatom.push_back(ofVec3f(10.793205, 10.290170, 13.312840));
    Hatom.push_back(ofVec3f( 9.562833, 11.289575, 13.162883));
    Hatom.push_back(ofVec3f(11.539830, 8.506047, 9.630565));
    Oatom.push_back(ofVec3f(13.262444, 9.319636, 10.262050));
    Hatom.push_back(ofVec3f( 7.170341, 11.119306, 11.779947));
    Hatom.push_back(ofVec3f( 7.359293, 10.912022, 13.362824));
    Hatom.push_back(ofVec3f( 5.142385, 9.631303, 11.779947));
    Hatom.push_back(ofVec3f( 5.090771, 11.074888, 12.912954));
    Hatom.push_back(ofVec3f( 5.645787, 8.898406, 15.762135));
    Hatom.push_back(ofVec3f( 5.247807, 10.379005, 15.445560));
    Hatom.push_back(ofVec3f( 6.712816, 9.660915, 15.178970));
    Hatom.push_back(ofVec3f( 1.052911, 8.705928, 13.679400));
    Catom.push_back(ofVec3f( 0.354217, 9.053869, 15.520538));
    Hatom.push_back(ofVec3f( 1.617248, 7.506642, 14.829070));
    Hatom.push_back(ofVec3f( 2.352999, 10.593693, 13.129559));
    Hatom.push_back(ofVec3f( 1.785274, 11.171127, 14.545818));
    Hatom.push_back(ofVec3f( 3.356221, 11.126709, 14.279228));
    Catom.push_back(ofVec3f(-7.273910, 1.787084, 12.203158));
    Catom.push_back(ofVec3f(-5.267595, 2.953797, 13.276184));
    Catom.push_back(ofVec3f(-6.860991, -0.607046, 12.831311));
    Hatom.push_back(ofVec3f(-4.970233, -1.177077, 12.346451));
    Hatom.push_back(ofVec3f(15.561684, 2.524423, 4.315426));
    Catom.push_back(ofVec3f(17.452442, 3.094454, 3.830565));
    Catom.push_back(ofVec3f(15.859047, 6.655297, 3.385693));
    Catom.push_back(ofVec3f(17.865360, 5.488584, 4.458718));
    Hatom.push_back(ofVec3f(15.290402, 2.472602, 7.031312));
    Catom.push_back(ofVec3f(17.248190, 2.465199, 7.732777));
    Hatom.push_back(ofVec3f(15.554695, 2.035825, 10.946853));
    Catom.push_back(ofVec3f(17.378763, 2.265318, 10.107094));
    Catom.push_back(ofVec3f(-6.787313, 5.966818, 6.554782));
    Hatom.push_back(ofVec3f(-4.963244, 5.737325, 5.715024));
    Catom.push_back(ofVec3f(-6.656738, 6.166699, 8.929100));
    Hatom.push_back(ofVec3f(-4.698951, 6.174102, 9.630565));
    Hatom.push_back(ofVec3f(11.284059, 10.971246, 9.680550));
    Hatom.push_back(ofVec3f(11.535076, 11.067485, 11.146796));
    Catom.push_back(ofVec3f(13.764309, 9.527660, 9.037402));
    Catom.push_back(ofVec3f(-0.953040, 8.320971, 15.397240));
    Hatom.push_back(ofVec3f( 0.707466, 8.853988, 16.445272));
    Hatom.push_back(ofVec3f( 0.200606, 10.031065, 15.528869));
    Hatom.push_back(ofVec3f(-7.659379, 1.406570, 11.163458));
    Catom.push_back(ofVec3f(-8.116455, 3.042633, 12.451421));
    Catom.push_back(ofVec3f(-7.585966, 0.681076, 13.214535));
    Hatom.push_back(ofVec3f(-5.639400, 2.546632, 14.079287));
    Hatom.push_back(ofVec3f(-4.340795, 2.887170, 13.312840));
    Hatom.push_back(ofVec3f(-5.571167, 3.886575, 13.162883));
    Hatom.push_back(ofVec3f(-7.506672, -1.051226, 12.213156));
    Hatom.push_back(ofVec3f(-6.896958, -1.206689, 13.529444));
    Hatom.push_back(ofVec3f(17.488409, 2.494811, 3.132433));
    Hatom.push_back(ofVec3f(18.098124, 2.650274, 4.448721));
    Catom.push_back(ofVec3f(18.177416, 4.382576, 3.447342));
    Hatom.push_back(ofVec3f(16.230850, 6.248132, 2.582590));
    Hatom.push_back(ofVec3f(14.932245, 6.588670, 3.349037));
    Hatom.push_back(ofVec3f(16.162619, 7.588075, 3.498994));
    Hatom.push_back(ofVec3f(18.250828, 5.108070, 5.498419));
    Catom.push_back(ofVec3f(18.707905, 6.744133, 4.210456));
    Hatom.push_back(ofVec3f(17.655872, 2.465199, 6.848032));
    Catom.push_back(ofVec3f(18.012444, 2.383766, 8.887445));
    Hatom.push_back(ofVec3f(17.998848, 2.206094, 10.863544));
    Hatom.push_back(ofVec3f(-7.407397, 5.907594, 5.798333));
    Catom.push_back(ofVec3f(-7.420994, 6.085266, 7.774432));
    Hatom.push_back(ofVec3f(-7.064421, 6.166699, 9.813845));
    Oatom.push_back(ofVec3f(13.068284, 9.646109, 8.054351));
    Catom.push_back(ofVec3f(15.238514, 9.660915, 9.025739));
    Hatom.push_back(ofVec3f(-1.312653, 8.735540, 14.529156));
    Catom.push_back(ofVec3f(-2.003789, 8.676316, 16.436941));
    Catom.push_back(ofVec3f(-9.643353, 2.768722, 12.656362));
    Hatom.push_back(ofVec3f(-7.963658, 3.716306, 11.779947));
    Hatom.push_back(ofVec3f(-7.774707, 3.509022, 13.362824));
    Catom.push_back(ofVec3f(-9.076830, 0.481195, 13.326169));
    Hatom.push_back(ofVec3f(-7.163392, 0.895763, 14.062624));
    Hatom.push_back(ofVec3f(17.754843, 4.597263, 2.599253));
    Catom.push_back(ofVec3f(19.668282, 4.182695, 3.335708));
    Hatom.push_back(ofVec3f(18.366158, 7.210522, 3.299052));
    Catom.push_back(ofVec3f(20.234804, 6.470222, 4.005516));
    Hatom.push_back(ofVec3f(18.555109, 7.417806, 4.881929));
    Oatom.push_back(ofVec3f(19.369507, 2.430405, 8.905773));
    Oatom.push_back(ofVec3f(-8.778057, 6.131905, 7.756104));
    Catom.push_back(ofVec3f(15.873706, 9.801572, 7.822751));
    Catom.push_back(ofVec3f(16.018358, 9.594288, 10.178741));
    Catom.push_back(ofVec3f(-3.205711, 7.743538, 16.293650));
    Catom.push_back(ofVec3f(-2.450692, 10.119901, 16.405283));
    Hatom.push_back(ofVec3f(-1.621781, 8.402405, 17.328352));
    Hatom.push_back(ofVec3f(-9.991614, 2.228303, 11.779947));
    Hatom.push_back(ofVec3f( -10.043229, 3.671888, 12.912954));
    Catom.push_back(ofVec3f(-9.872185, 1.717496, 13.767709));
    Catom.push_back(ofVec3f(-9.607405, -0.614449, 14.234241));
    Hatom.push_back(ofVec3f(-9.433256, 0.259105, 12.396437));
    Catom.push_back(ofVec3f(20.198856, 3.087051, 2.427635));
    Catom.push_back(ofVec3f(20.463634, 5.418996, 2.894168));
    Hatom.push_back(ofVec3f(20.024708, 3.960605, 4.265440));
    Hatom.push_back(ofVec3f(20.634680, 7.373388, 3.748923));
    Hatom.push_back(ofVec3f(20.583065, 5.929803, 4.881929));
    Catom.push_back(ofVec3f(20.080074, 2.442990, 7.659464));
    Catom.push_back(ofVec3f(-9.488623, 6.144490, 9.002412));
    Hatom.push_back(ofVec3f(15.290402, 9.875602, 7.031312));
    Catom.push_back(ofVec3f(17.248190, 9.868198, 7.732777));
    Hatom.push_back(ofVec3f(15.554695, 9.438825, 10.946853));
    Catom.push_back(ofVec3f(17.378763, 9.668318, 10.107094));
    Hatom.push_back(ofVec3f(-3.548037, 7.773150, 15.178970));
    Hatom.push_back(ofVec3f(-4.014412, 7.773150, 16.945127));
    Hatom.push_back(ofVec3f(-3.007802, 9.623899, 15.528869));
    Hatom.push_back(ofVec3f(-3.231987, 10.216140, 16.961790));
    Hatom.push_back(ofVec3f(-1.718516, 10.660320, 16.461933));
    Catom.push_back(ofVec3f( -11.295400, 1.140062, 13.864347));
    Catom.push_back(ofVec3f(-9.403372, 2.250512, 15.112322));
    Catom.push_back(ofVec3f( -11.089657, -0.281314, 14.430852));
    Hatom.push_back(ofVec3f(-9.512152, -1.391764, 13.796034));
    Hatom.push_back(ofVec3f(-9.168776, -0.555225, 14.979027));
    Hatom.push_back(ofVec3f(19.760227, 3.146275, 1.682850));
    Hatom.push_back(ofVec3f(20.103601, 2.309736, 2.865843));
    Catom.push_back(ofVec3f(21.681107, 3.420186, 2.231025));
    Catom.push_back(ofVec3f(19.994822, 5.952012, 1.549554));
    Catom.push_back(ofVec3f(21.886850, 4.841562, 2.797529));
    Hatom.push_back(ofVec3f(19.773079, 1.673078, 7.131283));
    Catom.push_back(ofVec3f(21.544104, 2.413378, 7.896063));
    Hatom.push_back(ofVec3f(19.854797, 3.360962, 7.164607));
    Hatom.push_back(ofVec3f(-9.181628, 5.374578, 9.530594));
    Catom.push_back(ofVec3f( -10.952652, 6.114878, 8.765814));
    Hatom.push_back(ofVec3f(-9.263346, 7.062462, 9.497270));
    Hatom.push_back(ofVec3f(17.655872, 9.868198, 6.848032));
    Catom.push_back(ofVec3f(18.012444, 9.786766, 8.887445));
    Hatom.push_back(ofVec3f(17.998848, 9.609094, 10.863544));
    Hatom.push_back(ofVec3f( -11.688316, 0.984599, 13.062911));
    Catom.push_back(ofVec3f( -12.374413, 1.924780, 14.652454));
    Hatom.push_back(ofVec3f(-9.488213, 1.495406, 15.762135));
    Hatom.push_back(ofVec3f(-9.886193, 2.976006, 15.445560));
    Hatom.push_back(ofVec3f(-8.421185, 2.257915, 15.178970));
    Hatom.push_back(ofVec3f( -11.727811, -0.866151, 14.095948));
    Hatom.push_back(ofVec3f( -11.292126, -0.392359, 15.328927));
    Hatom.push_back(ofVec3f(21.883575, 3.309141, 1.332950));
    Hatom.push_back(ofVec3f(22.319262, 2.835349, 2.565929));
    Hatom.push_back(ofVec3f(19.012634, 5.959415, 1.482907));
    Hatom.push_back(ofVec3f(20.079664, 5.196906, 0.899742));
    Hatom.push_back(ofVec3f(20.477644, 6.677506, 1.216317));
    Hatom.push_back(ofVec3f(22.279766, 4.686099, 3.598966));
    Catom.push_back(ofVec3f(22.965862, 5.626280, 2.009422));
    Hatom.push_back(ofVec3f(21.705524, 1.710093, 8.314277));
    Catom.push_back(ofVec3f(22.323685, 2.354154, 6.613099));
    Hatom.push_back(ofVec3f(21.841705, 3.168484, 8.480895));
    Hatom.push_back(ofVec3f( -11.114073, 5.411593, 8.347600));
    Hatom.push_back(ofVec3f( -11.250256, 6.869984, 8.180982));
    Catom.push_back(ofVec3f( -11.732234, 6.055654, 10.048778));
    Oatom.push_back(ofVec3f(19.369507, 9.833405, 8.905773));
    Catom.push_back(ofVec3f( -13.738121, 1.214092, 14.530823));
    Catom.push_back(ofVec3f( -12.530684, 3.368365, 14.204250));
    Hatom.push_back(ofVec3f( -12.145725, 2.006213, 15.628840));
    Hatom.push_back(ofVec3f(22.737175, 5.707713, 1.033036));
    Catom.push_back(ofVec3f(24.329573, 4.915592, 2.131054));
    Catom.push_back(ofVec3f(23.122135, 7.069865, 2.457626));
    Hatom.push_back(ofVec3f(22.049370, 1.613854, 6.164895));
    Hatom.push_back(ofVec3f(22.137161, 3.168484, 6.064923));
    Catom.push_back(ofVec3f(23.818903, 2.272721, 6.763056));
    Catom.push_back(ofVec3f( -13.227452, 5.974221, 9.898821));
    Hatom.push_back(ofVec3f( -11.545710, 6.869984, 10.596954));
    Hatom.push_back(ofVec3f( -11.457918, 5.315354, 10.496983));
    Catom.push_back(ofVec3f(20.080074, 9.845990, 7.659464));
    Hatom.push_back(ofVec3f( -14.081089, 1.302928, 13.679400));
    Catom.push_back(ofVec3f( -14.779783, 1.650869, 15.520538));
    Hatom.push_back(ofVec3f( -13.516752, 0.103642, 14.829070));
    Hatom.push_back(ofVec3f( -12.781000, 3.190693, 13.129559));
    Hatom.push_back(ofVec3f( -13.348725, 3.768127, 14.545818));
    Hatom.push_back(ofVec3f( -11.777779, 3.723709, 14.279228));
    Catom.push_back(ofVec3f(25.371233, 5.352369, 1.141338));
    Hatom.push_back(ofVec3f(24.108204, 3.805142, 1.832807));
    Hatom.push_back(ofVec3f(24.672539, 5.004428, 2.982476));
    Hatom.push_back(ofVec3f(22.369228, 7.425209, 2.382648));
    Hatom.push_back(ofVec3f(23.372452, 6.892193, 3.532318));
    Hatom.push_back(ofVec3f(23.940176, 7.469627, 2.116058));
    Hatom.push_back(ofVec3f(24.175518, 1.495406, 7.414535));
    Catom.push_back(ofVec3f(24.536139, 2.191288, 5.436770));
    Hatom.push_back(ofVec3f(24.021191, 3.109260, 7.147945));
    Hatom.push_back(ofVec3f( -13.584068, 5.196906, 9.247342));
    Hatom.push_back(ofVec3f( -13.429740, 6.810760, 9.513931));
    Catom.push_back(ofVec3f( -13.944688, 5.892788, 11.225106));
    Hatom.push_back(ofVec3f(19.773079, 9.076077, 7.131283));
    Catom.push_back(ofVec3f(21.544104, 9.816378, 7.896063));
    Hatom.push_back(ofVec3f(19.854797, 10.763962, 7.164607));
    Catom.push_back(ofVec3f( -16.087040, 0.917972, 15.397240));
    Hatom.push_back(ofVec3f( -14.426534, 1.450988, 16.445272));
    Hatom.push_back(ofVec3f( -14.933393, 2.628065, 15.528869));
    Hatom.push_back(ofVec3f(25.017984, 5.152488, 0.216605));
    Hatom.push_back(ofVec3f(25.524845, 6.329565, 1.133008));
    Catom.push_back(ofVec3f(26.678490, 4.619472, 1.264637));
    Hatom.push_back(ofVec3f(24.246998, 1.391764, 4.931916));
    Hatom.push_back(ofVec3f(24.281811, 2.976006, 4.915254));
    Catom.push_back(ofVec3f(26.028040, 2.028422, 5.493421));
    Catom.push_back(ofVec3f( -15.436589, 5.729922, 11.168456));
    Hatom.push_back(ofVec3f( -13.655548, 5.093264, 11.729961));
    Hatom.push_back(ofVec3f( -13.690359, 6.677506, 11.746624));
    Hatom.push_back(ofVec3f(21.705524, 9.113092, 8.314277));
    Catom.push_back(ofVec3f(22.323685, 9.757154, 6.613099));
    Hatom.push_back(ofVec3f(21.841705, 10.571484, 8.480895));
    Hatom.push_back(ofVec3f( -15.845952, -0.088836, 15.378912));
    Hatom.push_back(ofVec3f( -16.446651, 1.332540, 14.529156));
    Catom.push_back(ofVec3f( -17.137789, 1.273316, 16.436941));
    Hatom.push_back(ofVec3f(26.437403, 3.612664, 1.282965));
    Catom.push_back(ofVec3f(27.729240, 4.974816, 0.224935));
    Hatom.push_back(ofVec3f(27.038103, 5.034040, 2.132720));
    Hatom.push_back(ofVec3f(26.203686, 1.391764, 5.914966));
    Catom.push_back(ofVec3f(26.671417, 1.932183, 4.193795));
    Hatom.push_back(ofVec3f(26.439758, 2.753916, 6.048261));
    Hatom.push_back(ofVec3f( -15.612236, 5.093264, 10.746910));
    Catom.push_back(ofVec3f( -16.079967, 5.633683, 12.468081));
    Hatom.push_back(ofVec3f( -15.848308, 6.455416, 10.613615));
    Hatom.push_back(ofVec3f(22.049370, 9.016853, 6.164895));
    Hatom.push_back(ofVec3f(22.137161, 10.571484, 6.064923));
    Catom.push_back(ofVec3f(23.818903, 9.675721, 6.763056));
    Catom.push_back(ofVec3f( -18.339710, 0.340538, 16.293650));
    Catom.push_back(ofVec3f( -17.584692, 2.716901, 16.405283));
    Hatom.push_back(ofVec3f( -16.755779, 0.999405, 17.328352));
    Hatom.push_back(ofVec3f(27.347231, 4.700905, -0.666474));
    Catom.push_back(ofVec3f(28.931160, 4.042038, 0.368227));
    Catom.push_back(ofVec3f(28.176144, 6.418401, 0.256593));
    Hatom.push_back(ofVec3f(26.750347, 2.813140, 3.632289));
    Catom.push_back(ofVec3f(27.160339, 1.021614, 3.543981));
    Catom.push_back(ofVec3f( -16.568888, 4.723114, 13.117895));
    Hatom.push_back(ofVec3f( -16.158895, 6.514640, 13.029588));
    Hatom.push_back(ofVec3f(24.175518, 8.898406, 7.414535));
    Catom.push_back(ofVec3f(24.536139, 9.594288, 5.436770));
    Hatom.push_back(ofVec3f(24.021191, 10.512259, 7.147945));
    Hatom.push_back(ofVec3f( -18.094988, -0.740300, 16.245331));
    Hatom.push_back(ofVec3f( -18.682037, 0.370150, 15.178970));
    Hatom.push_back(ofVec3f( -19.148411, 0.370150, 16.945127));
    Hatom.push_back(ofVec3f( -18.141802, 2.220900, 15.528869));
    Hatom.push_back(ofVec3f( -18.365986, 2.813140, 16.961790));
    Hatom.push_back(ofVec3f( -16.852516, 3.257320, 16.461933));
    Hatom.push_back(ofVec3f(28.686440, 2.961200, 0.416547));
    Hatom.push_back(ofVec3f(29.739861, 4.071650, -0.283251));
    Hatom.push_back(ofVec3f(29.273487, 4.071650, 1.482907));
    Hatom.push_back(ofVec3f(27.443966, 6.958820, 0.199943));
    Hatom.push_back(ofVec3f(28.957436, 6.514640, -0.299913));
    Hatom.push_back(ofVec3f(28.733253, 5.922400, 1.133008));
    Hatom.push_back(ofVec3f(27.555626, 0.592240, 2.565929));
    Hatom.push_back(ofVec3f( -16.964174, 4.293740, 14.095948));
    Hatom.push_back(ofVec3f(24.246998, 8.794764, 4.931916));
    Hatom.push_back(ofVec3f(24.281811, 10.379005, 4.915254));
    Catom.push_back(ofVec3f(26.028040, 9.431421, 5.493421));
    Hatom.push_back(ofVec3f(26.203686, 8.794764, 5.914966));
    Catom.push_back(ofVec3f(26.671417, 9.335183, 4.193795));
    Hatom.push_back(ofVec3f(26.439758, 10.156916, 6.048261));
    Hatom.push_back(ofVec3f(26.750347, 10.216140, 3.632289));
    Catom.push_back(ofVec3f(27.160339, 8.424614, 3.543981));
    Hatom.push_back(ofVec3f(27.555626, 7.995240, 2.565929));
     */
    /*
    // SINGLE Cholesteryl benzoate
    Catom.push_back(ofVec3f(9.678114,7.025447,12.368111));
    Catom.push_back(ofVec3f(8.273008,6.795954,12.831312));
    Catom.push_back(ofVec3f(5.526594,6.788551,14.234242));
    Catom.push_back(ofVec3f(4.044343,7.121686,14.430852));
    Hatom.push_back(ofVec3f(11.972386,7.277149,11.596666));
    Hatom.push_back(ofVec3f(10.163767,6.225923,12.346451));
    Hatom.push_back(ofVec3f( 8.237042,6.196311,13.529444));
    Hatom.push_back(ofVec3f( 7.627328,6.351774,12.213156));
    Hatom.push_back(ofVec3f( 5.965224,6.847775,14.979028));
    Hatom.push_back(ofVec3f( 5.621849,6.011236,13.796035));
    Hatom.push_back(ofVec3f( 3.841875,7.010641,15.328928));
    Hatom.push_back(ofVec3f( 3.406189,6.536849,14.095949));
    Hatom.push_back(ofVec3f(-0.711953,7.314164,15.378913));
    Hatom.push_back(ofVec3f(-2.960988,6.662700,16.245331));
    Catom.push_back(ofVec3f(10.172132,8.202524,12.054869));
    Catom.push_back(ofVec3f(7.548035,8.084076,13.214535));
    Catom.push_back(ofVec3f(6.057169,7.884195,13.326169));
    Catom.push_back(ofVec3f(3.838600,8.543062,13.864348));
    Catom.push_back(ofVec3f(9.378804,9.498049,12.078195));
    Catom.push_back(ofVec3f(11.637461,8.335778,11.664981));
    Catom.push_back(ofVec3f(7.860090,9.190084,12.203159));
    Hatom.push_back(ofVec3f( 7.970608,8.298763,14.062624));
    Hatom.push_back(ofVec3f( 5.700743,7.662105,12.396437));
    Catom.push_back(ofVec3f(5.261815,9.120496,13.767710));
    Hatom.push_back(ofVec3f( 3.445683,8.387599,13.062912));
    Catom.push_back(ofVec3f(2.759587,9.327780,14.652455));
    Catom.push_back(ofVec3f(9.637327,10.238349,10.763573));
    Catom.push_back(ofVec3f(9.866404,10.356797,13.276184));
    Catom.push_back(ofVec3f(11.828935,9.135302,10.385349));
    Hatom.push_back(ofVec3f(12.208363,8.979839,12.396437));
    Hatom.push_back(ofVec3f( 7.474622,8.809570,11.163458));
    Catom.push_back(ofVec3f(7.017545,10.445633,12.451422));
    Catom.push_back(ofVec3f(5.490647,10.171722,12.656362));
    Catom.push_back(ofVec3f(5.730628,9.653512,15.112323));
    Catom.push_back(ofVec3f(1.395878,8.617092,14.530823));
    Catom.push_back(ofVec3f(2.603315,10.771365,14.204251));
    Hatom.push_back(ofVec3f( 2.988274,9.409213,15.628842));
    Hatom.push_back(ofVec3f( 9.248477,9.535064,10.097098));
    Hatom.push_back(ofVec3f( 9.237768,11.052679,10.913529));
    Catom.push_back(ofVec3f(11.114753,10.453036,10.440333));
    Hatom.push_back(ofVec3f( 9.494600,9.949632,14.079287));
    Hatom.push_back(ofVec3f(10.793205,10.290170,13.312841));
    Hatom.push_back(ofVec3f( 9.562832,11.289575,13.162884));
    Hatom.push_back(ofVec3f(11.539830,8.506047,9.630565));
    Oatom.push_back(ofVec3f(13.262445,9.319636,10.262050));
    Hatom.push_back(ofVec3f( 7.170341,11.119306,11.779948));
    Hatom.push_back(ofVec3f( 7.359293,10.912022,13.362826));
    Hatom.push_back(ofVec3f( 5.142385,9.631303,11.779948));
    Hatom.push_back(ofVec3f( 5.090770,11.074888,12.912955));
    Hatom.push_back(ofVec3f( 5.645787,8.898406,15.762136));
    Hatom.push_back(ofVec3f( 5.247807,10.379006,15.445560));
    Hatom.push_back(ofVec3f( 6.712816,9.660915,15.178971));
    Hatom.push_back(ofVec3f( 1.052911,8.705928,13.679401));
    Catom.push_back(ofVec3f(0.354217,9.053869,15.520539));
    Hatom.push_back(ofVec3f( 1.617247,7.506642,14.829071));
    Hatom.push_back(ofVec3f( 2.353000,10.593693,13.129559));
    Hatom.push_back(ofVec3f( 1.785274,11.171127,14.545820));
    Hatom.push_back(ofVec3f( 3.356221,11.126709,14.279229));
    Hatom.push_back(ofVec3f(11.284059,10.971246,9.680550));
    Hatom.push_back(ofVec3f(11.535076,11.067485,11.146797));
    Catom.push_back(ofVec3f(13.764309,9.527661,9.037402));
    Catom.push_back(ofVec3f(-0.953040,8.320972,15.397241));
    Hatom.push_back(ofVec3f( 0.707466,8.853988,16.445273));
    Hatom.push_back(ofVec3f( 0.200606,10.031065,15.528870));
    Oatom.push_back(ofVec3f(13.068284,9.646109,8.054351));
    Catom.push_back(ofVec3f(15.238514,9.660915,9.025739));
    Hatom.push_back(ofVec3f(-1.312653,8.735540,14.529157));
    Catom.push_back(ofVec3f(-2.003789,8.676316,16.436943));
    Catom.push_back(ofVec3f(15.873706,9.801572,7.822752));
    Catom.push_back(ofVec3f(16.018358,9.594288,10.178741));
    Catom.push_back(ofVec3f(-3.205711,7.743538,16.293651));
    Catom.push_back(ofVec3f(-2.450692,10.119901,16.405285));
    Hatom.push_back(ofVec3f(-1.621781,8.402405,17.328352));
    Hatom.push_back(ofVec3f(15.290402,9.875602,7.031312));
    Catom.push_back(ofVec3f(17.248189,9.868199,7.732777));
    Hatom.push_back(ofVec3f(15.554695,9.438825,10.946854));
    Catom.push_back(ofVec3f(17.378763,9.668318,10.107095));
    Hatom.push_back(ofVec3f(-3.548037,7.773150,15.178971));
    Hatom.push_back(ofVec3f(-4.014412,7.773150,16.945129));
    Hatom.push_back(ofVec3f(-3.007802,9.623900,15.528870));
    Hatom.push_back(ofVec3f(-3.231987,10.216140,16.961791));
    Hatom.push_back(ofVec3f(-1.718516,10.660320,16.461935));
    Hatom.push_back(ofVec3f(17.655872,9.868199,6.848032));
    Catom.push_back(ofVec3f(18.012444,9.786766,8.887445));
    Hatom.push_back(ofVec3f(17.998847,9.609094,10.863544));
    Oatom.push_back(ofVec3f(19.369507,9.833405,8.905774));
    Catom.push_back(ofVec3f(20.080073,9.845990,7.659465));
    Hatom.push_back(ofVec3f(19.773079,9.076078,7.131284));
    Catom.push_back(ofVec3f(21.544103,9.816378,7.896064));
    Hatom.push_back(ofVec3f(19.854798,10.763962,7.164607));
    Hatom.push_back(ofVec3f(21.705523,9.113093,8.314277));
    Catom.push_back(ofVec3f(22.323685,9.757154,6.613099));
    Hatom.push_back(ofVec3f(21.841706,10.571484,8.480896));
    Hatom.push_back(ofVec3f(22.049369,9.016854,6.164895));
    Hatom.push_back(ofVec3f(22.137160,10.571484,6.064923));
    Catom.push_back(ofVec3f(23.818903,9.675721,6.763056));
    Hatom.push_back(ofVec3f(24.175519,8.898406,7.414535));
    Catom.push_back(ofVec3f(24.536139,9.594288,5.436771));
    Hatom.push_back(ofVec3f(24.021190,10.512260,7.147945));
    Hatom.push_back(ofVec3f(24.246999,8.794764,4.931916));
    Hatom.push_back(ofVec3f(24.281810,10.379006,4.915254));
    Catom.push_back(ofVec3f(26.028040,9.431422,5.493421));
    Hatom.push_back(ofVec3f(26.203687,8.794764,5.914966));
    Catom.push_back(ofVec3f(26.671418,9.335183,4.193795));
    Hatom.push_back(ofVec3f(26.439758,10.156916,6.048262));
    Hatom.push_back(ofVec3f(26.750346,10.216140,3.632289));
    Catom.push_back(ofVec3f(27.160339,8.424614,3.543981));
    Hatom.push_back(ofVec3f(27.555625,7.995240,2.565929));
    */
    
    /*
    // SINGLE Cheda Poli-B
    Hatom.push_back(ofVec3f(3.215478,4.949511,10.419262));
    Hatom.push_back(ofVec3f(1.834189,5.922234,12.000610));
    Catom.push_back(ofVec3f(3.612120,6.075868,12.855555));
    Hatom.push_back(ofVec3f(3.275187,6.619269,13.572990));
    Hatom.push_back(ofVec3f(3.990538,6.639419,12.176980));
    Catom.push_back(ofVec3f(2.524263,5.303133,12.287584));
    Hatom.push_back(ofVec3f(4.283981,5.480591,13.191853));
    Hatom.push_back(ofVec3f(2.144341,4.756067,12.993063));
    Catom.push_back(ofVec3f(2.848936,4.409296,11.136698));
    Catom.push_back(ofVec3f(1.686591,3.635046,10.606093));
    Hatom.push_back(ofVec3f(3.537433,3.786125,11.414704));
    Catom.push_back(ofVec3f(1.984309,2.717226,9.446239));
    Hatom.push_back(ofVec3f(1.001406,4.263906,10.328087));
    Hatom.push_back(ofVec3f(1.318082,3.103979,11.328013));
    Catom.push_back(ofVec3f(0.798208,1.978237,8.896205));
    Hatom.push_back(ofVec3f(2.646020,2.066906,9.730225));
    Hatom.push_back(ofVec3f(2.378039,3.241734,8.731792));
    Hatom.push_back(ofVec3f(0.136674,2.629913,8.615210));
    Catom.push_back(ofVec3f(1.083423,1.056215,7.733361));
    Hatom.push_back(ofVec3f(0.405264,1.455831,9.612146));
    Catom.push_back(ofVec3f(-0.133893,0.384770,7.150444));
    Hatom.push_back(ofVec3f(1.703845,0.370828,8.027808));
    Hatom.push_back(ofVec3f(1.520134,1.566178,7.033861));
    Hatom.push_back(ofVec3f(-0.561809,-0.142130,7.843966));
    Hatom.push_back(ofVec3f(-0.762383,1.067734,6.869448));
    Catom.push_back(ofVec3f(0.170959,-0.521093,5.963686));
    Hatom.push_back(ofVec3f(-0.659125,-0.782747,5.534719));
    Catom.push_back(ofVec3f(0.912019,-1.744207,6.398631));
    Hatom.push_back(ofVec3f(0.703713,-0.034978,5.315004));
    Oatom.push_back(ofVec3f(0.240891,-2.701794,6.901584));
    Oatom.push_back(ofVec3f(2.164382,-1.742887,6.304468));
    */
     
     
     // FULL Cheda Poli-B
     CUatom.push_back(ofVec3f(2.824582,9.931890,7.823190));
     CUatom.push_back(ofVec3f(5.045933,11.076813,7.123391));
     Oatom.push_back(ofVec3f(2.972481,10.989587,9.437271));
     Oatom.push_back(ofVec3f(4.898033,10.019116,5.509310));
     Oatom.push_back(ofVec3f(3.931325,8.530914,8.642113));
     Oatom.push_back(ofVec3f(3.939190,12.477789,6.304468));
     Oatom.push_back(ofVec3f(3.000133,9.014656,6.125109));
     Oatom.push_back(ofVec3f(4.870381,11.994047,8.821472));
     Oatom.push_back(ofVec3f(2.015698,11.518883,6.901584));
     Oatom.push_back(ofVec3f(5.854816,9.489821,8.044997));
     Catom.push_back(ofVec3f(3.918508,11.802794,9.612146));
     Catom.push_back(ofVec3f(3.952006,9.205909,5.334434));
     Catom.push_back(ofVec3f(5.183688,8.532234,8.547950));
     Catom.push_back(ofVec3f(2.686826,12.476469,6.398631));
     Catom.push_back(ofVec3f(3.851449,12.636135,10.864669));
     Catom.push_back(ofVec3f(4.019066,8.372568,4.081911));
     Hatom.push_back(ofVec3f(3.117853,13.264587,10.770506));
     Hatom.push_back(ofVec3f(4.752661,7.744117,4.176074));
     Hatom.push_back(ofVec3f(3.636901,12.049899,11.606020));
     Hatom.push_back(ofVec3f(4.233614,8.958804,3.340561));
     Catom.push_back(ofVec3f(5.082576,13.407078,11.218903));
     Catom.push_back(ofVec3f(2.787938,7.601625,3.727677));
     Hatom.push_back(ofVec3f(5.296046,14.009442,10.488015));
     Hatom.push_back(ofVec3f(2.574468,6.999261,4.458565));
     Hatom.push_back(ofVec3f(5.822530,12.787838,11.314562));
     Hatom.push_back(ofVec3f(2.047985,8.220865,3.632019));
     Catom.push_back(ofVec3f(4.956979,14.208876,12.483384));
     Catom.push_back(ofVec3f(2.913535,6.799827,2.463196));
     Hatom.push_back(ofVec3f(4.222326,14.833383,12.374274));
     Hatom.push_back(ofVec3f(3.648189,6.175320,2.572307));
     Hatom.push_back(ofVec3f(4.716092,13.603927,13.202315));
     Hatom.push_back(ofVec3f(3.154423,7.404777,1.744266));
     Catom.push_back(ofVec3f(6.155704,14.980987,12.915340));
     Catom.push_back(ofVec3f(1.714810,6.027716,2.031241));
     Hatom.push_back(ofVec3f(6.894351,14.359582,13.012493));
     Hatom.push_back(ofVec3f(0.976163,6.649121,1.934088));
     Hatom.push_back(ofVec3f(6.389168,15.597185,12.202388));
     Hatom.push_back(ofVec3f(1.481347,5.411518,2.744192));
     Catom.push_back(ofVec3f(6.038092,15.755217,14.172348));
     Catom.push_back(ofVec3f(1.832422,5.253487,0.774233));
     Hatom.push_back(ofVec3f(5.319185,16.397596,14.058753));
     Hatom.push_back(ofVec3f(2.551330,4.611107,0.887827));
     Hatom.push_back(ofVec3f(5.766652,15.143543,14.873343));
     Hatom.push_back(ofVec3f(2.103863,5.865160,0.073238));
     Catom.push_back(ofVec3f(7.225542,16.490066,14.650638));
     Catom.push_back(ofVec3f(0.644973,4.518638,0.295942));
     Hatom.push_back(ofVec3f(2.268191,17.140299,13.975053));
     Hatom.push_back(ofVec3f(5.602323,3.868405,0.971528));
     Hatom.push_back(ofVec3f(2.756064,15.861258,14.728361));
     Hatom.push_back(ofVec3f(5.114450,5.147445,0.218220));
     Catom.push_back(ofVec3f(6.192598,10.418968,1.013379));
     Catom.push_back(ofVec3f(1.677917,10.589736,13.933202));
     Hatom.push_back(ofVec3f(5.499011,11.088724,0.916226));
     Hatom.push_back(ofVec3f(2.371503,9.919979,14.030355));
     Hatom.push_back(ofVec3f(5.885854,9.779690,1.675512));
     Hatom.push_back(ofVec3f(1.984661,11.229013,13.271069));
     Catom.push_back(ofVec3f(2.204812,11.083039,1.542487));
     Catom.push_back(ofVec3f(5.665703,9.925664,13.404094));
     Hatom.push_back(ofVec3f(2.476000,11.778297,0.922204));
     Hatom.push_back(ofVec3f(5.394514,9.230406,14.024377));
     Hatom.push_back(ofVec3f(2.922976,10.432188,1.587327));
     Hatom.push_back(ofVec3f(4.947539,10.576515,13.359254));
     Catom.push_back(ofVec3f(2.016125,11.701786,2.920562));
     Catom.push_back(ofVec3f(5.854390,9.306917,12.026019));
     Hatom.push_back(ofVec3f(2.834360,12.118145,3.198568));
     Hatom.push_back(ofVec3f(5.036154,8.890558,11.748013));
     Hatom.push_back(ofVec3f(1.776273,11.015611,3.548319));
     Hatom.push_back(ofVec3f(6.094242,9.993092,11.398262));
     Hatom.push_back(ofVec3f(6.521515,12.358876,2.883195));
     Hatom.push_back(ofVec3f(1.348999,8.649828,12.063386));
     Catom.push_back(ofVec3f(5.924749,7.309120,8.982895));
     Catom.push_back(ofVec3f(1.945766,13.699583,5.963685));
     Hatom.push_back(ofVec3f(5.391994,6.823005,9.631577));
     Hatom.push_back(ofVec3f(2.478520,14.185698,5.315004));
     Hatom.push_back(ofVec3f(1.551332,7.570774,9.411862));
     Hatom.push_back(ofVec3f(6.319182,13.437930,5.534719));
     Catom.push_back(ofVec3f(1.026101,6.403257,7.796137));
     Catom.push_back(ofVec3f(6.844413,14.605446,7.150444));
     Hatom.push_back(ofVec3f(1.454016,6.930157,7.102615));
     Hatom.push_back(ofVec3f(6.416498,14.078546,7.843966));
     Hatom.push_back(ofVec3f(1.654590,5.720294,8.077133));
     Hatom.push_back(ofVec3f(6.215924,15.288410,6.869448));
     Catom.push_back(ofVec3f(5.012284,5.731813,7.213220));
     Catom.push_back(ofVec3f(2.858230,15.276891,7.733361));
     Hatom.push_back(ofVec3f(4.391862,6.417199,6.918772));
     Hatom.push_back(ofVec3f(3.478652,14.591504,8.027808));
     Hatom.push_back(ofVec3f(4.575573,5.221849,7.912720));
     Hatom.push_back(ofVec3f(3.294941,15.786854,7.033861));
     Catom.push_back(ofVec3f(5.297500,4.809791,6.050376));
     Catom.push_back(ofVec3f(2.573015,16.198912,8.896205));
     Hatom.push_back(ofVec3f(0.755533,4.158113,6.331371));
     Hatom.push_back(ofVec3f(7.114981,16.850590,8.615209));
     Hatom.push_back(ofVec3f(5.690443,5.332197,5.334435));
     Hatom.push_back(ofVec3f(2.180071,15.676506,9.612146));
     Catom.push_back(ofVec3f(4.111398,4.070801,5.500342));
     Catom.push_back(ofVec3f(3.759116,16.937902,9.446239));
     Hatom.push_back(ofVec3f(3.449687,4.721121,5.216357));
     Hatom.push_back(ofVec3f(4.420828,16.287582,9.730224));
     Hatom.push_back(ofVec3f(3.717668,3.546294,6.214788));
     Hatom.push_back(ofVec3f(4.152847,17.462409,8.731792));
     Catom.push_back(ofVec3f(4.409116,3.152981,4.340487));
     Catom.push_back(ofVec3f(3.461399,17.855722,10.606094));
     Hatom.push_back(ofVec3f(4.777625,3.684048,3.618567));
     Hatom.push_back(ofVec3f(3.092889,17.324655,11.328013));
     Hatom.push_back(ofVec3f(5.094301,2.524121,4.618493));
     Hatom.push_back(ofVec3f(2.776213,18.484583,10.328087));
     Catom.push_back(ofVec3f(3.246771,2.378731,3.809884));
     Catom.push_back(ofVec3f(4.623743,18.629972,11.136697));
     Hatom.push_back(ofVec3f(4.655036,16.059193,4.527319));
     Hatom.push_back(ofVec3f(3.215478,4.949510,10.419262));
     Hatom.push_back(ofVec3f(2.558274,3.001903,3.531877));
     Hatom.push_back(ofVec3f(5.312241,18.006800,11.414703));
     Catom.push_back(ofVec3f(3.571445,1.484893,2.658997));
     Catom.push_back(ofVec3f(4.299070,19.523810,12.287584));
     Hatom.push_back(ofVec3f(3.951367,2.031960,1.953518));
     Hatom.push_back(ofVec3f(3.919148,18.976744,12.993062));
     Hatom.push_back(ofVec3f(6.036325,15.086470,2.945971));
     Hatom.push_back(ofVec3f(1.834189,5.922233,12.000610));
     Catom.push_back(ofVec3f(4.258394,14.932836,2.091027));
     Catom.push_back(ofVec3f(3.612120,6.075867,12.855554));
     Hatom.push_back(ofVec3f(4.595327,14.389434,1.373591));
     Hatom.push_back(ofVec3f(3.275187,6.619269,13.572990));
     Hatom.push_back(ofVec3f(1.811726,1.307437,1.754729));
     Hatom.push_back(ofVec3f(6.058788,19.701267,13.191852));
     Hatom.push_back(ofVec3f(3.879976,14.369285,2.769601));
     Hatom.push_back(ofVec3f(3.990538,6.639419,12.176979));
     Oatom.push_back(ofVec3f( 0.651317,9.489821,8.044997));
     Oatom.push_back(ofVec3f( 7.219198,11.518883,6.901584));
     Catom.push_back(ofVec3f(7.084805,17.206995,15.959959));
     Hatom.push_back(ofVec3f(7.959564,15.861258,14.728361));
     Hatom.push_back(ofVec3f(7.471691,17.140299,13.975053));
     Hatom.push_back(ofVec3f(0.398823,3.868405,0.971528));
     Hatom.push_back(ofVec3f(-0.089050,5.147445,0.218220));
     Catom.push_back(ofVec3f(0.785710,3.801708,-1.013379));
     Catom.push_back(ofVec3f(6.333334,9.702038,-0.295942));
     Catom.push_back(ofVec3f(7.408311,11.083039,1.542487));
     Catom.push_back(ofVec3f(0.462203,9.925664,13.404094));
     Catom.push_back(ofVec3f(1.537180,11.306665,15.242523));
     Catom.push_back(ofVec3f(0.989098,10.418968,1.013379));
     Catom.push_back(ofVec3f(6.881417,10.589736,13.933202));
     Hatom.push_back(ofVec3f(1.318015,12.358876,2.883195));
     Hatom.push_back(ofVec3f(6.552499,8.649828,12.063386));
     Catom.push_back(ofVec3f(6.229601,6.403257,7.796137));
     Hatom.push_back(ofVec3f(6.754832,7.570774,9.411862));
     Hatom.push_back(ofVec3f(1.115683,13.437930,5.534719));
     Catom.push_back(ofVec3f(1.640913,14.605446,7.150444));
     Catom.push_back(ofVec3f(-0.191216,5.731813,7.213220));
     Catom.push_back(ofVec3f(0.721249,7.309120,8.982895));
     Catom.push_back(ofVec3f(7.149266,13.699583,5.963685));
     Catom.push_back(ofVec3f(8.061730,15.276891,7.733361));
     Hatom.push_back(ofVec3f(5.959033,4.158113,6.331371));
     Hatom.push_back(ofVec3f(1.911482,16.850590,8.615209));
     Hatom.push_back(ofVec3f(2.880229,1.838517,4.527319));
     Hatom.push_back(ofVec3f(4.990286,19.170186,10.419262));
     Catom.push_back(ofVec3f(2.483587,0.712160,2.091027));
     Hatom.push_back(ofVec3f(4.261518,0.865794,2.945971));
     Hatom.push_back(ofVec3f(3.608996,20.142910,12.000610));
     Catom.push_back(ofVec3f(5.386927,20.296544,12.855554));
     Hatom.push_back(ofVec3f(3.586533,15.528113,1.754729));
     Catom.push_back(ofVec3f(5.346252,15.705569,2.658997));
     Catom.push_back(ofVec3f(2.524263,5.303134,12.287584));
     Hatom.push_back(ofVec3f(4.283981,5.480590,13.191852));
     Catom.push_back(ofVec3f(-0.019812,8.532234,8.547950));
     Catom.push_back(ofVec3f(7.890326,12.476469,6.398631));
     Hatom.push_back(ofVec3f(6.778061,16.567717,16.622092));
     Hatom.push_back(ofVec3f(6.391219,17.876751,15.862807));
     Catom.push_back(ofVec3f(8.300519,17.871067,16.489067));
     Catom.push_back(ofVec3f(-0.430004,3.137637,-1.542487));
     Hatom.push_back(ofVec3f(1.479296,3.131952,-0.916226));
     Hatom.push_back(ofVec3f(1.092453,4.440986,-1.675512));
     Catom.push_back(ofVec3f(5.145885,8.967189,-0.774233));
     Hatom.push_back(ofVec3f(7.067357,9.073231,-0.218220));
     Hatom.push_back(ofVec3f(6.579484,10.352271,-0.971528));
     Hatom.push_back(ofVec3f(7.679500,11.778297,0.922204));
     Hatom.push_back(ofVec3f(8.126475,10.432188,1.587327));
     Catom.push_back(ofVec3f(7.219625,11.701786,2.920562));
     Catom.push_back(ofVec3f(0.650890,9.306917,12.026019));
     Hatom.push_back(ofVec3f(0.191014,9.230406,14.024377));
     Hatom.push_back(ofVec3f(-0.255961,10.576515,13.359254));
     Hatom.push_back(ofVec3f(1.291031,10.656432,15.918108));
     Hatom.push_back(ofVec3f(0.803158,11.935472,15.164800));
     Catom.push_back(ofVec3f(2.724630,12.041514,15.720814));
     Hatom.push_back(ofVec3f(0.682354,9.779690,1.675512));
     Catom.push_back(ofVec3f(1.129835,9.702038,-0.295942));
     Hatom.push_back(ofVec3f(0.295511,11.088724,0.916226));
     Hatom.push_back(ofVec3f(7.575003,9.919979,14.030355));
     Catom.push_back(ofVec3f(6.740680,11.306665,15.242523));
     Hatom.push_back(ofVec3f(7.188160,11.229013,13.271069));
     Hatom.push_back(ofVec3f(6.858090,5.720294,8.077133));
     Hatom.push_back(ofVec3f(6.657516,6.930157,7.102615));
     Hatom.push_back(ofVec3f(1.212999,14.078546,7.843966));
     Hatom.push_back(ofVec3f(1.012425,15.288410,6.869448));
     Catom.push_back(ofVec3f(0.094000,4.809791,6.050376));
     Hatom.push_back(ofVec3f(-0.627927,5.221849,7.912720));
     Hatom.push_back(ofVec3f(-0.811638,6.417199,6.918772));
     Hatom.push_back(ofVec3f(0.188495,6.823005,9.631577));
     Hatom.push_back(ofVec3f(7.682020,14.185698,5.315004));
     Hatom.push_back(ofVec3f(8.682152,14.591504,8.027808));
     Hatom.push_back(ofVec3f(8.498441,15.786854,7.033861));
     Catom.push_back(ofVec3f(7.776514,16.198912,8.896205));
     Hatom.push_back(ofVec3f(2.820520,0.168758,1.373591));
     Hatom.push_back(ofVec3f(2.105169,0.148609,2.769601));
     Hatom.push_back(ofVec3f(5.765346,20.860095,12.176979));
     Hatom.push_back(ofVec3f(5.049994,20.839945,13.572990));
     Catom.push_back(ofVec3f(5.021578,16.599407,3.809884));
     Hatom.push_back(ofVec3f(5.726174,16.252636,1.953518));
     Hatom.push_back(ofVec3f(2.144340,4.756068,12.993062));
     Catom.push_back(ofVec3f(2.848936,4.409296,11.136697));
     Oatom.push_back(ofVec3f(-1.272175,8.530914,8.642113));
     Oatom.push_back(ofVec3f( 9.142689,12.477789,6.304468));
     Hatom.push_back(ofVec3f(8.571707,18.566324,15.868784));
     Hatom.push_back(ofVec3f(9.018683,17.220215,16.533907));
     Catom.push_back(ofVec3f(8.111832,18.489813,17.867142));
     Catom.push_back(ofVec3f(-0.241317,2.518890,-2.920562));
     Hatom.push_back(ofVec3f(-0.701193,2.442379,-0.922204));
     Hatom.push_back(ofVec3f(-1.148168,3.788488,-1.587327));
     Hatom.push_back(ofVec3f(4.426977,9.609569,-0.887827));
     Catom.push_back(ofVec3f(5.263497,8.192960,-2.031241));
     Hatom.push_back(ofVec3f(4.874444,8.355516,-0.073238));
     Hatom.push_back(ofVec3f(6.979772,11.015611,3.548319));
     Hatom.push_back(ofVec3f(8.037860,12.118145,3.198568));
     Hatom.push_back(ofVec3f(-0.167346,8.890558,11.748013));
     Hatom.push_back(ofVec3f(0.890742,9.993092,11.398262));
     Catom.push_back(ofVec3f(2.607017,12.815743,16.977821));
     Hatom.push_back(ofVec3f(2.996070,12.653187,15.019819));
     Hatom.push_back(ofVec3f(3.443537,11.399134,15.834408));
     Catom.push_back(ofVec3f(-0.057615,8.967189,-0.774233));
     Hatom.push_back(ofVec3f(1.863857,9.073231,-0.218220));
     Hatom.push_back(ofVec3f(1.375984,10.352271,-0.971528));
     Hatom.push_back(ofVec3f(6.494531,10.656432,15.918108));
     Hatom.push_back(ofVec3f(6.006658,11.935472,15.164800));
     Catom.push_back(ofVec3f(7.928130,12.041514,15.720814));
     Catom.push_back(ofVec3f(-1.092101,4.070801,5.500342));
     Hatom.push_back(ofVec3f(0.486943,5.332197,5.334435));
     Hatom.push_back(ofVec3f(7.383571,15.676506,9.612146));
     Catom.push_back(ofVec3f(8.962616,16.937902,9.446239));
     Hatom.push_back(ofVec3f(4.333081,17.222579,3.531877));
     Catom.push_back(ofVec3f(6.183923,17.373657,4.340487));
     Catom.push_back(ofVec3f(1.686591,3.635046,10.606094));
     Hatom.push_back(ofVec3f(3.537433,3.786124,11.414703));
     Hatom.push_back(ofVec3f(7.413722,19.146903,17.829775));
     Hatom.push_back(ofVec3f(7.871980,17.803638,18.494900));
     Hatom.push_back(ofVec3f(8.930068,18.906172,18.145149));
     Hatom.push_back(ofVec3f(-1.059553,2.102531,-3.198568));
     Hatom.push_back(ofVec3f(-0.001465,3.205065,-3.548319));
     Hatom.push_back(ofVec3f(0.456792,1.861801,-2.883195));
     Catom.push_back(ofVec3f(4.064772,7.420849,-2.463196));
     Hatom.push_back(ofVec3f(6.002144,7.571555,-1.934088));
     Hatom.push_back(ofVec3f(5.496960,8.809158,-2.744192));
     Hatom.push_back(ofVec3f(2.373554,12.199545,17.690773));
     Hatom.push_back(ofVec3f(1.868370,13.437148,16.880668));
     Catom.push_back(ofVec3f(3.805742,13.587854,17.409777));
     Hatom.push_back(ofVec3f(-0.776522,9.609569,-0.887827));
     Catom.push_back(ofVec3f(0.059997,8.192960,-2.031241));
     Hatom.push_back(ofVec3f(-0.329055,8.355516,-0.073238));
     Catom.push_back(ofVec3f(7.810517,12.815743,16.977821));
     Hatom.push_back(ofVec3f(8.199570,12.653187,15.019819));
     Hatom.push_back(ofVec3f(8.647037,11.399134,15.834408));
     Hatom.push_back(ofVec3f(-1.753813,4.721121,5.216357));
     Hatom.push_back(ofVec3f(-1.485832,3.546294,6.214788));
     Catom.push_back(ofVec3f(-0.794384,3.152981,4.340487));
     Catom.push_back(ofVec3f(8.664898,17.855722,10.606094));
     Hatom.push_back(ofVec3f(9.624327,16.287582,9.730224));
     Hatom.push_back(ofVec3f(9.356346,17.462409,8.731792));
     Hatom.push_back(ofVec3f(6.552432,17.904725,3.618567));
     Catom.push_back(ofVec3f(5.886206,18.291477,5.500342));
     Hatom.push_back(ofVec3f(6.869108,16.744797,4.618493));
     Catom.push_back(ofVec3f(1.984309,2.717226,9.446239));
     Hatom.push_back(ofVec3f(1.001406,4.263907,10.328087));
     Hatom.push_back(ofVec3f(1.318082,3.103979,11.328013));
     Catom.push_back(ofVec3f(4.190369,6.619051,-3.727677));
     Hatom.push_back(ofVec3f(3.330118,8.045356,-2.572307));
     Hatom.push_back(ofVec3f(3.823884,6.815899,-1.744266));
     Hatom.push_back(ofVec3f(4.046630,14.192804,16.690846));
     Catom.push_back(ofVec3f(3.680146,14.389652,18.674258));
     Hatom.push_back(ofVec3f(4.540396,12.963347,17.518887));
     Catom.push_back(ofVec3f(-1.138728,7.420849,-2.463196));
     Hatom.push_back(ofVec3f(0.798644,7.571555,-1.934088));
     Hatom.push_back(ofVec3f(0.293460,8.809158,-2.744192));
     Hatom.push_back(ofVec3f(7.577054,12.199545,17.690773));
     Hatom.push_back(ofVec3f(7.071870,13.437148,16.880668));
     Catom.push_back(ofVec3f(9.009242,13.587854,17.409777));
     Catom.push_back(ofVec3f(-1.956729,2.378731,3.809884));
     Hatom.push_back(ofVec3f(-0.425875,3.684048,3.618567));
     Hatom.push_back(ofVec3f(-0.109199,2.524121,4.618493));
     Hatom.push_back(ofVec3f(7.979713,18.484583,10.328087));
     Hatom.push_back(ofVec3f(8.296389,17.324655,11.328013));
     Catom.push_back(ofVec3f(9.827243,18.629972,11.136697));
     Hatom.push_back(ofVec3f(5.224494,18.941797,5.216357));
     Hatom.push_back(ofVec3f(5.492475,17.766970,6.214788));
     Catom.push_back(ofVec3f(7.072307,19.030467,6.050376));
     Catom.push_back(ofVec3f(0.798207,1.978236,8.896205));
     Hatom.push_back(ofVec3f(2.646020,2.066906,9.730224));
     Hatom.push_back(ofVec3f(2.378039,3.241733,8.731792));
     Catom.push_back(ofVec3f(2.959241,5.848108,-4.081911));
     Hatom.push_back(ofVec3f(4.403839,7.221415,-4.458565));
     Hatom.push_back(ofVec3f(4.930322,5.999811,-3.632019));
     Hatom.push_back(ofVec3f(2.940192,15.008892,18.578599));
     Hatom.push_back(ofVec3f(3.466675,13.787288,19.405146));
     Catom.push_back(ofVec3f(4.911273,15.160596,19.028492));
     Catom.push_back(ofVec3f(-1.013131,6.619051,-3.727677));
     Hatom.push_back(ofVec3f(-1.873381,8.045356,-2.572307));
     Hatom.push_back(ofVec3f(-1.379615,6.815899,-1.744266));
     Hatom.push_back(ofVec3f(9.250130,14.192804,16.690846));
     Catom.push_back(ofVec3f(8.883646,14.389652,18.674258));
     Hatom.push_back(ofVec3f(9.743896,12.963347,17.518887));
     Hatom.push_back(ofVec3f(-2.323271,1.838517,4.527319));
     Hatom.push_back(ofVec3f(-2.645226,3.001903,3.531877));
     Catom.push_back(ofVec3f(-1.632055,1.484893,2.658997));
     Hatom.push_back(ofVec3f(10.515740,18.006800,11.414703));
     Catom.push_back(ofVec3f(9.502570,19.523810,12.287584));
     Hatom.push_back(ofVec3f(10.193785,19.170186,10.419262));
     Hatom.push_back(ofVec3f(7.465250,19.552873,5.334435));
     Catom.push_back(ofVec3f(6.787091,19.952489,7.213220));
     Hatom.push_back(ofVec3f(7.733840,18.378789,6.331371));
     Hatom.push_back(ofVec3f(0.136674,2.629914,8.615209));
     Catom.push_back(ofVec3f(1.083423,1.056214,7.733361));
     Hatom.push_back(ofVec3f(0.405264,1.455830,9.612146));
     Hatom.push_back(ofVec3f(2.225646,6.476560,-4.176074));
     Hatom.push_back(ofVec3f(2.744693,5.261872,-3.340561));
     Catom.push_back(ofVec3f(3.026301,5.014767,-5.334434));
     Catom.push_back(ofVec3f(4.844214,15.993936,20.281015));
     Hatom.push_back(ofVec3f(5.125821,15.746831,18.287142));
     Hatom.push_back(ofVec3f(5.644869,14.532144,19.122655));
     Catom.push_back(ofVec3f(-2.244258,5.848108,-4.081911));
     Hatom.push_back(ofVec3f(-0.799661,7.221415,-4.458565));
     Hatom.push_back(ofVec3f(-0.273178,5.999811,-3.632019));
     Hatom.push_back(ofVec3f(8.143692,15.008892,18.578599));
     Hatom.push_back(ofVec3f(8.670175,13.787288,19.405146));
     Catom.push_back(ofVec3f(10.114773,15.160596,19.028492));
     Catom.push_back(ofVec3f(-2.719913,0.712160,2.091027));
     Hatom.push_back(ofVec3f(-0.941982,0.865794,2.945971));
     Hatom.push_back(ofVec3f(-1.252133,2.031960,1.953518));
     Hatom.push_back(ofVec3f(9.122647,18.976744,12.993062));
     Hatom.push_back(ofVec3f(8.812496,20.142910,12.000610));
     Catom.push_back(ofVec3f(10.590427,20.296544,12.855554));
     Hatom.push_back(ofVec3f(6.350380,19.442525,7.912720));
     Hatom.push_back(ofVec3f(6.166669,20.637876,6.918772));
     Catom.push_back(ofVec3f(8.004408,20.623933,7.796137));
     Catom.push_back(ofVec3f(-0.133894,0.384770,7.150444));
     Hatom.push_back(ofVec3f(1.703845,0.370828,8.027808));
     Hatom.push_back(ofVec3f(1.520134,1.566178,7.033861));
     Oatom.push_back(ofVec3f( 2.080274,4.201560,-5.509310));
     Oatom.push_back(ofVec3f( 3.978174,5.206020,-6.125109));
     Oatom.push_back(ofVec3f( 3.892341,15.802683,21.071689));
     Oatom.push_back(ofVec3f( 5.790240,16.807144,20.455890));
     Hatom.push_back(ofVec3f(-2.977854,6.476560,-4.176074));
     Hatom.push_back(ofVec3f(-2.458806,5.261872,-3.340561));
     Catom.push_back(ofVec3f(-2.177199,5.014767,-5.334434));
     Catom.push_back(ofVec3f(10.047713,15.993936,20.281015));
     Hatom.push_back(ofVec3f(10.329321,15.746831,18.287142));
     Hatom.push_back(ofVec3f(10.848369,14.532144,19.122655));
     Hatom.push_back(ofVec3f(-3.391774,1.307437,1.754729));
     Hatom.push_back(ofVec3f(-2.382980,0.168758,1.373591));
     Hatom.push_back(ofVec3f(-3.098331,0.148609,2.769601));
     Hatom.push_back(ofVec3f(10.968845,20.860095,12.176979));
     Hatom.push_back(ofVec3f(10.253494,20.839945,13.572990));
     Hatom.push_back(ofVec3f(11.262288,19.701267,13.191852));
     Catom.push_back(ofVec3f(7.699556,21.529796,8.982895));
     Hatom.push_back(ofVec3f(8.632897,19.940970,8.077133));
     Hatom.push_back(ofVec3f(8.432323,21.150833,7.102615));
     Hatom.push_back(ofVec3f(-0.561809,-0.142130,7.843966));
     Hatom.push_back(ofVec3f(-0.762383,1.067734,6.869448));
     Catom.push_back(ofVec3f(0.170959,-0.521093,5.963685));
     Oatom.push_back(ofVec3f(-3.123226,4.201560,-5.509310));
     Oatom.push_back(ofVec3f(-1.225326,5.206020,-6.125109));
     Oatom.push_back(ofVec3f( 9.095840,15.802683,21.071689));
     Oatom.push_back(ofVec3f(10.993740,16.807144,20.455890));
     Catom.push_back(ofVec3f(6.958495,22.752910,8.547950));
     Hatom.push_back(ofVec3f(7.166802,21.043681,9.631577));
     Hatom.push_back(ofVec3f(8.529639,21.791450,9.411862));
     Hatom.push_back(ofVec3f(-0.659125,-0.782746,5.534719));
     Catom.push_back(ofVec3f(0.912019,-1.744207,6.398631));
     Hatom.push_back(ofVec3f(0.703713,-0.034978,5.315004));
     Oatom.push_back(ofVec3f(5.706132,22.751591,8.642113));
     Oatom.push_back(ofVec3f(7.629624,23.710497,8.044997));
     Oatom.push_back(ofVec3f(0.240891,-2.701794,6.901584));
     Oatom.push_back(ofVec3f(2.164382,-1.742887,6.304468));
    
      
}

//--------------------------------------------------------------
void ofApp::atomNoise(){
    ofIcoSpherePrimitive icoSphere;
    icoSphere.setResolution(1);
    
    ofVboMesh sphereMesh;
    //offset = media*media*0.75;
    
    // Cholesteryl offset
    // float myOffset = media*media*120.0;
    
    // Cheda offset
    float myOffset = media*media*60.0 + 0.03;
    
    //icoSphere.setRadius(0.25+ofRandomf()*myOffset);
    icoSphere.setRadius(0.25);
    for (int i=0; i<Hatom.size(); i++) {
        icoSphere.setPosition(Hatom[i].x, Hatom[i].y, Hatom[i].z);
        sphereMesh = icoSphere.getMesh();
        auto &vertsS = sphereMesh.getVertices();
        for(unsigned int u = 0; u < vertsS.size(); u++){
            vertsS[u].x += Hatom[i].x + ofRandomf()*myOffset;
            vertsS[u].y += Hatom[i].y + ofRandomf()*myOffset;
            vertsS[u].z += Hatom[i].z + ofRandomf()*myOffset;
        }
        
        for(auto& normal : sphereMesh.getNormals()) {
            normal = glm::normalize(normal);
        }
        
        //icoSphere.drawWireframe();
        //sphereMesh.drawWireframe();
        //icoSphere.draw();
        //ofDrawSphere(Hatom[i].x, Hatom[i].y, Hatom[i].z,0.35*myOffset);
        sphereMesh.drawFaces();
    }
    
    //icoSphere.setRadius(0.35+ofRandomf()*myOffset);
    icoSphere.setRadius(0.35);
    for (int i=0; i<Oatom.size(); i++) {
        icoSphere.setPosition(Oatom[i].x, Oatom[i].y, Oatom[i].z);
        sphereMesh = icoSphere.getMesh();
        auto &vertsS = sphereMesh.getVertices();
        for(unsigned int u = 0; u < vertsS.size(); u++){
            vertsS[u].x += Oatom[i].x + ofRandomf()*myOffset;
            vertsS[u].y += Oatom[i].y + ofRandomf()*myOffset;
            vertsS[u].z += Oatom[i].z + ofRandomf()*myOffset;
        }
        
        for(auto& normal : sphereMesh.getNormals()) {
            normal = glm::normalize(normal);
        }
        
        //icoSphere.drawWireframe();
        //sphereMesh.drawWireframe();
        //icoSphere.draw();
        //ofDrawSphere(Oatom[i].x, Oatom[i].y, Oatom[i].z,0.35*myOffset);
        sphereMesh.drawFaces();
    }
    
    //icoSphere.setRadius(0.45+ofRandomf()*myOffset);
    icoSphere.setRadius(0.45);
    for (int i=0; i<Catom.size(); i++) {
        
        //icoSphere.setPosition(Catom[i].x+ofRandomf()*offset, Catom[i].y+ofRandomf()*offset, Catom[i].z+ofRandomf()*offset);
        icoSphere.setPosition(Catom[i].x, Catom[i].y, Catom[i].z);
        sphereMesh = icoSphere.getMesh();
        auto &vertsS = sphereMesh.getVertices();
        for(unsigned int u = 0; u < vertsS.size(); u++){
            vertsS[u].x += Catom[i].x + ofRandomf()*myOffset;
            vertsS[u].y += Catom[i].y + ofRandomf()*myOffset;
            vertsS[u].z += Catom[i].z + ofRandomf()*myOffset;
        }
        
        for(auto& normal : sphereMesh.getNormals()) {
            normal = glm::normalize(normal);
        }
        
        //icoSphere.drawWireframe();
        //sphereMesh.drawWireframe();
        //icoSphere.draw();
        //ofDrawSphere(Catom[i].x, Catom[i].y, Catom[i].z,0.45*myOffset);
        sphereMesh.drawFaces();
    }
    
    //icoSphere.setRadius(0.0.65+ofRandomf()*myOffset);
    icoSphere.setRadius(0.65);
    for (int i=0; i<CUatom.size(); i++) {
        
        //icoSphere.setPosition(Catom[i].x+ofRandomf()*offset, Catom[i].y+ofRandomf()*offset, Catom[i].z+ofRandomf()*offset);
        //icoSphere.setPosition(Catom[i].x, Catom[i].y, Catom[i].z);
        sphereMesh = icoSphere.getMesh();
        auto &vertsS = sphereMesh.getVertices();
        for(unsigned int u = 0; u < vertsS.size(); u++){
            vertsS[u].x += CUatom[i].x + ofRandomf()*myOffset;
            vertsS[u].y += CUatom[i].y + ofRandomf()*myOffset;
            vertsS[u].z += CUatom[i].z + ofRandomf()*myOffset;
        }
        
        for(auto& normal : sphereMesh.getNormals()) {
            normal = glm::normalize(normal);
        }
        
        //icoSphere.drawWireframe();
        //sphereMesh.drawWireframe();
        //icoSphere.draw();
        //ofDrawSphere(CUatom[i].x, CUatom[i].y, CUatom[i].z,0.65*myOffset);
        sphereMesh.drawFaces();
    }
    
}


//--------------------------------------------------------------
void ofApp::atomMovement(){
    ofIcoSpherePrimitive icoSphere;
    icoSphere.setResolution(1);
    
    ofVboMesh sphereMesh;
    //offset = media*media*0.75;
    
    // Cholesteryl offset
    // float myOffset = media*media*120.0;
    
    // Cheda offset
    // float myOffset = media*media*60.0 + 0.03;
    //float myOffset = thresholdS/255.0 + 0.03;
    float myOffset = thresholdS/1500.0 + 0.03;
    
    //icoSphere.setRadius(0.0.55+ofRandomf()*myOffset);
    icoSphere.setRadius(0.55);
    for (int i=0; i<CUatom.size(); i++) {
        icoSphere.setPosition(CUatom[i].x + ofRandomf()*myOffset, CUatom[i].y + ofRandomf()*myOffset, CUatom[i].z + ofRandomf()*myOffset);
        sphereMesh = icoSphere.getMesh();
        
        auto &vertsS = sphereMesh.getVertices();
        for(unsigned int u = 0; u < vertsS.size(); u++){
            vertsS[u].x += icoSphere.getX() + ofRandomf()*myOffset;
            vertsS[u].y += icoSphere.getY() + ofRandomf()*myOffset;
            vertsS[u].z += icoSphere.getZ() + ofRandomf()*myOffset;
        }
        for(auto& normal : sphereMesh.getNormals()) {
            normal = glm::normalize(normal);
        }
        
        //sphereMesh.drawWireframe();
        //material_01.begin();
        //icoSphere.draw();
        //material_01.end();
        //ofDrawSphere(Hatom[i].x, Hatom[i].y, Hatom[i].z,0.35*myOffset);
        //sphereMesh.drawFaces();
        
        material_03.begin();
        sphereMesh.drawFaces();
        material_03.end();
        
        material_02.begin();
        sphereMesh.drawWireframe();
        material_02.end();
        
    }
    
    //icoSphere.setRadius(0.25+ofRandomf()*myOffset);
    icoSphere.setRadius(0.15);
    for (int i=0; i<Hatom.size(); i++) {
        icoSphere.setPosition(Hatom[i].x + ofRandomf()*myOffset, Hatom[i].y + ofRandomf()*myOffset, Hatom[i].z + ofRandomf()*myOffset);
        sphereMesh = icoSphere.getMesh();
        
        //icoSphere.drawWireframe();
        //sphereMesh.drawWireframe();
        icoSphere.draw();
        //ofDrawSphere(Hatom[i].x, Hatom[i].y, Hatom[i].z,0.35*myOffset);
        //sphereMesh.drawFaces();
    }
    
    //icoSphere.setRadius(0.35+ofRandomf()*myOffset);
    icoSphere.setRadius(0.25);
    for (int i=0; i<Oatom.size(); i++) {
        icoSphere.setPosition(Oatom[i].x + ofRandomf()*myOffset, Oatom[i].y + ofRandomf()*myOffset, Oatom[i].z + ofRandomf()*myOffset);
        sphereMesh = icoSphere.getMesh();
        
        //icoSphere.drawWireframe();
        //sphereMesh.drawWireframe();
        icoSphere.draw();
        //ofDrawSphere(Hatom[i].x, Hatom[i].y, Hatom[i].z,0.35*myOffset);
        //sphereMesh.drawFaces();
    }
    
    ofColor actual = diffuse;
    material_01.setDiffuseColor(ofFloatColor(actual.r/255.0,actual.g/255.0,actual.b/255.0, actual.a/510.0));
    
    //icoSphere.setRadius(0.0.65+ofRandomf()*myOffset);
    icoSphere.setRadius(0.25);
    for (int i=0; i<Catom.size(); i++) {
        
        icoSphere.setPosition(Catom[i].x+ofRandomf()*myOffset, Catom[i].y+ofRandomf()*myOffset, Catom[i].z+ofRandomf()*myOffset);
        //icoSphere.setPosition(Catom[i].x, Catom[i].y, Catom[i].z);
        sphereMesh = icoSphere.getMesh();
        
        
        auto &vertsS = sphereMesh.getVertices();
        for(unsigned int u = 0; u < vertsS.size(); u++){
            vertsS[u].x += icoSphere.getX() + ofRandomf()*myOffset;
            vertsS[u].y += icoSphere.getY() + ofRandomf()*myOffset;
            vertsS[u].z += icoSphere.getZ() + ofRandomf()*myOffset;
        }
        for(auto& normal : sphereMesh.getNormals()) {
            normal = glm::normalize(normal);
        }
        
        //sphereMesh.drawWireframe();
        //material_01.begin();
        //icoSphere.draw();
        //material_01.end();
        //ofDrawSphere(Hatom[i].x, Hatom[i].y, Hatom[i].z,0.35*myOffset);
        //sphereMesh.drawFaces();
        
        material_02.begin();
        sphereMesh.drawWireframe();
        material_02.end();
        material_01.begin();
        sphereMesh.drawWireframe();
        material_01.end();
    }
    
}


//--------------------------------------------------------------
void ofApp::setupModels(){
    // load models
    //model_01.loadModel("AnyConv.com__2003649_Cholesteryl_benzoate_01.obj", true);
    model_01.loadModel("cuc10_Cheda_Poli-B.obj", true);
    //model_01.setScale(0.2, 0.2, 0.2);
    model_01.enableMaterials();
    
    rotationX = 0.0;
    rotationY = 0.0;
    rotationZ = 0.0;
    
    /*
    ofVec3f scale = model_01.getScale();
    cout << "Model scale:" << ofToString(scale) <<endl;
    ofVec3f position = model_01.getPosition();
    cout << "Model position:" << ofToString(position) <<endl;
    */
    
    
    
    model_02.loadModel("AnyConv.com__2003649_Cholesteryl_benzoate_02.obj", true);
    model_02.enableMaterials();
    
    model_03.loadModel("AnyConv.com__2003649_Cholesteryl_benzoate_03.obj", true);
    model_03.enableMaterials();
    
    model_04.loadModel("AnyConv.com__2003649_Cholesteryl_benzoate_04.obj", true);
    model_04.enableMaterials();
    
    model_05.loadModel("AnyConv.com__2003649_Cholesteryl_benzoate_05.obj", true);
    model_05.enableMaterials();
    
    /*
    //get the model attributes we need
    
    float normalizedScale = model.getNormalizedScale();
    ofVboMesh mesh = model.getMesh(0);
    */
}

//--------------------------------------------------------------
void ofApp::updateModels(){
    //model_01.setPosition(renderPosition.x,renderPosition.y,renderPosition.z);
    
    
    
}

//--------------------------------------------------------------
void ofApp::modelNoise(){
    ofVboMesh mesh_01 = model_01.getMesh(0);
    auto &verts_01 = mesh_01.getVertices();
    
    //Cholesteryl offset
    //float myOffset = media*media*120.0;
    //float myOffset = media*media*40.0;
    
    //Cheda offset
    //float myOffset = media*media*60.0 + 0.03;
    //float myOffset = thresholdS/500.0 + 0.03;
    float myOffset = thresholdS/1500.0 + 0.03;
    
    // Other ofsets
    // float myOffset = media*media*220.0 + 0.03;
    
    for(unsigned int i = 0; i < verts_01.size(); i++){
        verts_01[i].x += ofRandomf()*myOffset;
        verts_01[i].y += ofRandomf()*myOffset;
        verts_01[i].z += ofRandomf()*myOffset;
    }
    mesh_01.drawWireframe();
}

//--------------------------------------------------------------
void ofApp::setupVideo(){
    fbo.allocate(2048,1536, GL_RGBA);
    fbo.begin();
        ofClear(0,0,0);
    fbo.end();
    buffer.allocate(2048,1536, GL_RGBA);
    buffer.begin();
        ofClear(0,0,0);
    buffer.end();
    
    tempBuffer.allocate(2048,1536, GL_RGBA);
    tempBuffer.begin();
        ofClear(0,0,0);
    tempBuffer.end();
    
    tempTex.allocate(2048, 1536, GL_RGBA);
    bufferTex.allocate(2048, 1536, GL_RGBA);
    tempPix.allocate(2048, 1536, GL_RGBA);
    
    // shaders
    shaderContrast.load("shadersGL2/contrast");
    shaderSaturation.load("shadersGL2/greyscale");
    shaderMixture.load("shadersGL2/mixture");
    
    mode = 0;
}

//--------------------------------------------------------------
void ofApp::updateVideo(){
    
}
