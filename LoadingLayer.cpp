//
//  LoadingLayer.cpp
//  ChinaTaxi2
//
//  Created by Wang Lingbo on 14-6-20.
//
//

#include "LoadingLayer.h"
#include "../LayerLevelDefine.h"
#include "../../util/Resources.h"
#include "AudioManage.h"
#include<time.h>
#include "../../data/ReadString.h"


#define random(x) (rand()%x)
#define PERCENTPNG       0.35
#define PERCENTPLIST     0.65

static pthread_t pid;
static CCArray* m_staticYPlist = NULL;
static CCArmature* m_taxiBus = NULL;
static float m_moveLength;
static LoadingLayer* m_instance;
static int m_staticYloadIndex = 0;

/**
 * 游戏加载界面
 */
LoadingLayer* LoadingLayer::create(CCArray* imageList, CCArray* yPlistList, CCArray* plistList, CCObject* target, SEL_CallFunc callFunc, SEL_CallFunc callFuncData, float loadTime)
{
    m_instance = new LoadingLayer();
    if (m_instance && m_instance->init(imageList, yPlistList, plistList, target, callFunc, callFuncData, loadTime)) {
        m_instance->autorelease();
        return m_instance;
    }
    else{
        delete m_instance;
        m_instance = NULL;
        return NULL;
    }
}
/**
 * 初始化
 */
