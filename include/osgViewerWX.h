#ifndef _OSGVIEWERWX_H_
#define _OSGVIEWERWX_H_

#include "wx/defs.h"
#include "wx/app.h"
#include "wx/cursor.h"
#include "wx/glcanvas.h" //to work with openscenegraph opengl

#include <osgViewer/Viewer>
#include <string>
#include <wx/filedlg.h> //for file dialog
#include <wx/listctrl.h> //for popup menu



// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

// For wxCURSOR_BLANK below, but isn't used a.t.m.
//#ifdef WIN32
//#include "wx/msw/wx.rc"
//#endif

#include <wx/image.h>
#include <wx/menu.h>

#include <osgViewer/ViewerEventHandlers>
#include <osgDB/ReadFile>

#include <osg/ShapeDrawable>
#include <osg/Geode>

#include "openalsoftaudioengine.h"
#include "EditorManipulator.h"

#include "soundproducer.h"
#include "CreateSoundProducerDialog.h"

#include <iostream>
#include <memory> //for unique_ptr use

class GraphicsWindowWX;

class OSGCanvas : public wxGLCanvas
{
public:
    OSGCanvas(wxWindow *parent, wxWindowID id = wxID_ANY,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize, long style = 0,
        const wxString& name = wxT("Binaural Audio Editor Canvas"),
        int *attributes = 0);

    virtual ~OSGCanvas();

    void SetGraphicsWindow(osgViewer::GraphicsWindow *gw)   { _graphics_window = gw; }

    void OnPaint(wxPaintEvent& event);
    void OnSize(wxSizeEvent& event);
    void OnEraseBackground(wxEraseEvent& event);

	//where camera viewer gets manipulated by mouse movements
	
    void OnChar(wxKeyEvent &event);
    void OnKeyUp(wxKeyEvent &event);
    
    void OnKeyDown(wxKeyEvent& event); 
	
    void OnMouseEnter(wxMouseEvent &event);
    void OnMouseDown(wxMouseEvent &event);
    void OnMouseUp(wxMouseEvent &event);
    void OnMouseMotion(wxMouseEvent &event);
    void OnMouseWheel(wxMouseEvent &event);

    void UseCursor(bool value);

	void SetContextCurrent();
	
	
private:
    DECLARE_EVENT_TABLE()

    osg::ref_ptr<osgViewer::GraphicsWindow> _graphics_window;

    wxCursor _oldCursor;
    wxGLContext _context;
    
};

class GraphicsWindowWX : public osgViewer::GraphicsWindow
{
public:
    GraphicsWindowWX(OSGCanvas *canvas);
    ~GraphicsWindowWX();

    void init();

    //
    // GraphicsWindow interface
    //
    void grabFocus();
    void grabFocusIfPointerInWindow();
    void useCursor(bool cursorOn);

    bool makeCurrentImplementation();
    void swapBuffersImplementation();

    // not implemented yet...just use dummy implementation to get working.
    virtual bool valid() const { return true; }
    virtual bool realizeImplementation() { return true; }
    virtual bool isRealizedImplementation() const  { return _canvas->IsShownOnScreen(); }
    virtual void closeImplementation() {}
    virtual bool releaseContextImplementation() { return true; }

private:
    // XXX need to set _canvas to NULL when the canvas is deleted by
    // its parent. for this, need to add event handler in OSGCanvas
    OSGCanvas*  _canvas;
};


class MainFrame : public wxFrame
{
public:
    MainFrame(wxFrame *frame, const wxString& title, const wxPoint& pos,
        const wxSize& size, long style = wxDEFAULT_FRAME_STYLE);
	
	//for connecting mainframe to wxOsgApp
	
    void SetViewer(osgViewer::Viewer *viewer);
    void SetRootNode(osg::Geode* root); 
    void SetSoundProducerVectorRef(std::vector <SoundProducer*> *sound_producer_vector);
    
    // Mainframe menu operations
    
    void OnOpen(wxCommandEvent& WXUNUSED(event));
    void OnIdle(wxIdleEvent& event);
	void OnExit(wxCommandEvent& event);
	void OnAbout(wxCommandEvent& event);
	
	void OnListRightClick(wxListEvent &evt);
	void OnPopupClick(wxCommandEvent &evt);
	
	void OnKeyDown(wxKeyEvent& event); //where camera viewer gets manipulated
	
private:
    osg::ref_ptr<osgViewer::Viewer> _viewer;
	osg::ref_ptr<osg::Geode> _rootNode;
	
	enum
	{
		ID_CREATE_SOUND_PRODUCER = 1
	};
	
	std::vector <SoundProducer*> *sound_producer_vector_ref;
	
	void OnCreateSoundProducer(wxCommandEvent& event); //function for menu to create and place sound producer
	void CreateSoundProducer(double& x, double& y, double& z);
	
	void OnEditSoundProducers(wxCommandEvent& event); //function for menu to edit current available sound producers
	
    DECLARE_EVENT_TABLE()
};

/* Define a new application type */
//the main of the application
class wxOsgApp : public wxApp
{
public:
    bool OnInit();
    
    
private:
	osg::ref_ptr<osg::Geode> rootNode; //geometry node to hold ShapeDrawable objects
	
	std::vector <SoundProducer*> sound_producer_vector;
	
	void initListener();
	
};

#endif // _WXSIMPLEVIEWERWX_H_
