#include "ofApp.h"

using namespace ofxCv;
using namespace cv;

//--------------------------------------------------------------
void ofApp::setup(){
    
    ofSetVerticalSync(true);
    ofSetFrameRate(120);
    
    /////// This is all crazy land stuff
    mesh.setMode(OF_PRIMITIVE_TRIANGLES);
    stepSize = 6;
    xSteps = 512 / stepSize; // 512
    ySteps = 424 / stepSize; // 424
    for(int y = 0; y < ySteps; y++) {
        for(int x = 0; x < xSteps; x++) {
            mesh.addVertex(ofVec2f(x * stepSize, y * stepSize));
            mesh.addTexCoord(ofVec2f(x * stepSize, y * stepSize));
        }
    }
    for(int y = 0; y + 1 < ySteps; y++) {
        for(int x = 0; x + 1 < xSteps; x++) {
            int nw = y * xSteps + x;
            int ne = nw + 1;
            int sw = nw + xSteps;
            int se = sw + 1;
            mesh.addIndex(nw);
            mesh.addIndex(ne);
            mesh.addIndex(se);
            mesh.addIndex(nw);
            mesh.addIndex(se);
            mesh.addIndex(sw);
        }
    }

    titleFont.load("LikesSlab.otf", 40, true, true);
    titleFont.setLineHeight(18.0f);
    titleFont.setLetterSpacing(1.037);
    
    //Uncomment for verbose info from libfreenect2
    ofSetLogLevel(OF_LOG_VERBOSE);
    kinect.open(0);

    //ofHideCursor();
    
    ofxCVPanel.setup("OFX CV", "settings.xml", ofGetWidth() - 250, ofGetHeight()/2);
  //  ofxCVPanel.setSize(300, 1000);
    ofxCVPanel.add(useBlur.set("Use Blur", false));
    ofxCVPanel.add(useGaussian.set("Use Gaussian", true));
    ofxCVPanel.add(radius.set("Radius", 6, 0, 100));
    ofxCVPanel.add(useLinger.set("Linger Shadow", false));
    ofxCVPanel.add(learningTime.set("Learning Time", 0.5, 0, 30));
    ofxCVPanel.add(thresholdValue.set("Threshold Value", 22, 0, 255));
    ofxCVPanel.add(useEdge.set("Edge of Shadow", false));
    ofxCVPanel.add(withDepth.set("With Depth", false));
    ofxCVPanel.add(crazyLand.set("Crazy Land", false));
    ofxCVPanel.add(crazyLandWindow.set("Crazy Window", 15, 1, 30));
    ofxCVPanel.add(crazyLandStrength.set("Crazy Strength", 5, 1, 30));


    
    kinect.params.add(dimWidth.set("Screen Width", dimWidth, 400, 1920));
    kinect.params.add(dimHeight.set("Screen Height", dimHeight, 400, 1080));
    kinect.params.add(absolutePlayTime.set("Play Time", absolutePlayTime, 2000, 300000));
    kinect.params.add(triggerPerAmount.set("Percentage Silhouette Trigger", triggerPerAmount, 5, 70));
    kinect.params.add(randBird.set("Random Bird", randBird, 0, 1));
    kinect.params.add(pixelCheckMultiplier.set("Pixel multiplier", pixelCheckMultiplier, 1, 5));
    panel.setup("Kinect 4 life", "settings.xml", 10, ofGetHeight()/2);
    panel.add(kinect.params);

    setupColorPalette();                                                        // Set array of background colours
    changeColor();
    
    loadImages();                                                               // Load bird images
    loadSounds();
    currentImageNumber = ofRandom(12);
    loadNewBirdImage();
    
    savedShape.allocate(dimWidth, dimHeight, OF_IMAGE_COLOR_ALPHA);
    savedShape.setColor(ofColor(0,0,0,0));
}