bool LoadingLayer::init(CCArray* imageList, CCArray* yPlistList, CCArray* plistList, CCObject* target, SEL_CallFunc callFunc, SEL_CallFunc callFuncData, float loadTime)
{
    if (!KLLayer::init("LoadingLayer", 10))
    {
        return false;
    }

    m_imageList = imageList;
    if (m_imageList)
    {
        m_imageList->retain();
    }
    
    m_plistList = plistList;
    m_staticYPlist = yPlistList;
    if (m_staticYPlist)
    {
        m_staticYPlist->retain();
    }
    m_target = target;
    m_callFunc = callFunc;
    m_callFuncData = callFuncData;
    AudioManage::sharedEngine()->stopBackgroundMusic();
    
    m_screenSize = KLPlatform::screenSize();
    // 加载loading界面的图片
//    CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("v1.0.0-640x960-loading_number.plist");
//    KLImages::addSpecial("LoadingPlist.csv");
    initRefresh();
    

    this->scheduleUpdate();

    ReadString* readString = ReadString::create();
    const char* pReadStr = readString->getLoadingLayerStringValue().GetString();
    
    CCLabelTTF* label = CCLabelTTF::create(pReadStr, "Arial", 28);
    label->setPosition(ccp(m_screenSize.width/2, m_screenSize.height/2-293));
    label->setColor(ccc3(255, 255, 255));
    addChild(label, TOPLEVEL+1);
    
    readString->release();
    return true;
}
void LoadingLayer::initRefresh()
{
    CCLayerColor* colorLayer = CCLayerColor::create(ccc4(0, 0, 0, 255), m_screenSize.width, m_screenSize.height);
    addChild(colorLayer, LOWLEVEL);
    
    // 黄色的底图
    CCSprite* yellowBase = CCSprite::create("imagebig/PLAY-02.png");
    yellowBase->setPosition(ccp(m_screenSize.width * 0.5, m_screenSize.height * 0.5));
    addChild(yellowBase, LOWLEVEL);
    // 黑边
    CCSprite* black = CCSprite::create("imagebig/loading_bg.png");
    black->setPosition(yellowBase->getPosition());
    addChild(black, TOPLEVEL);
    // 遮罩
    CCSprite* mask = CCSprite::create("imagebig/PLAY-03.png");
    mask->setPosition(yellowBase->getPosition());
    addChild(mask, TOPLEVEL);
    // loading
    m_loading = CCSprite::createWithSpriteFrameName("loading_tag.png");
    // 百分号
    m_percent = CCSprite::createWithSpriteFrameName("loading_per.png");
    addChild(m_percent, TOPLEVEL);
    
//    m_loading->setPosition(ccp(m_screenSize.width * 0.5, m_screenSize.height * 0.5));
    addChild(m_loading, TOPLEVEL);
    // 骨骼动画

    CCArmatureDataManager::sharedArmatureDataManager()->addArmatureFileInfo(taxiImage, taxiConfig, taxiData);
    m_taxiBus = CCArmature::create("ChinaTaxi");
    m_taxiBus->getAnimation()->play("Run", - 1, - 1, 1);
    
    m_taxiBus->setPosition(ccp(- m_taxiBus->getContentSize().width * 0.5, m_taxiBus->getContentSize().height * 0.45));
    m_startLength = m_taxiBus->getPosition().x;
    m_moveLength = m_screenSize.width + m_taxiBus->getContentSize().width * 0.5;
    addChild(m_taxiBus, BASELEVEL);
    // 设置百分比 添加数字 设置位置
    setPercent(m_nowPercent);
    // 加载
    schedule(schedule_selector(LoadingLayer::callFuncChange), 1/ 60);
    // 动画先移动后加载
    srand((int)time(0));
    int percent = random(20) + 40;
    m_busPercent = (float)percent * 0.01;
    // 设置加载plist的大小
    if(m_staticYPlist)
    {
        m_staticYloadIndex = m_staticYPlist->count() * 0.5;
    }
    
    // 没有大图加载就加载plist
    if (m_imageList)
    {
        m_taxiBus->runAction(CCSequence::create(
//                                                CCMoveTo::create(0.7 * (m_imageList->count() - 1), ccp(m_moveLength * m_busPercent, m_taxiBus->getPosition().y)),
                                                CCMoveTo::create(0.7, ccp(m_moveLength * m_busPercent, m_taxiBus->getPosition().y)),
                                                CCCallFuncO::create(this, callfuncO_selector(LoadingLayer::callBackLoadImage), NULL),
                                                NULL));
    }
    else
    {
        // 我的加载方法
        if (m_plistList)
        {
            m_taxiBus->runAction(CCSequence::create(
                                                    CCMoveTo::create(0.7 * 3, ccp(m_moveLength * m_busPercent, m_taxiBus->getPosition().y)),
                                                    CCCallFunc::create(this, callfunc_selector(LoadingLayer::callBackLoadPlist)),
                                                    NULL));
            
        }
        // 加载公用方法
        else if(m_staticYPlist)
        {
            m_taxiBus->runAction(CCSequence::create(
                                                    CCMoveTo::create(0.7 * 3, ccp(m_moveLength * m_busPercent, m_taxiBus->getPosition().y)),
                                                    CCCallFuncO::create(this, callfuncO_selector(LoadingLayer::loadYAllPng), NULL),
//                                                    CCCallFunc::create(this, callfunc_selector(LoadingLayer::loadYAllPng)),
                                                    NULL));
        }
    }
    CCLayerColor* colorLayerLeft = CCLayerColor::create(ccc4(0, 0, 0, 255), (m_screenSize.width - 960) * 0.5, m_screenSize.height);
    colorLayerLeft->setPosition(ccp((m_screenSize.width - 960) * 0.5, 0));
    addChild(colorLayerLeft, TOPLEVEL);
    
    CCLayerColor* colorLayerRight = CCLayerColor::create(ccc4(0, 0, 0, 255), (m_screenSize.width - 960) * 0.5, m_screenSize.height);
    colorLayerLeft->setPosition(ccp(m_screenSize.width - (m_screenSize.width - 960) * 0.5, 0));
    addChild(colorLayerRight, TOPLEVEL);
}
/**
 * 触摸
 */
bool LoadingLayer::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
    
    return true;
}
void LoadingLayer::ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent)
{
    
}
void LoadingLayer::ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent)
{
    
}
/**
 * 设置数字
 */
void LoadingLayer::setPercent(int percent)
{
    if (m_number )
    {
        // 数字不变就不创建
        if(m_number->number_ == percent)
            return ;
        
        if (!m_number->getParent())
            return;
        
        m_number->removeFromParent();
        m_number = NULL;
    }
    // 百分比
    m_number = NumbersNode::create("lv_", percent);
    m_number->setScale(1.5);
    addChild(m_number, TOPLEVEL);
    // 百分号
    if (m_percent)
    {
        float totalLength = m_loading->getContentSize().width + m_percent->getContentSize().width + m_percent->getContentSize().width;
        m_loading->setPosition(ccp(m_screenSize.width * 0.5 - totalLength * 0.5 + m_loading->getContentSize().width * 0.5, m_screenSize.height * 0.5));
        m_number->setPosition(ccp(m_screenSize.width * 0.5 + m_loading->getContentSize().width * 0.5 + m_number->getContentSize().width * 0.5 * m_number->getScaleX(), m_screenSize.height * 0.5));
        m_percent->setPosition(ccp(m_number->getPosition().x + m_number->getContentSize().width * 0.5 * m_number->getScaleX() + m_percent->getContentSize().width * 0.5, m_number->getPosition().y));
    }
}
/**
 * 回调改变百分比的update
 */
