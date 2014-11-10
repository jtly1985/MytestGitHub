//
//  LoadingLayer.h
//  ChinaTaxi2
//
//  Created by Wang Lingbo on 14-6-20.
//
//

//  phoenx分支

#ifndef __ChinaTaxi2__LoadingLayer__
#define __ChinaTaxi2__LoadingLayer__

#include <iostream>
#include "../../KLEngine/KLEngine.h"
#include "../../util/NumberSprite/NumbersNode.h"


class LoadingLayer : public KLLayer
{
    //注意线程函数必须是静态的
    static void* updateInfo(void* args); 
    // ============================== 变量 ==============================
    /**
     * 
     */
    
    /**
     * 打开下一个页面的方法
     */
    CCObject* m_target;
    SEL_CallFunc m_callFunc;
    /**
     * 多线程加载数据
     */
    SEL_CallFunc m_callFuncData;
    /**
     * 加载数字
     */
    NumbersNode* m_number;
    /**
     * 百分号
     */
    CCSprite* m_percent;
    /**
     * 屏幕大小
     */
    CCSize m_screenSize;
    /**
     * loading
     */
    CCSprite* m_loading;
    /**
     * 多长时间改变一次百分比
     */
//    float m_changeTime;
    /**
     * 当前的百分比
     */
    int m_nowPercent;
    /**
     * 骨骼动画
     */
//    CCArmature* m_taxiBus;
    float m_startLength;
    /**
     * 图片资源的字符链表
     */
    CCArray* m_imageList;
    /**
     * 我的plist资源的字符链表
     */
    CCArray* m_plistList;
//    CCArray* m_yPlistList;
    /**
     * 图片资源加载到第几张
     */
    int m_loadIndex;
    /**
     * 人跑的位置百分比
     */
    float m_busPercent;
    /**
     * 加载方式
     */
//    bool m_loadingType;
    /**
     * 加载完成
     */
    bool isOver;
    
public:
    // ============================== 静态 ==============================
    /**
     * 游戏加载界面
     */
    static LoadingLayer* create(CCArray* imageList, CCArray* yPlistList, CCArray* plistList = NULL, CCObject* target = NULL, SEL_CallFunc callFunc = NULL, SEL_CallFunc callFuncData = NULL, float loadTime = 1.2);
public:
    // ============================== 方法 ==============================
    /**
     * 初始化
     */
    virtual bool init(CCArray* imageList, CCArray* yPlistList, CCArray* plistList, CCObject* target, SEL_CallFunc callFunc, SEL_CallFunc callFuncData, float loadTime);
    
    void initRefresh();
    /**
     * 触摸
     */
    virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
    virtual void ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent);
    virtual void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent);
    /**
     * 设置数字
     */
    void setPercent(int percent);
    /**
     * 回调改变百分比
     */
    void callFuncChange(float dt);
    /**
     * 加载成功
     */
    void loadingSuccessful();
    void loadTime();
    /**
     * 加载plist文件
     */
    void loadPlist();
    /**
     * 加载共有plist文件
     */
    void loadYAllPng(CCObject* pSender);
    /**
     * 加载image的回调延迟
     */
    void imageCallBcakDelay(CCObject* pSender);
    /**
     * 加载完一张图片的回调
     * 人物开始移动
     */
    void callBackLoadImage(CCObject* pSender);
    void callBackLoadPlist();
    /**
     * 人物移动完后的回调
     * 加载下一张图
     */
    void moveEnd();
    
    /**
     * 定时器
     */
    void update(float dt);
    
    
    /**
     * 移动的动作循环
     */
//    void moveEventCallFunc(CCArmature *armature, MovementEventType type, const char *name);
    virtual void onExit();
    /**
     * 构造
     */
    LoadingLayer();
    /**
     * 析构
     */
    virtual ~LoadingLayer();
    
};

#endif /* defined(__ChinaTaxi2__LoadingLayer__) */
