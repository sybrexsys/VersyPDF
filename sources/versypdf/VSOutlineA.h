/*********************************************************************

This file is part of the VersyPDF project.
Copyright (C) 2005 - 2016 Sybrex Systems Ltd. All rights reserved.
Authors: Vadzim Shakun , et al.

VersyPDF is free software: you can redistribute it and/or modify it 
under the terms of the GNU Affero General Public License as published 
by the Free Software Foundation, either version 3 of the License, or 
(at your option) any later version, with the addition of the following 
permission added to Section 15 as permitted in Section 7(a):
FOR ANY PART OF THE COVERED WORK IN WHICH THE COPYRIGHT IS OWNED BY 
SYBREX SYSTEMS. SYBREX SYSTEMS DISCLAIMS THE WARRANTY OF NON 
INFRINGEMENT OF THIRD PARTY RIGHTS.
                    
VersyPDF is distributed in the hope that it will be useful, 
but WITHOUT ANY WARRANTY; without even the implied warranty of 
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the 
GNU Affero General Public License for more details.
                                                                   
In accordance with Section 7(b) of the GNU Affero General Public License, 
a covered work must retain the producer line in every PDF that is created 
or manipulated using VersyPDF.

You can be released from the requirements of the license by purchasing 
a commercial license. Buying such a license is mandatory as soon as you 
develop commercial activities involving VersyPDF without disclosing 
the source code of your own applications, offering paid services to customers 
as an ASP, serving PDFs on the fly in a commerce web application, 
or shipping VersyPDF with a closed source product.

For more information, please contact Sybrex Systems at http://www.sybrex.com

----------------------------------------------------------------------
VSOutlineA.h
*********************************************************************/

#ifndef VSOutlineA_H
#define VSOutlineA_H
#ifndef __GNUC__ /* suppress gcc warning */
#pragma once
#endif /* __GNUC__ */

#include "VSTypes.h"
#include "VSActionA.h"

