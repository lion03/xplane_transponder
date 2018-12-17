// Downloaded from https://developer.x-plane.com/code-sample/motionplatformdata/


/*
Plugin to show how to derive motion platform data from our datarefs
Thanks to Austin for allowing us to use the original Xplane conversion code.

Version 1.0.0.1			Intitial Sandy Barbour - 05/08/2007
*/

#include <stdio.h>
#include <string.h>
#include <hidapi.h>
#include <stdint.h>

#include "XPLMDisplay.h"
#include "XPLMGraphics.h"
#include "XPLMProcessing.h"
#include "XPLMDataAccess.h"

// Globals.
// Use MPD_ as a prefix for the global variables

// Used to store data for display
char MPD_Buffer[6][80];
// Used to store calculated motion data
float MPD_MotionData[6];

// Window ID
XPLMWindowID MPD_Window = NULL;

// Datarefs
XPLMDataRef	MPD_DR_groundspeed = NULL;
XPLMDataRef	MPD_DR_fnrml_prop = NULL;
XPLMDataRef	MPD_DR_fside_prop = NULL;
XPLMDataRef	MPD_DR_faxil_prop = NULL;
XPLMDataRef	MPD_DR_fnrml_aero = NULL;
XPLMDataRef	MPD_DR_fside_aero = NULL;
XPLMDataRef	MPD_DR_faxil_aero = NULL;
XPLMDataRef	MPD_DR_fnrml_gear = NULL;
XPLMDataRef	MPD_DR_fside_gear = NULL;
XPLMDataRef	MPD_DR_faxil_gear = NULL;
XPLMDataRef	MPD_DR_m_total = NULL;
XPLMDataRef	MPD_DR_the = NULL;
XPLMDataRef	MPD_DR_psi = NULL;
XPLMDataRef	MPD_DR_phi = NULL;

//sim/cockpit/radios/transponder_code
XPLMDataRef XPDR_DR_itransponder_code = NULL;
//sim/cockpit/radios/transponder_light
XPLMDataRef XPDR_DR_itransponder_light = NULL;
//sim/cockpit/radios/transponder_id
XPLMDataRef XPDR_DR_itransponder_id = NULL;
//sim/cockpit/radios/transponder_mode
XPLMDataRef XPDR_DR_itransponder_mode = NULL;
//sim/cockpit/electrical/avionics_on
XPLMDataRef XPDR_DR_iavionics_on = NULL;

hid_device *handle = NULL;



typedef enum transponder_mode
{
	eMode_off = 0x0,
	eMode_stdby = 0x1,
	eMode_on = 0x2, 
	eMode_test = 0x3
}transponder_mode_t ;


#pragma pack(push)  /* push current alignment to stack */
#pragma pack(1)     /* set alignment to 1 byte boundary */

typedef union transponder_code
{
	uint16_t raw;
	uint16_t code : 12;
	uint8_t padding : 4;

}transponder_code_t;


typedef struct transponder_state
{
	uint8_t report_id;
	uint8_t mode : 2;
	uint8_t light : 1;
	uint8_t id : 1;
	uint8_t avionics_on : 1;
	uint8_t padding : 3;
	uint16_t xpdr_code;
	

}transponder_state_t;

typedef struct MyStruct
{
	uint8_t x : 1;
	uint8_t y : 7;

}MyStruct_t ;


static const uint32_t x = sizeof(transponder_state);
#pragma pack(pop)  /* push current alignment to stack */
static const uint32_t y = sizeof(uint8_t);
//---------------------------------------------------------------------------
// Function prototypes

float MotionPlatformDataLoopCB(float elapsedMe, float elapsedSim, int counter, void * refcon);

void MotionPlatformDataDrawWindowCallback(
                                   XPLMWindowID         inWindowID,    
                                   void *               inRefcon);    

void MotionPlatformDataHandleKeyCallback(
                                   XPLMWindowID         inWindowID,    
                                   char                 inKey,    
                                   XPLMKeyFlags         inFlags,    
                                   char                 inVirtualKey,    
                                   void *               inRefcon,    
                                   int                  losingFocus);    

int MotionPlatformDataHandleMouseClickCallback(
                                   XPLMWindowID         inWindowID,    
                                   int                  x,    
                                   int                  y,    
                                   XPLMMouseStatus      inMouse,    
                                   void *               inRefcon);    

float MPD_fallout(float data, float low, float high);
float MPD_fltlim(float data, float min, float max);
float MPD_fltmax2 (float x1,const float x2);
void MPD_CalculateMotionData(void);

//---------------------------------------------------------------------------
// SDK Mandatory Callbacks

