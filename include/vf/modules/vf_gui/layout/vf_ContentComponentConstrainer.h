// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file LICENSE.txt for licensing information.

#ifndef VF_CONTENTCOMPONENTCONSTRAINER_VFHEADER
#define VF_CONTENTCOMPONENTCONSTRAINER_VFHEADER

// "shim" which goes between your constrainer and a ResizableWindow
// or derived class. This will adjust the parameters of the associated
// constrainer so that all constraints are based on the content component
// and do not include the title bar, native window frame, menu bar, or
// window border.
//
// If you set a minimum size with your constrainer, the content component will
// be constrained to EXACTLY your desired dimensions.
//
// This class is SUPER easy to use. First put a constrainer on your window
// or use the function ResizableWindow::setResizeLimits(), and then
// just call ContentComponentConstrainer::attachTo (yourResizableWindow).
// It will take care of deleting itself and handle everything for you.
class ContentComponentConstrainer
  : private ComponentBoundsConstrainer
  , private ComponentListener
{
public:
  // we can attach to anything with ResizableWindow as a base
  static void attachTo (ResizableWindow* resizableWindow)
  {
    ContentComponentConstrainer* contentConstrainer =
      new ContentComponentConstrainer (resizableWindow);
    resizableWindow->addComponentListener (contentConstrainer);
  }

private:
  ContentComponentConstrainer (ResizableWindow* resizableWindow)
   : m_resizableWindow (resizableWindow)
   , m_originalConstrainer (0)
  {
    // if you aren't using a custom constrainer, then at least put a
    // constraint on your ResizableWindow using ResizableWindow::setResizeLimits
    // so that it gets the defaultConstrainer.
    m_originalConstrainer = m_resizableWindow->getConstrainer();
    jassert (m_originalConstrainer); // must exist

    m_resizableWindow->setConstrainer (this);
    m_resizableWindow->addComponentListener (this);
  }

  void resizeStart()
  {
    m_originalConstrainer->resizeStart();
    copyConstraints (*m_originalConstrainer);
    adjustConstraints();
  }

  void resizeEnd()
  {
    m_originalConstrainer->resizeEnd();
  }

  void applyBoundsToComponent (Component* component,
                               const Rectangle<int>& bounds)
  {
    m_originalConstrainer->applyBoundsToComponent (component, bounds);
  }

  void copyConstraints (ComponentBoundsConstrainer& from)
  {
    setMinimumWidth (from.getMinimumWidth());
    setMaximumWidth (from.getMaximumWidth());
    setMinimumHeight (from.getMinimumHeight());
    setMaximumHeight (from.getMaximumHeight());
    setFixedAspectRatio (from.getFixedAspectRatio());

    int minimumWhenOffTheTop;
    int minimumWhenOffTheLeft;
    int minimumWhenOffTheBottom;
    int minimumWhenOffTheRight;

    minimumWhenOffTheTop = from.getMinimumWhenOffTheTop();
    minimumWhenOffTheLeft = from.getMinimumWhenOffTheLeft();
    minimumWhenOffTheBottom = from.getMinimumWhenOffTheBottom();
    minimumWhenOffTheRight = from.getMinimumWhenOffTheRight();

    setMinimumOnscreenAmounts (minimumWhenOffTheTop,
                               minimumWhenOffTheLeft,
                               minimumWhenOffTheBottom,
                               minimumWhenOffTheRight);
  }

  static int addWithoutOverflow (int a, int b)
  {
    if (a < (0x7ffffff-b))
      return a+b;
    else
      return 0x7fffffff;
  }

  // adjusts the current constraints to take into account decorations
  void adjustConstraints()
  {
    BorderSize<int> peerFrameBorder = m_resizableWindow->getPeer()->getFrameSize();
    BorderSize<int> contentCompBorder = m_resizableWindow->getContentComponentBorder();
    
    int extraWidth = peerFrameBorder.getLeftAndRight() + contentCompBorder.getLeftAndRight();
    int extraHeight = peerFrameBorder.getTopAndBottom() + contentCompBorder.getTopAndBottom();

    setMinimumHeight (m_originalConstrainer->getMinimumHeight() + extraHeight);
    setMaximumHeight (addWithoutOverflow (m_originalConstrainer->getMaximumHeight(), extraHeight));
    setMinimumWidth (m_originalConstrainer->getMinimumWidth() + extraWidth);
    setMaximumWidth (addWithoutOverflow (m_originalConstrainer->getMaximumWidth(), extraWidth));
  }

  void componentBeingDeleted (Component& component)
  {
    delete this;
  }

private:
  ResizableWindow* m_resizableWindow;
  ComponentBoundsConstrainer* m_originalConstrainer;
};

#endif