#ifdef __cplusplus
extern "C"{
#endif


	/********************************************************************
    Description:
        Returns root of outlines.
    Parameters:
           Doc - [ in ] Current PDF document.
    Returns:
        Handle to document outlines root.
    *********************************************************************/
    LIB_API PDFOutlineHandle PDFDocGetOutlineRoot(PDFDocHandle Doc);


	/* Description
	   Creates new outline which will be parallel for current
	   \outline.
	   Parameters
	   Doc :      [ in ] Current PDF document.
	   Outline :  [ in ] Current outline.
	   Returns
	   Handle to created outline.                             */
    LIB_API PDFOutlineHandle PDFOutlineAddNewSibling( PDFOutlineHandle Outline );


	/* Description
	   Adds new outline which will be next for current outline.
	   Parameters
	   Doc :      [ in ] Current PDF document.
	   Outline :  [ in ] Current outline.
	   Returns
	   Handle to created outline.                               */
    LIB_API PDFOutlineHandle PDFOutlineAddNewNext ( PDFOutlineHandle Outline);


	/* Description
	   Adds new outline which will be previous for current outline.
	   Parameters
	   Doc :      [ in ] Current PDF document.
	   Outline :  [ in ] Current outline.
	   Returns
	   Handle to created outline.                                   */
    LIB_API PDFOutlineHandle PDFOutlineAddNewPrev(PDFOutlineHandle Outline);


	
		
	/********************************************************************
    Description:
        Creates outline as child for current outline.
    Parameters:
           Doc        - [ in ] Current PDF document.
		   Outline  - [ in ] Current outline.
    Returns:
	    Handle to created outline.
    *********************************************************************/
	LIB_API PDFOutlineHandle PDFOutlineAddNewChild(PDFOutlineHandle Outline);


	

	/* Description
	   \Returns the first outline child.
	   Parameters
	   Doc :      [ in ] Current PDF document.
	   Outline :  [ in ] Current outline.
	   Returns
	   Handle to the first outline child.      */
	LIB_API PDFOutlineHandle PDFOutlineGetFirstChild(PDFOutlineHandle Outline);


	/* Description
	   \Returns the last outline child.
	   Parameters
	   Doc :      [ in ] Current PDF document.
	   Outline :  [ in ] Current outline.
	   Returns
	   Handle to the last outline child.       */
    LIB_API PDFOutlineHandle PDFOutlineGetLastChild(PDFOutlineHandle Outline);


	/* Description
	   \Returns following outline.
	   Parameters
	   Doc :      [ in ] Current PDF document.
	   Outline :  [ in ] Current outline.
	   Returns
	   Handle to following outline.            */
    LIB_API PDFOutlineHandle PDFOutlineGetNext(PDFOutlineHandle Outline);


	/********************************************************************
    Description:
        Returns previous outline.
    Parameters:
           Doc     - [ in ] Current PDF document.
		   Outline - [ in ] Current outline.
    Returns:
        Handle to previous outline.
    *********************************************************************/
    LIB_API PDFOutlineHandle PDFOutlineGetPrev(PDFOutlineHandle Outline);


	/* Description
	   \Returns parent outline.
	   Parameters
	   Doc :      [ in ] Current PDF document.
	   Outline :  [ in ] Current outline.
	   Returns
	   Handle to parent outline.               */
	LIB_API PDFOutlineHandle PDFOutlineGetParent(PDFOutlineHandle Outline);



	/* Description
	   Inspects current outline on children presence.
	   Parameters
	   Doc :      [ in ] Current PDF document.
	   Outline :  [ in ] Current outline.
	   Returns
	   If current outline has children - true, else - false. */
    LIB_API ppBool PDFOutlineHasChildren(PDFOutlineHandle Outline);



	/* Description
	   Calculates count of the children.
	   Parameters
	   Doc :      [ in ] Current PDF document.
	   Outline :  [ in ] Current outline.
	   Returns
	   Count of children.                      */
    LIB_API ppInt32 PDFOutlineGetCount(PDFOutlineHandle Outline);



	/* Description
	   Sets current outline on expanding.
	   Parameters
	   Doc :       [ in ] Current PDF document.
	   Outline :   [ in ] Current outline.
	   Expanded :  [ in ] Outline expand state.
	   Returns
	   None.                                    */
    LIB_API void PDFOutlineSetExpanded(PDFOutlineHandle Outline, ppBool Expanded);



	/********************************************************************
    Description:
        Sets color of outline.
    Parameters:
           Doc      - [ in ] Current PDF document.
		   Outline  - [ in ] Current outline.
		   Color    - [ in ] Outline color.
    Returns:
        None.
    *********************************************************************/
    LIB_API void PDFOutlineSetColor(PDFOutlineHandle Outline, PPDFColor Color);




	/********************************************************************
    Description:
        Sets the outline flags.
    Parameters:
           Doc      - [ in ] Current PDF document.
		   Outline  - [ in ] Current outline.
		   Flags    - [ in ] Outline flags.
    Returns:
        None.
    *********************************************************************/
    LIB_API void PDFOutlineSetFlags(PDFOutlineHandle Outline, ppInt32 Flags);



	/********************************************************************
    Description:
        Sets title of outline.
    Parameters:
           Doc      - [ in ] Current PDF document.
		   Outline  - [ in ] Current outline.
		   str      - [ in ] Outline title.  
		   len      - [ in ] Title length.
    Returns:
        None.
    *********************************************************************/
    LIB_API void PDFOutlineSetTitle(PDFOutlineHandle Outline, char *str, ppInt32 len); 



	/********************************************************************
    Description:
        Links outline to object.
    Parameters:
           Doc         - [ in ] Current PDF document.
		   Outline     - [ in ] Current outline.
           Destination - [ in ] Linked object handle. 
    Returns:
        None.
    *********************************************************************/
    LIB_API void PDFOutlineSetDestination(PDFOutlineHandle Outline,
                                  PDFDestinationHandle Destination);


	/* Description
	   \Links action to outline.
	   Parameters
	   Doc :      [ in ] Current PDF document.
	   Outline :  [ in ] Current outline.
	   Action :   [ in ] Linked action.
	   Returns
	   None.                                   */
    LIB_API void PDFOutlineSetAction(PDFOutlineHandle Outline, PDFActionHandle Action);

	/********************************************************************
	Description:
	Deletes outline from tree.
	Parameters:
	Doc      - [ in ] Current PDF document.
	Outline  - [ in ] Outline for delete.
	Returns:
	None.
	*********************************************************************/

	LIB_API void PDFOutlineDelete(PDFOutlineHandle Outline);


#ifdef __cplusplus
}
#endif

#endif /* #ifndef VSOutlineA_H */
