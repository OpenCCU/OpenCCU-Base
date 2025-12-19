/*******************************************************************************
 * tclticks.cpp
 * \brief Liefert die Anzahl der Sekunden seit Systemstart
 *
 * (c) 2009, eQ-3 Entwicklung GmbH
 *
 ******************************************************************************/

/*############################################################################*/
/*# Header                                                                   #*/
/*############################################################################*/

#include <tcl.h>
#include <time.h>

#include <sstream>


/*############################################################################*/
/*# Definitionen                                                             #*/
/*############################################################################*/

// Um herauszubekommen, ob CLOCK_MONOTONIC verf�gbar ist, musste ich zu einem
// kleinen Trick greifen:
//   * In der urspr�nglichen Toolchain wurde GCC 3.x verwendet. Dort war die
//     Konstante CLOCK_MONOTONIC bereits vorhanden, aber nicht nutzbar. 
//   * Mit der neuen Toolchain kann dann CLOCK_MONOTIC genutzt werden. Hier 
//     wird GCC 4.x verwendet.
// ==> Daher kann von der GCC-Version auf die Verf�gbarkeit von CLOCK_MONOTONIC
//     geschlossen werden.
#if __GNUC__ >= 4
#  define TCLTICKS_CLOCK CLOCK_MONOTONIC
#else
#  define TCLTICKS_CLOCK CLOCK_REALTIME
#endif

#define TCLTICKS_VERSION "1.0"

/*############################################################################*/
/*# Variablen                                                                #*/
/*############################################################################*/

static Tcl_HashTable hashTable;

extern "C" {

/*############################################################################*/
/*# Prototypen                                                               #*/
/*############################################################################*/

static int Tclticks_Seconds (ClientData, Tcl_Interp * interp, int argc, char* argv[]);


/*############################################################################*/
/*# Funktionen                                                               #*/
/*############################################################################*/

/**
 * \fn int Tclrega_Init (Tcl_Interp* interp)
 * \brief Initialisiert das Paket
 **/
int Tclticks_Init (Tcl_Interp* interp) {
	Tcl_InitHashTable(&hashTable, TCL_STRING_KEYS);
	Tcl_CreateCommand(interp, "ticks_seconds", Tclticks_Seconds, (ClientData) NULL, NULL);
	Tcl_PkgProvide(interp, "ticks", TCLTICKS_VERSION);
	Tcl_SetVar(interp, "ticks_version", TCLTICKS_VERSION, TCL_GLOBAL_ONLY);
	return TCL_OK;
}

/**
 * Liefert die Anzahl der Sekunden seit Systemstart.
 **/
static int Tclticks_Seconds(ClientData, Tcl_Interp * interp, int argc, char* argv[])
{
  struct timespec ts;
  
  if (0 == clock_gettime(TCLTICKS_CLOCK, &ts))
  {
    std::stringstream seconds;
    seconds << ts.tv_sec;
    
    Tcl_AppendResult(interp, const_cast<char*>(seconds.str().c_str()), NULL);
    return TCL_OK;
  }
  else
  {
    Tcl_AppendResult(interp, "Error getting time", NULL);
    return TCL_ERROR;
  }
}


}/* extern "C" */