void LoadingLayer::callFuncChange(float dt)
{
    if (m_nowPercent < 100)
    {
        float length = m_taxiBus->getPosition().x - m_startLength;
        m_nowPercent = (length / m_moveLength) * 100;
        if (m_nowPercent >= 100)
        {
            m_nowPercent = 100;
        }
        setPercent(m_nowPercent);
    }
}
/**
 * 加载成功
 */
void LoadingLayer::loadingSuccessful()
{
//    CCLog("LoadingLayer::loadingSuccessful加载成功");
    unschedule(schedule_selector(LoadingLayer::callFuncChange));
    m_nowPercent = 0;
    m_taxiBus->getAnimation()->stop();
    if (m_target != NULL)
    {
        (m_target->*m_callFunc)();
    }
    removeFromParent();
}
void LoadingLayer::loadPlist()
{
    m_loadIndex = 0;
    if (m_plistList)
    {
        // 我的加载方法
        callBackLoadPlist();
    }
    else if(m_staticYPlist)
    {
        // 公用加载方法
        loadYAllPng(NULL);
    }
    // 没有plist文件
    else
    {
        m_taxiBus->runAction(CCSequence::create(
                                                CCMoveTo::create(0.7, ccp(m_moveLength, m_taxiBus->getPosition().y)),
                                                CCCallFunc::create(this, callfunc_selector(LoadingLayer::loadingSuccessful)),
                                                NULL));
    }
}
/**
 * 加载共有plist文件
 */
void LoadingLayer::loadYAllPng(CCObject* pSender)
{
    m_staticYloadIndex--;
    if (m_staticYPlist && m_staticYloadIndex >= 0)
    {
        const char* loadPngName = ((CCString* )m_staticYPlist->objectAtIndex(m_staticYloadIndex))->getCString();
        CCLog("loading加载plist:%s", loadPngName);
        CCTextureCache::sharedTextureCache()->addImageAsync(loadPngName, this, callfuncO_selector(LoadingLayer::imageCallBcakDelay));
    }
    if(m_staticYloadIndex < 0)
    {
        // 开启线程加载plist
        pthread_create(&pid, NULL, updateInfo, NULL);
    }
}
/**
 * 加载image的回调延迟
 */
void LoadingLayer::imageCallBcakDelay(CCObject* pSender)
{
    CCNode* node = CCNode::create();
    this->addChild(node);
    node->runAction(CCSequence::create(CCDelayTime::create(0.2), CCCallFuncO::create(this, callfuncO_selector(LoadingLayer::loadYAllPng), NULL), NULL));
}
void* LoadingLayer::updateInfo(void* args)
{

    // 开线程加载plist文件，将plist加载到CCSpriteFrameCache
    CCSpriteFrameCache *cache = CCSpriteFrameCache::sharedSpriteFrameCache();
    CCTextureCache* teCache = CCTextureCache::sharedTextureCache();
    for (int i=0; i<m_staticYPlist->count() * 0.5; i++)
    {
        const char* m_pngFullPath = ((CCString* )m_staticYPlist->objectAtIndex(i))->getCString();
        const char* m_plistFullPath = ((CCString* )m_staticYPlist->objectAtIndex(i + m_staticYPlist->count() * 0.5))->getCString();
//        CCLog("m_pngFullPath:%s", m_pngFullPath);
//        CCLog("m_plistFullPath:%s", m_plistFullPath);
        
        CCTexture2D* texture = teCache->textureForKey(m_pngFullPath);
        cache->addSpriteFramesWithFile(m_plistFullPath, texture);
        // 删除纹理
        teCache->removeTextureForKey(m_pngFullPath);
    }
    // 可以异步加载数据 m_target为空，就跳过
    if (m_instance->m_target)
    {
        if (m_instance->m_callFuncData)
        {
            (m_instance->m_target->*m_instance->m_callFuncData)();
        }
        
    }
    m_instance->isOver = true;
//    m_taxiBus->runAction(CCSequence::create(
//                                            CCMoveTo::create(0.7, ccp(m_moveLength, m_taxiBus->getPosition().y)),
//                                            CCCallFunc::create(m_instance, callfunc_selector(LoadingLayer::loadingSuccessful)),
//                                            NULL));

    return NULL;
}
void LoadingLayer::callBackLoadPlist()
{
    // 加载完最后一张大图
    CCString* name = (CCString* )m_plistList->objectAtIndex(0);
//    CCLog("name:%s", name->getCString());
    if (m_loadIndex >= m_plistList->count())
    {
        if (m_staticYPlist)
        {
            loadYAllPng(NULL);
        }
        else
        {
            m_taxiBus->runAction(CCSequence::create(
                                                    CCMoveTo::create(0.7, ccp(m_moveLength, m_taxiBus->getPosition().y)),
                                                    CCCallFunc::create(this, callfunc_selector(LoadingLayer::loadingSuccessful)),
                                                    NULL));
        }
    }
    // 未加载完 继续加载
    else
    {
        if (m_plistList->count() > m_loadIndex)
        {
            CCString* plistName = (CCString* )m_plistList->objectAtIndex(m_loadIndex);
            m_loadIndex++;
            
//            CCSprite::createWithSpriteFrameName->addPlistCsv(plistName->getCString(), this, callfunc_selector(LoadingLayer::callBackLoadPlist));
        }
        
    }
}