//--------------------------------------------------------------
void ofApp::update(){

    // This is here to update the content image to the right size for the pixel check if we change it live
    if(previousPixelCheckMultiplier != pixelCheckMultiplier){
        loadNewBirdImage();
        previousPixelCheckMultiplier = pixelCheckMultiplier;
    }
    
    kinect.update();
    if( kinect.isFrameNew() ){
        
        imgDepth.setFromPixels(kinect.getDepthPixels()); // converts kinect depth to image
        imgDepth.update();
        //imgDepth.resize(dimWidth/pixelCheckMultiplier, dimHeight/pixelCheckMultiplier);

        
        if(useLinger){ ////////// BACKGROUND FADE UPDATE
            background.setLearningTime(learningTime);
            background.setThresholdValue(thresholdValue);
            background.update(imgDepth, lingerImageDepth);
            lingerImageDepth.update();
            stupidSecondLingerImage.allocate(lingerImageDepth.getWidth(), lingerImageDepth.getHeight(), OF_IMAGE_COLOR_ALPHA);
            for(int i = 0; i < lingerImageDepth.getWidth() * lingerImageDepth.getHeight(); i ++){
                if(lingerImageDepth.getColor(i) == ofColor(255, 255, 255, 255)){
                    stupidSecondLingerImage.setColor(i*4, ofColor(255));
                }
                else{
                    stupidSecondLingerImage.setColor(i*4, ofColor(0,0));
                }
            }
            stupidSecondLingerImage.update();
        }
        if(useBlur) {  /////// BLUR UPDATE
            ofxCv::copy(imgDepth, blurDepthImg); // copies image for ofxcv to use, its also the actual displayed image and is required to blur
            if(useGaussian){
                ofxCv::GaussianBlur(blurDepthImg, radius);
            } else {
                ofxCv::blur(blurDepthImg, radius);
            }
            blurDepthImg.update();
        }
        else if(useEdge){  /////// OUTLINE UPDATE
            convertColor(imgDepth, grayDepth, CV_RGB2GRAY);
            Canny(grayDepth, edgeofDepth, 200, 100, 3);
            edgeofDepth.update();
            if(withDepth){
            stupidSecondOutlineImage.allocate(edgeofDepth.getWidth(), edgeofDepth.getHeight(), OF_IMAGE_COLOR_ALPHA);
            for(int i = 0; i < edgeofDepth.getWidth() * edgeofDepth.getHeight(); i ++){
                if(edgeofDepth.getColor(i) != ofColor(0, 0, 0, 255)){
                    stupidSecondOutlineImage.setColor(i*4, ofColor(255));
                }
                else{
                    stupidSecondOutlineImage.setColor(i*4, ofColor(0,0));
                }
            }
            stupidSecondOutlineImage.update();
            }
        }
       
        if(crazyLand){ ////// Distortion Update
        flow.setWindowSize(crazyLandWindow);
        flow.calcOpticalFlow(imgDepth);
        int i = 0;
        float distortionStrength = crazyLandStrength;
        for(int y = 1; y + 1 < ySteps; y++) {
            for(int x = 1; x + 1 < xSteps; x++) {
                int i = y * xSteps + x;
                ofVec2f position(x * stepSize, y * stepSize);
                ofRectangle area(position - ofVec2f(stepSize, stepSize) / 2, stepSize, stepSize);
                ofVec2f offset = flow.getAverageFlowInRegion(area);
                mesh.setVertex(i, position + distortionStrength * offset);
                i++;
            }
        }
        }
    }
    
    //We compare the two images and see if they are the same
    bool setReadySwitch = false;
    setReadySwitch = gameColorComp(imgDepth, contentPixelCheck);
    
    if (setReadySwitch) {
        // reset the ready to play timer...
        readyToPlay=true;
        if (!currentBirdSound.isPlaying()) {
            currentBirdSound.play();
        }
    }
    
    // Wait until triggered
    if (!readyToPlay)  {
        triggerStartTime = ofGetElapsedTimeMillis();
    }
    
    // Timer
    triggerTimer =  absolutePlayTime - (ofGetElapsedTimeMillis() - triggerStartTime);
    //Indicate time left to play
    if(triggerTimer < 0){ // Don't want dimmer to be there
        readyToPlay = false;
        triggerStartTime = ofGetElapsedTimeMillis();
        
        if (currentBirdSound.isPlaying()) {
            currentBirdSound.stop();
        }
    }
    
    }

