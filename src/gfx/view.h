#ifndef _VIEW_H_
#define _VIEW_H_

#include "../common/defines.h"
#include <algorithm>

namespace gcw
{
  class Manager;
  class Gfx;

  class View
  {
  protected:
    Manager* const manager;
    Gfx* const gfx;
  
  public:
    View(Manager *manager);
  
    virtual void activated() { }
    virtual void deactivated() { }
  
    virtual void render() = 0;
    virtual void handleEvents() = 0;
    
    enum class Type : u8
    {
      MENU,
      PATH,
      CORE,
      LOADING,
      PAUSE,
      KEYBIND,
      DIALOG
    };
  };
  
  class ViewWrapper : public View
  {
    View* view;
  private:
    void handleEvents() override { }
    void render() override { }

    
  public:
    ViewWrapper(Manager* manager) : View(manager), view(nullptr) { }
    
    void setView(View* view) { this->view = view; }
    
    View* operator->() const { return view; }
    
    
  };
  
  template<class T>
  class OffsettableList
  {
  protected:
    u32 offset;
    
  public:
    OffsettableList(u32 listSize) : offset(0), LIST_SIZE(listSize) { }
        
    u32 minOffset() const { return offset; }
    u32 maxOffset() const {
      u32 maxDisplayableOffset = offset + LIST_SIZE - 1;
      u32 maxRealOffset = count() - 1;
      return std::min(maxDisplayableOffset, maxRealOffset);
    }
    bool isSelected(u32 i) const { return i + offset == current(); }
    u32 relativeIndex(u32 i) const { return i - offset; }
    void reset() { offset = 0; }

    u32 getDisplayedAmount() const { return std::min(LIST_SIZE, count() - offset); }
    
    virtual u32 current() const = 0;
    virtual u32 count() const = 0;
    virtual void set(u32 i) = 0;
    virtual T get(u32 i) = 0;
    
    bool hasNext(u32 i) { return i < LIST_SIZE && i + offset < count(); }
    
    bool down() {
      if (current() < count() - 1)
      {
        set(current()+1);
        
        if (current() > maxOffset())
          ++offset;
        
        return true;
      }
      return false;
    }
    
    bool up() {
      if (current() > 0)
      {
        set(current()-1);
        
        if (current() < minOffset())
          --offset;
        
        return true;
      }
      return false;
    }
    
    bool nextPage() {
      if (current() < maxOffset())
      {
        set(maxOffset());
        return true;
      }
      else
      {
        u32 ccount = count();
        
        if (current() < ccount-1)
        {
          if (current()+LIST_SIZE <= ccount-1)
          {
            offset += LIST_SIZE;
            set(current()+LIST_SIZE);
          }
          else
          {
            offset = ccount - LIST_SIZE;
            set(ccount-1);
          }
          return true;
        }
      }
      return false;
    }
    
    bool prevPage() {
      if (current() > minOffset())
      {
        set(minOffset());
        return true;
      }
      else
      {
        if (offset > 0)
        {
          u8 delta = LIST_SIZE <= offset ? LIST_SIZE : offset;
          offset -= delta;
          set(current()-delta);
          return true;
        }
      }
      return false;
    }
    
    const u32 LIST_SIZE;
  };
  
}


#endif