/**
 * 加载完一张图片的回调
 * 人物开始移动
 */
void LoadingLayer::callBackLoadImage(CCObject* pSender)
{
    // 加载完最后一张大图
    if (m_loadIndex >= m_imageList->count())
    {
        m_taxiBus->runAction(CCSequence::create(
                                                CCMoveTo::create(0.7, ccp(m_moveLength * m_busPercent + 0.1, m_taxiBus->getPosition().y)),
                                                CCCallFunc::create(this, callfunc_selector(LoadingLayer::loadPlist)),
                                                NULL));
    }
    // 未加载完 继续加载
    else
    {
        if (m_imageList->count() > m_loadIndex)
        {
            CCString* imageName = (CCString* )m_imageList->objectAtIndex(m_loadIndex);
            CCLog("loadingLayer加载:%s", imageName->getCString());
            m_loadIndex++;
            CCTextureCache::sharedTextureCache()->addImageAsync(imageName->getCString(),this,callfuncO_selector(LoadingLayer::callBackLoadImage));
        }
        else
        {
            m_loadIndex++;
        }
    }
}
/**
 * 人物移动完后的回调
 * 加载下一张图
 */
void LoadingLayer::moveEnd()
{
    if (m_imageList->count() > m_loadIndex)
    {
        CCString* imageName = (CCString* )m_imageList->objectAtIndex(m_loadIndex);
        CCTextureCache::sharedTextureCache()->addImageAsync(imageName->getCString(),this,callfuncO_selector(LoadingLayer::callBackLoadImage));
    }
}
/**
 * 定时器
 */
void LoadingLayer::update(float dt)
{
    if (isOver) {
        m_taxiBus->runAction(CCSequence::create(
                                                CCMoveTo::create(0.7, ccp(m_moveLength, m_taxiBus->getPosition().y)),
                                                CCCallFunc::create(this, callfunc_selector(LoadingLayer::loadingSuccessful)),
                                            NULL));
        isOver = false;
    }
    
}

void LoadingLayer::onExit()
{
//    CCLog("LoadingLayer::onExit");
    KLLayer::onExit();
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->removeDelegate(this);
}
/**
 * 构造
 */
LoadingLayer::LoadingLayer()
:m_number(NULL)
,m_percent(NULL)
,m_nowPercent(0)
,m_loadIndex(0)
,m_busPercent(0)
,isOver(false)
{
    
}
/**
 * 析构
 */
LoadingLayer::~LoadingLayer()
{
    CCTextureCache::sharedTextureCache()->removeTextureForKey("imagebig/PLAY-02.png");
    CCTextureCache::sharedTextureCache()->removeTextureForKey("imagebig/loading_bg.png");
    CCTextureCache::sharedTextureCache()->removeTextureForKey("imagebig/PLAY-03.png");
    
//    CCSpriteFrameCache::sharedSpriteFrameCache()->removeSpriteFramesFromFile("v1.0.0-640x960-loading_number.plist");
    if (m_taxiBus)
    {
        m_taxiBus->getAnimation()->stop();
        m_taxiBus->unscheduleUpdate();
        m_taxiBus->removeFromParent();
        m_taxiBus = NULL;
    }
    if (m_instance) {
         m_instance = NULL;
    }
    if (m_imageList)
    {
        m_imageList->release();
    }
    if (m_staticYPlist)
    {
        m_staticYPlist->release();
    }
}