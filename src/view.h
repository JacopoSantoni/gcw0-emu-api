#ifndef _VIEW_H_
#define _VIEW_H_

#include "defines.h"

namespace gcw
{
  struct ViewOffset
  {
    const u16 x, y;
    
    ViewOffset(const u16 x, const u16 y) : x(x), y(y) { }
  };
  
  
  class Manager;
  class Gfx;

  class View
  {
    protected:
      Manager* const manager;
      Gfx* const gfx;
    
    public:
      View(Manager *manager);
      virtual void render() = 0;
      virtual void handleEvents() = 0;
    
    
    static const ViewOffset TITLE_OFFSET;
    static const ViewOffset MENU_OFFSET;
  };
  
  template<class T>
  class OffsettableList
  {
  protected:
    u32 offset;
    
  public:
    OffsettableList(u32 listSize) : offset(0), LIST_SIZE(listSize) { }
    
    u32 getOffset() { return offset; }
    void changeOffset(int delta) { offset += delta; }
    
    u32 minOffset() { return offset; }
    u32 maxOffset() { return offset + LIST_SIZE - 1; }
    bool isSelected(u32 i) { return i + offset == current(); }
    u32 relativeIndex(u32 i) { return i - offset; }
    void reset() { offset = 0; }
    u32 currentOffset() { return offset; }
    
    virtual u32 current() = 0;
    virtual u32 count() = 0;
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
        if (offset < count()-1)
        {
          if (current()+LIST_SIZE <= count()-1)
          {
            offset += LIST_SIZE;
            set(current()+LIST_SIZE);
          }
          else
          {
            offset = count() - LIST_SIZE;
            set(count()-1);
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