//--------------------------------------------------------------
void ofApp::draw(){

    if(!crazyLand){
    ofSetColor(randomCol);
    ofSetRectMode(OF_RECTMODE_CENTER);
    ofDrawRectangle(dimWidth/2,dimHeight/2, dimWidth, dimHeight); // The main space
    
    ofSetColor(randomColDepth);
    
    if(stupidSecondLingerImage.isAllocated() && useLinger) {
        stupidSecondLingerImage.draw(dimWidth/2,dimHeight/2, dimWidth, dimHeight);
    }
    
    if (useBlur && blurDepthImg.isAllocated()){
        blurDepthImg.draw(dimWidth/2,dimHeight/2, dimWidth, dimHeight);
    }
    else if(useEdge && edgeofDepth.isAllocated()){
        
        if(withDepth && stupidSecondOutlineImage.isAllocated()){
        imgDepth.draw(dimWidth/2,dimHeight/2, dimWidth, dimHeight);
        ofSetColor(255, 0, 0);
        stupidSecondOutlineImage.draw(dimWidth/2,dimHeight/2, dimWidth, dimHeight);
        ofSetColor(randomColDepth);
        }
        else{
            edgeofDepth.draw(dimWidth/2,dimHeight/2, dimWidth, dimHeight);
        }
    }
    
    else if(imgDepth.isAllocated()){
        imgDepth.draw(dimWidth/2,dimHeight/2, dimWidth, dimHeight);
    }
    }
    
    else{
            ofColor invertCol = randomCol.getInverted();
            ofSetColor(invertCol);
            ofSetRectMode(OF_RECTMODE_CORNER);
            ofDrawRectangle(0,0, dimWidth, dimHeight); // The main space
        
            ofColor invertColDepth = randomColDepth.getInverted();
            ofSetColor(invertColDepth);
            imgDepth.getTexture().bind();
            mesh.draw();
            imgDepth.getTexture().unbind();
        
            crazyLandOfDistortion.grabScreen(0, 0, imgDepth.getWidth(), imgDepth.getHeight());
            crazyLandOfDistortion.resize(dimWidth, dimHeight);
            crazyLandOfDistortion.update();
            ofSetColor(255);
            crazyLandOfDistortion.draw(0,0, dimWidth, dimHeight);
    }
    ofSetRectMode(OF_RECTMODE_CENTER);

    ofSetColor(0, 0, 0, 255);
    currentBirdImage.draw(dimWidth/2,dimHeight/2, dimWidth, dimHeight); // Silhouette
    
    
    displayTimeIndicator();
    ofSetColor(255, 255, 255);
    ofSetRectMode(OF_RECTMODE_CENTER);
    savedShape.draw(dimWidth/2,dimHeight/2, dimWidth, dimHeight); // The image created by the depth
    
    
    if (debugMode) {
        //Display debug stats
        displayDebugText();
        panel.draw();
        ofxCVPanel.draw();
    }
    
    // Outside Space
    ofSetColor(255, 0, 0);
    ofDrawLine(dimWidth+1, 0, dimWidth+1, dimHeight+1);
    ofDrawLine(0, dimHeight+1, dimWidth+1, dimHeight+1);
    
    ofRectangle bounds = titleFont.getStringBoundingBox("SHADOW SHOWDOWN", dimWidth/2, 50);
    ofSetColor(200, 10, 30, 150);

    ofDrawRectangle(dimWidth/2, 45, bounds.width + 20, bounds.height + 30);
    ofSetColor(225);
    titleFont.drawString("SHADOW SHOWDOWN", -bounds.width/2 + dimWidth/2, bounds.height/2 + 45);
}