PLUGIN_API int XPluginStart(
						char *		outName,
						char *		outSig,
						char *		outDesc)
{
	strcpy(outName, "MotionPlatformData");
	strcpy(outSig, "xplanesdk.examples.motiondplatformdata");
	strcpy(outDesc, "A plug-in that derives motion platform data from datarefs.");

	MPD_Window = XPLMCreateWindow(
		                      50, 600, 200, 500,								/* Area of the window. */
                              1,												/* Start visible. */
                              MotionPlatformDataDrawWindowCallback,			/* Callbacks */
                              MotionPlatformDataHandleKeyCallback,
                              MotionPlatformDataHandleMouseClickCallback,
                              NULL);											/* Refcon - not used. */

	XPLMRegisterFlightLoopCallback(MotionPlatformDataLoopCB, -1.0, NULL);
	
	MPD_DR_groundspeed = XPLMFindDataRef("sim/flightmodel/position/groundspeed");
	MPD_DR_fnrml_prop = XPLMFindDataRef("sim/flightmodel/forces/fnrml_prop");
	MPD_DR_fside_prop = XPLMFindDataRef("sim/flightmodel/forces/fside_prop");
	MPD_DR_faxil_prop = XPLMFindDataRef("sim/flightmodel/forces/faxil_prop");
	MPD_DR_fnrml_aero = XPLMFindDataRef("sim/flightmodel/forces/fnrml_aero");
	MPD_DR_fside_aero = XPLMFindDataRef("sim/flightmodel/forces/fside_aero");
	MPD_DR_faxil_aero = XPLMFindDataRef("sim/flightmodel/forces/faxil_aero");
	MPD_DR_fnrml_gear = XPLMFindDataRef("sim/flightmodel/forces/fnrml_gear");
	MPD_DR_fside_gear = XPLMFindDataRef("sim/flightmodel/forces/fside_gear");
	MPD_DR_faxil_gear = XPLMFindDataRef("sim/flightmodel/forces/faxil_gear");
	MPD_DR_m_total = XPLMFindDataRef("sim/flightmodel/weight/m_total");
	MPD_DR_the = XPLMFindDataRef("sim/flightmodel/position/theta");
	MPD_DR_psi = XPLMFindDataRef("sim/flightmodel/position/psi");
	MPD_DR_phi = XPLMFindDataRef("sim/flightmodel/position/phi");

	//sim/cockpit/radios/transponder_code
	XPDR_DR_itransponder_code = XPLMFindDataRef("sim/cockpit/radios/transponder_code");
	//sim/cockpit/radios/transponder_light
	XPDR_DR_itransponder_light = XPLMFindDataRef("sim/cockpit/radios/transponder_light");
	//sim/cockpit/radios/transponder_id
	XPDR_DR_itransponder_id = XPLMFindDataRef("sim/cockpit/radios/transponder_id");
	//sim/cockpit/radios/transponder_mode
	XPDR_DR_itransponder_mode = XPLMFindDataRef("sim/cockpit/radios/transponder_mode");
	//sim/cockpit/electrical/avionics_on
	XPLMDataRef XPDR_DR_iavionics_on = XPLMFindDataRef("sim/cockpit/electrical/avionics_on");


	memset(MPD_Buffer, 0, sizeof(MPD_Buffer));

	return 1;
}

//---------------------------------------------------------------------------

PLUGIN_API void	XPluginStop(void)
{
    XPLMDestroyWindow(MPD_Window);
	XPLMUnregisterFlightLoopCallback(MotionPlatformDataLoopCB, NULL);
}

//---------------------------------------------------------------------------

PLUGIN_API int XPluginEnable(void)
{
	return 1;
}

//---------------------------------------------------------------------------

PLUGIN_API void XPluginDisable(void)
{
}

//---------------------------------------------------------------------------

PLUGIN_API void XPluginReceiveMessage(XPLMPluginID inFrom, int inMsg, void * inParam)
{
}


//---------------------------------------------------------------------------
// Mandatory callback for SDK 2D Window
// Used to display the data to the screen

void MotionPlatformDataDrawWindowCallback(
                                   XPLMWindowID         inWindowID,    
                                   void *               inRefcon)
{

	float		rgb [] = { 1.0, 1.0, 1.0 };
	int			l, t, r, b;

	XPLMGetWindowGeometry(inWindowID, &l, &t, &r, &b);
	XPLMDrawTranslucentDarkBox(l, t, r, b);

	for (int i=0; i<6; i++)
		XPLMDrawString(rgb, l+10, (t-20) - (10*i), MPD_Buffer[i], NULL, xplmFont_Basic);
}                                   

//---------------------------------------------------------------------------
// Mandatory callback for SDK 2D Window
// Not used in this plugin

