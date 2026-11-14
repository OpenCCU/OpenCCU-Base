/**
 * @file  cursor.js
 * @brief Zugriff auf den Cursor
 **/

/**
 * Cursor-Objekt
 **/
Cursor = {
	
	NORMAL: "auto",		//< normaler Cursor
	WAIT  : "wait",		//< Sanduhr
	
	/**
	 * Setzt den Cursor-Typ.
	 * @param cursorType Cursor-Typ
	 **/
	set: function( cursorType)
	{
		document.body.style.cursor = cursorType;
	}
	
};