//--------------------------------------------------------------
Boolean ofApp::gameColorComp(ofImage firstImgCheck, ofImage secondImgCheck){
    // This should always be whatever the image we are comparing to
    // firstImgCheck.resize(dimWidth/pixelCheckMultiplier, dimHeight/pixelCheckMultiplier);

    savedShape.setColor(ofColor(0,0,0,0)); // We clear the whole image before adding in the pixels
    
    totCount = 0;
    perCount = 0;
    int pixDiff = (pixelCheckMultiplier)*0.5;
    //IMPORTANT!! BECAUSE WE ARE FILLING IN AROUND THE ACTUAL DEPTH POINT WE NEED TO MAKE SURE OUR CHECKS STAY WITHIN THE CONTENT AREA
    for(int x = pixDiff; x < secondImgCheck.getWidth()-pixDiff; x++){
        
        for(int y = pixDiff; y < secondImgCheck.getHeight()-pixDiff; y++){
            
            ofColor shapeContentCol = secondImgCheck.getColor(x, y);// This is sourcing the image colour at a certain location
            
            if(shapeContentCol.a != 0){ // We only need to check the alpha dots
                
                // Make the image to compare against at the same point
                int XMap = ofMap(x, 0, secondImgCheck.getWidth(), 0, firstImgCheck.getWidth());
                int YMap = ofMap(y, 0, secondImgCheck.getHeight(), 0, firstImgCheck.getHeight());

                ofColor depthImageCol = firstImgCheck.getColor(XMap, YMap);
                
                if(depthImageCol.a != 0){
                    
                    // Hey! We got something!
                    perCount ++;
                    
                    int xMultiple = x*pixelCheckMultiplier;
                    int yMultiple = y*pixelCheckMultiplier;
                    
                    for(int pixX = -pixDiff; pixX <= pixDiff; pixX++){
                        for(int pixY = -pixDiff; pixY <= pixDiff; pixY++){
                            shapeContentCol = currentBirdImage.getColor(xMultiple +pixX, yMultiple +pixY);
                            savedShape.setColor(xMultiple+pixX, yMultiple+pixY, shapeContentCol);// x, y
                        }
                    }
                    
                }
                totCount ++;
            }
        }
    }
    savedShape.update();
    
    percentage = ((float)perCount / (float)totCount)*100;
    if (percentage >= triggerPerAmount) {
        //cout << "TRIGGERED PLAY! Let's go! " << x << " and " << y << endl;
        playTrigger=true;
        
    }
    else{
        playTrigger = false;
    }
    
    return playTrigger;
}

//--------------------------------------------------------------
void ofApp::displayTimeIndicator() {
    
    //sides of screen
    int timeHeight = ofMap(triggerTimer,0,absolutePlayTime,0, 100);
    
    ofSetRectMode(OF_RECTMODE_CORNER);
    ofSetColor(170, 170, 170, 155);
    //ofRect(0,0,timeWidth,dimHeight-timeHeight);
    ofDrawRectangle(dimWidth - 150, 50, 100, 100);
    ofSetColor(0, 170, 102, 155);
    //ofRect(0,0,timeWidth,dimHeight-timeHeight);
    ofDrawRectangle(dimWidth - 150, 150, 100, -timeHeight);
    ofSetColor(255, 255, 255, 255);
}


//--------------------------------------------------------------
void ofApp::displayDebugText() {
    
    //DEBUG STATS
    
    ofSetColor(255);
    ofDrawBitmapString("Framerate: " + ofToString(ofGetFrameRate()), 15, 20);
    ofDrawBitmapString("FRACTION MADE: " + ofToString(perCount) + " / " + ofToString(totCount), 15, 40);
    ofDrawBitmapString("PERCENT MADE: " + ofToString(percentage) + "%", 15, 60);
    ofDrawBitmapString("Game Timer: " + ofToString(triggerTimer), 15, 80);
    ofDrawBitmapString("CHANGE CONTENT SIZE: ',' and '.'. CURRENTLY: " + ofToString(silhoetteAdjust), 15, 100);
    ofDrawBitmapString("CHANGE CONTENT SHAPE: '-' and '='", 15, 120);
    ofDrawBitmapString("CHANGE BACKGROUND COLOUR: SPACEBAR", 15, 140);
    
}


//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    if (key == 'd') {
        debugMode = !debugMode;
        if(debugMode){
            ofShowCursor();
        }
        else {
            ofHideCursor();
        }
    }
    if(key == ','){
        silhoetteAdjust -=0.1;
        loadNewBirdImage();
    }
    else if(key == '.'){
        silhoetteAdjust +=0.1;
        
        loadNewBirdImage();
    }
    if(key == '-'){
        currentImageNumber--;
        if(currentImageNumber<0){
            currentImageNumber=11;
        }
        loadNewBirdImage();
    }
    else if(key == '='){
        currentImageNumber++;
        if(currentImageNumber>11){
            currentImageNumber=0;
        }
        loadNewBirdImage();
    }
    
    if(key == ' '){
        changeColor();
    }
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    
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
