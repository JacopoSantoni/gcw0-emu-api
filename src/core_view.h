#ifndef _CONTROLS_H_
#define _CONTROLS_H_

#include <SDL.h>
#include "emu_interface.h"
#include "menu.h"
#include "view.h"

namespace gcw {

struct ButtonDefinition
{
  bool enabled;
  u32 mask;
};
  
class ControlsHandler
{
  public:
    virtual void handleEvents() = 0;
};
  
class Controls;
class Manager;
  
class CoreControlsHandler : public ControlsHandler
{
  private:
    struct IntegralDeadZone { s32 min, max, delta; };
  
    const Controls * const controls;
  
    s8 indexForKey(GCWKey key);
    ButtonDefinition mapping[GCW_KEY_COUNT+GCW_ANALOG_COUNT];
    IntegralDeadZone analogDeadZone;
    AnalogMode analogMode;
  
  
    ButtonStatus status;
    AnalogStatus analogStatus;
  
    ButtonStatus suspendShortcut;
  
  public:
    CoreControlsHandler(Controls *controls) : controls(controls) { }
    void initControls(CoreInterface *core, ButtonStatus suspendKeys);
  
    virtual void handleEvents();
  
    friend class Controls;
};
  
class MainMenuControlsHandler : public ControlsHandler
{
  private:
    Menu* menu;
  
  public:
    MainMenuControlsHandler();
    void setMenu(Menu* menu) { this->menu = menu; }
  
    virtual void handleEvents();

};
  
class Controls
{
  private:
    Manager * const manager;
  
    CoreControlsHandler coreHandler;
    ControlsHandler *handler;
  
  
  public:
    Controls(Manager *manager) : manager(manager), coreHandler(this), handler(&coreHandler) { }
    void handleEvents() { handler->handleEvents(); }
  
    void swithToCoreControls(CoreInterface *core) { coreHandler.initControls(core, GCW_KEY_L | GCW_KEY_R); handler = &coreHandler; }
    ControlsHandler *current() { return handler; }
    ButtonStatus getButtonStatus() { return coreHandler.status; }
    AnalogStatus getAnalogStatus() { return coreHandler.analogStatus; }
  
    friend class CoreControlsHandler;
  

};
  
  
class CoreView : public View
{
  private:
    struct IntegralDeadZone { s32 min, max, delta; };
        
    s8 indexForKey(GCWKey key);
    ButtonDefinition mapping[GCW_KEY_COUNT+GCW_ANALOG_COUNT];
    IntegralDeadZone analogDeadZone;
    AnalogMode analogMode;
    
    
    ButtonStatus status;
    AnalogStatus analogStatus;
    
    ButtonStatus suspendShortcut;
  
    CoreInterface *core;
  
  public:
    CoreView(Manager *manager) : View(manager) { }
  
    void initControls(CoreInterface *core, ButtonStatus suspendKeys);
  
    virtual void render();
    virtual void handleEvents();
};
  
}


#endif