void MotionPlatformDataHandleKeyCallback(
                                   XPLMWindowID         inWindowID,    
                                   char                 inKey,    
                                   XPLMKeyFlags         inFlags,    
                                   char                 inVirtualKey,    
                                   void *               inRefcon,    
                                   int                  losingFocus)
{
}                                   

//---------------------------------------------------------------------------
// Mandatory callback for SDK 2D Window
// Not used in this plugin

int MotionPlatformDataHandleMouseClickCallback(
                                   XPLMWindowID         inWindowID,    
                                   int                  x,    
                                   int                  y,    
                                   XPLMMouseStatus      inMouse,    
                                   void *               inRefcon)
{
	return 1;
}                                      

//---------------------------------------------------------------------------
// FlightLoop callback to calculate motion data and store it in our buffers

float MotionPlatformDataLoopCB(float elapsedMe, float elapsedSim, int counter, void * refcon)
{
	MPD_CalculateMotionData();
	transponder_state_t xpdr_state;
	xpdr_state.mode = XPLMGetDatai(XPDR_DR_itransponder_mode) & 0x3;
	xpdr_state.light = XPLMGetDatai(XPDR_DR_itransponder_light) & 0x1;
	xpdr_state.id = XPLMGetDatai(XPDR_DR_itransponder_id) & 0x1;
	xpdr_state.avionics_on = XPLMGetDatai(XPDR_DR_iavionics_on) & 0x1;
	int xpdr_code = XPLMGetDatai(XPDR_DR_itransponder_code);
	xpdr_state.xpdr_code = xpdr_code & 0x1FFF;

	sprintf(MPD_Buffer[0], "code0 = %d", xpdr_state.xpdr_code);
	sprintf(MPD_Buffer[4], "code = %d", xpdr_code);
	//sprintf(MPD_Buffer[5], "a_axil = %f", MPD_MotionData[5]);

	return (float)0.1;
}

//---------------------------------------------------------------------------
// Original function used in the Xplane code.

float MPD_fallout(float data, float low, float high)
{
	if (data < low) return data;
	if (data > high) return data;
	if (data < ((low + high) * 0.5)) return low;
    return high;
}

//---------------------------------------------------------------------------
// Original function used in the Xplane code.

float MPD_fltlim(float data, float min, float max)
{
	if (data < min) return min;
	if (data > max) return max;
	return data;
}

//---------------------------------------------------------------------------
// Original function used in the Xplane code.

float MPD_fltmax2 (float x1,const float x2)
{
	return (x1 > x2) ? x1 : x2;
}

//---------------------------------------------------------------------------
// This is original Xplane code converted to use 
// our datarefs instead of the Xplane variables

void MPD_CalculateMotionData(void)
{
	float groundspeed = XPLMGetDataf(MPD_DR_groundspeed);
	float fnrml_prop = XPLMGetDataf(MPD_DR_fnrml_prop);
	float fside_prop = XPLMGetDataf(MPD_DR_fside_prop);
	float faxil_prop = XPLMGetDataf(MPD_DR_faxil_prop);
	float fnrml_aero = XPLMGetDataf(MPD_DR_fnrml_aero);
	float fside_aero = XPLMGetDataf(MPD_DR_fside_aero);
	float faxil_aero = XPLMGetDataf(MPD_DR_faxil_aero);
	float fnrml_gear = XPLMGetDataf(MPD_DR_fnrml_gear);
	float fside_gear = XPLMGetDataf(MPD_DR_fside_gear);
	float faxil_gear = XPLMGetDataf(MPD_DR_faxil_gear);
	float m_total = XPLMGetDataf(MPD_DR_m_total);
	float the = XPLMGetDataf(MPD_DR_the);
	float psi = XPLMGetDataf(MPD_DR_psi);
	float phi = XPLMGetDataf(MPD_DR_phi);

	float ratio = MPD_fltlim(groundspeed*0.2,0.0,1.0);
	float a_nrml= MPD_fallout(fnrml_prop+fnrml_aero+fnrml_gear,-0.1,0.1)/MPD_fltmax2(m_total,1.0);
	float a_side= (fside_prop+fside_aero+fside_gear)/MPD_fltmax2(m_total,1.0)*ratio;
	float a_axil= (faxil_prop+faxil_aero+faxil_gear)/MPD_fltmax2(m_total,1.0)*ratio;

	// Store the results in an array so that we can easily display it.
	MPD_MotionData[0] = the;
	MPD_MotionData[1] = psi;
	MPD_MotionData[2] = phi;
	MPD_MotionData[3] = a_side;
	MPD_MotionData[4] = a_nrml;
	MPD_MotionData[5] = a_axil;
}

//---------------------------------------------------------------------------

