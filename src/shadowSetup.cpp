//
//  shadowSetup.cpp
//  ShadowV2Test
//
//  Created by Matt Martin on 3/08/17.
//
//
#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::changeColor() {
    
    // Pick at random one of the colours in the array
    int rCol = ofRandom(0,colorPaletteSize);
    
    // Change bg color
    randomCol = colorPalette[rCol];
    // Change depth color
    randomColDepth = colorPaletteDepth[rCol];
}


//--------------------------------------------------------------
void ofApp::setupColorPalette() {
    
    //MmmMmManual Settings!
    
    //pink maroon
    colorPalette[0] = ofColor(152,44,84);
    colorPaletteDepth[0] = ofColor(224,126,186);
    
    // dark blue
    colorPalette[1] = ofColor(64,50,170);
    colorPaletteDepth[1] = ofColor(21,108,253);
    
    // blue green
    colorPalette[2] = ofColor(11,151,153);
    colorPaletteDepth[2] = ofColor(105,198,199);
    
    //green
    colorPalette[3] = ofColor(7,167,90);
    colorPaletteDepth[3] = ofColor(67,255,186);
    
    //yellow
    colorPalette[4] = ofColor(240,198,28);
    colorPaletteDepth[4] = ofColor(252,236,63);
    
    //orange
    colorPalette[5] = ofColor(244,103,0);
    colorPaletteDepth[5] = ofColor(216,180,152);
    
    //red
    colorPalette[6] = ofColor(255,47,47);
    colorPaletteDepth[6] = ofColor(252,167,167);
    
    //purple
    colorPalette[7] = ofColor(150,100,252);
    colorPaletteDepth[7] = ofColor(202,181,244);
    
    //blue
    colorPalette[8] = ofColor(49,82,110);
    colorPaletteDepth[8] = ofColor(49,186,237);
    
    // pinker
    colorPalette[9] = ofColor(255,34,133);
    colorPaletteDepth[9] = ofColor(244,168,255);
    
}

//--------------------------------------------------------------
void ofApp::loadImages() {
    birdCatalog[0].load("images/fantail.png");
    birdCatalog[1].load("images/dotterel.png");
    birdCatalog[2].load("images/kingfisher.png");
    birdCatalog[3].load("images/magpie.png");
    birdCatalog[4].load("images/pigeon.png");
    birdCatalog[5].load("images/quail.png");
    birdCatalog[6].load("images/kakapo.png");
    birdCatalog[7].load("images/penguin.png");
    birdCatalog[8].load("images/kokako.png");
    birdCatalog[9].load("images/oystercatcher.png");
    birdCatalog[10].load("images/whio.png");
    birdCatalog[11].load("images/seagull.png");
}

//--------------------------------------------------------------
void ofApp::loadSounds() {
    //Load Sounds * * * * * * * * * * * * * *
    
    //replace with for loop above
    
    birdSoundCatalog[0].load("sounds/fantail.mp3");
    birdSoundCatalog[1].load("sounds/dotterel.mp3");
    
    birdSoundCatalog[2].load("sounds/kingfisher.mp3");
    birdSoundCatalog[3].load("sounds/magpie.mp3");
    birdSoundCatalog[4].load("sounds/pigeon.mp3");
    
    birdSoundCatalog[5].load("sounds/quail.mp3");
    birdSoundCatalog[6].load("sounds/kakapo.mp3");
    
    birdSoundCatalog[7].load("sounds/penguin.mp3");
    birdSoundCatalog[8].load("sounds/kokako.mp3");
    birdSoundCatalog[9].load("sounds/oystercatcher.mp3");
    birdSoundCatalog[10].load("sounds/whio.mp3");
    birdSoundCatalog[11].load("sounds/seagull.mp3");
    birdSoundCatalog[12].load("sounds/fantail.mp3"); // Yeah Jenna says this is bullshit but for some reason array 0 does not work!!
    
    // birdSoundCatalog[0].load("sounds/fantail.mp3");
}

//--------------------------------------------------------------
void ofApp::loadNewBirdImage() {
    
    currentBirdImage=birdCatalog[currentImageNumber];
    currentBirdSound=birdSoundCatalog[currentImageNumber];
    
    //originalBirdImage = currentBirdImage; //This makes sure we have the original size if we need to resize;
    
    // YOU NEED TO ADD HERE WHERE THE IMAGE GETS BIGGER
    currentBirdImage.resize(currentBirdImage.getWidth()*silhoetteAdjust, currentBirdImage.getHeight()*silhoetteAdjust);
    resizedBirdImage.allocate(dimWidth, dimHeight, OF_IMAGE_COLOR_ALPHA);
    
    int beginHereX = resizedBirdImage.getWidth()/2 - currentBirdImage.getWidth()/2;
    int endHereX = resizedBirdImage.getWidth()/2 + currentBirdImage.getWidth()/2;
    int beginhereY = resizedBirdImage.getHeight() - currentBirdImage.getHeight();
    for(int x = 1; x < resizedBirdImage.getWidth(); x++){
        for(int y = 1; y < resizedBirdImage.getHeight(); y++){// Took out top thrid because it is blocked by roof
            if(x > beginHereX &&
               x < endHereX &&
               y > beginhereY){ // This puts the resized image in the center bottom
                
                
                ofColor birdCol = currentBirdImage.getColor(x - beginHereX, y - beginhereY);// This is sourcing the image colour at a certain location
                resizedBirdImage.setColor(x, y, birdCol);
            }
            else{
                ofColor transCol = ofColor(255, 255, 255, 0); // transparency
                resizedBirdImage.setColor(x, y, transCol);
            }
        }
    }
    resizedBirdImage.update();
    currentBirdImage = resizedBirdImage;
    contentPixelCheck = resizedBirdImage;
    contentPixelCheck.resize(dimWidth/pixelCheckMultiplier, dimHeight/pixelCheckMultiplier);
    contentPixelCheck.update();
    savedShape.setColor(ofColor(0,0,0,0));

}


