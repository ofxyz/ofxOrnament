/*
 *  Ornament.cpp
 *  textureExample
 *
 *  Created by Brian Eschrich on 01.10.15
 *  Copyright 2015 __MyCompanyName__. All rights reserved.
 *
 */

#include "Ornament.h"


//------------------------------------------------------------------
void Ornament::setup(int w, int h, WALLPAPER_GROUP wallpaperGroup_, int tileSize_, float angle_) {
    width = w;
    height = h;
    wallpaperGroup = wallpaperGroup_;
    tileSize = tileSize_;
    angle = angle_;
    fbo.allocate(width, height);

	if (ofIsGLProgrammableRenderer()) {
		ornamentShader.load("shadersGL3/ornament");
        wallpaperShader.load("shadersGL3/wallpaper");
	}
	else {
		ornamentShader.load("shadersGL2/ornament");
		wallpaperShader.load("shadersGL2/wallpaper");
	}
}

void Ornament::loadTexture(ofTexture texture){
    inputTexture = texture;

    int w = texture.getWidth();
    int h = texture.getHeight();
	int f = texture.getTextureData().glInternalFormat;

    fbo.allocate(w, h, f);
    tileFbo.allocate(w, h, f);
    resizeFbo.allocate(w, h, f);

    fbo.getTexture().setTextureMinMagFilter(inputTexture.getTextureData().minFilter, inputTexture.getTextureData().magFilter);
    tileFbo.getTexture().setTextureMinMagFilter(inputTexture.getTextureData().minFilter, inputTexture.getTextureData().magFilter);
    resizeFbo.getTexture().setTextureMinMagFilter(inputTexture.getTextureData().minFilter, inputTexture.getTextureData().magFilter);

    createOrnament();
}

void Ornament::setWallpaperGroup(int group){
    setWallpaperGroup((WALLPAPER_GROUP)(group%17));
}

void Ornament::setWallpaperGroup(WALLPAPER_GROUP group){
    wallpaperGroup = group;
    createOrnament();
}

WALLPAPER_GROUP Ornament::getWallpaperGroup(){
    return wallpaperGroup;
}

int Ornament::getWallpaperGroupAsInt(){
    return wallpaperGroup;
}

void Ornament::setTileSize(float t){
    tileSize = t;
}

int Ornament::getTileSize(){
    return tileSize;
}

void Ornament::setAngle(float angleRad){
    angle = angleRad;
}

float Ornament::getAngle(){
    return angle;
}

void Ornament::createOrnament(){
    tile = TileFactory::createTile(wallpaperGroup);
    //tile->setup(width,height);
    tile->setup(inputTexture.getWidth(),inputTexture.getHeight());
    update();
}

//------------------------------------------------------------------
void Ornament::update() {
    //resize texture
    resizeFbo.begin();
    ofClear(0, 0, 0, 0);
    ofVec2f pNew, sizeNew;
    getBoundingBox(tile->getOrnamentBase(),pNew,sizeNew);
    //toTextureSpace(pNew);
    //toTextureSpace(sizeNew);
    ofVec2f sizeTex = resize(ofVec2f(inputTexture.getWidth(),inputTexture.getHeight()), sizeNew);
    ofVec2f posTex = (pNew + sizeNew*0.5) - sizeTex*0.5;
    inputTexture.draw(posTex.x,posTex.y,sizeTex.x,sizeTex.y);
    resizeFbo.end();
    
    //create tile
    tileFbo.begin();
    ofClear(0, 0, 0, 0);
    ornamentShader.begin();
    ornamentShader.setUniform1i("cell_structure", tile->getCellStructure() );
    ornamentShader.setUniform1i("wallpaper_group", wallpaperGroup );
    ornamentShader.setUniform1f("width", inputTexture.getWidth() );
    ornamentShader.setUniform1f("height", inputTexture.getHeight() );
    resizeFbo.draw(0,0);
    
    ornamentShader.end();
    tileFbo.end();
    
    //create wallpaper
    fbo.begin();
    ofClear(255,255,255, 0);
    ofSetColor(255,255,255, 255);
    wallpaperShader.begin();
    wallpaperShader.setUniform1i("cell_structure", tile->getCellStructure() );
    wallpaperShader.setUniform1f("width", width );
    wallpaperShader.setUniform1f("height", height );
    wallpaperShader.setUniform1f("tile_size", tileSize );
    wallpaperShader.setUniform1f("angle", angle );
    wallpaperShader.setUniform1f("tex_width", inputTexture.getWidth() );
    wallpaperShader.setUniform1f("tex_height",inputTexture.getHeight() );
    tileFbo.draw(0, 0);
    wallpaperShader.end();
    fbo.end();
    
    outputTexture = fbo.getTexture();
    //outputTexture = tileFbo.getTexture();
    
    tile->updateTile();
}

