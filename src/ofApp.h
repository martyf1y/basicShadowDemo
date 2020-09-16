#pragma once

#include "ofMain.h"
#include "ofxKinectV2.h"
#include "ofxGui.h"
#include "ofxCv.h"

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
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    
    // Colour changes
    void setupColorPalette();
    void changeColor();
    
    // Bird content
    void loadImages();
    void loadSounds();
    void loadNewBirdImage();
    
    // While playing ..
    void displayTimeIndicator();
    void displayDebugText();
    
    // Kinect Colour comparison functions
    Boolean gameColorComp(ofImage firstImgCheck, ofImage secondImgCheck);
    
    ofParameterGroup mainParams;
    ofxPanel panel;
    ofxPanel ofxCVPanel;
    bool debugMode = true;              // Allow setting to change

    
    ofVideoGrabber camera;
    ///// Gameplay parameters /////
    ofParameter <int> dimWidth = ofGetWidth();                    // The screen size
    ofParameter <int> dimHeight = ofGetHeight();
    ofParameter <int> absolutePlayTime = 7000; // THe time for the birds to show
    ofParameter <int> triggerPerAmount = 25;              // The amount to trigger the game
    ofParameter <int> pixelCheckMultiplier = 3;
    ofParameter <float> silhoetteAdjust = 0.75; // this changes the image size
    ofParameter <bool> randBird = true;

    // Visual parameters
    ofParameter<int> radius;
    ofParameter<bool> useBlur;
    ofParameter<bool> useGaussian;
    ofParameter<bool> useLinger;
    ofParameter<bool> withDepth;
    ofParameter<float> learningTime, thresholdValue;
    ofParameter<bool> useEdge;
    ofParameter<bool> crazyLand;
    ofParameter<int> crazyLandWindow;
    ofParameter<int> crazyLandStrength;


    // Kinect things
    ofxKinectV2 kinect;
    ofImage imgDepth;
    ofImage contentPixelCheck;
    ofImage blurDepthImg;
    ofxCv::RunningBackground background;
    ofImage lingerImageDepth;
    ofImage stupidSecondLingerImage, stupidSecondOutlineImage;
    ofImage grayDepth, edgeofDepth;
    ofImage crazyLandOfDistortion;
    
    
    
    ofVideoGrabber cam;
    ofxCv::FlowFarneback flow;
    ofMesh mesh;
    int stepSize, xSteps, ySteps;
    
    // Trigger timer
    float triggerTimer = 0; // This is the actual timer that can be reset or changed
    float triggerStartTime; // Resets and changes the trigger timer
    bool playTrigger = false;
    Boolean readyToPlay=false; // This makes the playing timer go down
    
    // Bird array
    ofImage birdCatalog[12];
    ofSoundPlayer birdSoundCatalog[13];
    
    // Bird variables
    int currentImageNumber = 0;
    ofImage currentBirdImage;
    ofSoundPlayer currentBirdSound;
    ofImage resizedBirdImage;
    
    // Saving and resizing images
    ofImage savedShape; // The pixels used
    int previousPixelCheckMultiplier;
    
    // Debug dimensions
    
    // Playing stuff and coverage percentage
    int perCount; // gets percent amount for image
    int totCount; // gets total percent amount for image
    int percentage; // Overall percent
    
    // Changes the background colour
    ofColor randomCol;
    ofColor randomColDepth;
    
    // Colour palette - safest way to generate background colour
    int colorPaletteSize = 10;
    ofColor colorPalette[10];
    ofColor colorPaletteDepth[10];
    
    // HUD
    ofTrueTypeFont titleFont;

};