void Ornament::setCellStructure(int s){
    tile->setCellStructure(s);
    update();
}

int Ornament::getCellStructure(){
    return tile->getCellStructure();
}

//------------------------------------------------------------------
///\ draw the ornament
void Ornament::draw(int x, int y) {
    outputTexture.draw(x, y);
}

void Ornament::drawDebug(int x, int y, int w, int h ){
    if (w == -1) {
        w = inputTexture.getWidth();
    }
    if (h == -1){
        h = inputTexture.getHeight();
    }
    
    
    ofPushMatrix();
    ofTranslate(x, y);
    //texture
    ofVec2f pNew, sizeNew;
    getBoundingBox(tile->getOrnamentBase(),pNew,sizeNew);
    
    ofVec2f sizeTex = resize(ofVec2f(inputTexture.getWidth(),inputTexture.getHeight()), sizeNew);
    ofVec2f posTex = (pNew + sizeNew*0.5) - sizeTex*0.5;
    ofVec2f scaleFactor = ofVec2f(w,h)/sizeTex;
    
    sizeTex*=scaleFactor;
    
    inputTexture.draw(0,0,sizeTex.x,sizeTex.y);
    
    //bounding box
    vector<ofVec2f> base = tile->getOrnamentBase();
    for (auto& point : base) {
        point -= posTex;
        point *= scaleFactor;
    }
    
    ofSetColor(255, 0, 0);
    ofNoFill();
    ofBeginShape();
    for (auto point : base) {
        ofVertex(point.x,point.y);
    }
    ofVertex(base[0].x,base[0].y);
    ofEndShape();
    ofSetColor(255);
    
    ofPopMatrix();
}

ofTexture& Ornament::getTexture(){
    return outputTexture;
}

vector<ofVec2f> Ornament::getOrnamentBase(){
    return tile->getOrnamentBase();
}

vector<ofVec2f> Ornament::getTilePoly(){
    return tile->getTilePoly();
}

ofVec2f Ornament::resize(ofVec2f src, ofVec2f dst)
{
    float ratioDst = dst.x/dst.y;
    float ratioSrc = src.x/src.y;
    
    float w = src.x;
    float h = w / ratioDst;
    
    
    if (ratioSrc > ratioDst && h > dst.y)
    {
        h = dst.y;
        w = h*ratioSrc;
    }
    else if (ratioSrc < ratioDst && w > dst.x)
    {
        w = dst.x;
        h = w/ratioSrc;
    }
    return ofVec2f(w, h);
}

void Ornament::getBoundingBox(vector<ofVec2f> input, ofVec2f& pos, ofVec2f& size){
    float xMin = 1000000;
    float xMax = 0;
    float yMin = 1000000;
    float yMax = 0;
    
    for(auto v:input){
        if(v.x < xMin) {xMin = v.x;}
        if(v.x > xMax) {xMax = v.x;}
        if(v.y < yMin) {yMin = v.y;}
        if(v.y > yMax) {yMax = v.y;}
    }
    
    pos = ofVec2f(xMin,yMin);
    size = ofVec2f(xMax-xMin,yMax-yMin);
}

void Ornament::toTextureSpace(ofVec2f& value){
    float ratioSrc = inputTexture.getWidth() / inputTexture.getHeight();
    float ratioDst = width/height;
    value.x = value.x * inputTexture.getWidth() / width;
    value.y = value.y * inputTexture.getHeight() / height;
}


