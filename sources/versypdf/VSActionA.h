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
VSActionA.h
*********************************************************************/

#ifndef VSActionA_H
#define VSActionA_H
#ifndef __GNUC__ /* suppress gcc warning */
#pragma once
#endif /* __GNUC__ */

#include "VSTypes.h"
#include "VSThreadA.h"

#ifdef __cplusplus
extern "C"{
#endif



    /* PDF explicit destination types */
_EXP

	typedef enum PDFExplicitDestType {
        edtXYZ,                                 /*  Display the page, with the coordinates (<i>left</i>, <i>top</i>) positioned
                                                    at the top-left corner of the window and the contents of the page magnified
                                                    by the factor <i>zoom</i>. A null value for any of the parameters <i>left</i>, <i>top</i>, or
                                                    <i>zoom</i> specifies that the current value of that parameter is to be retained unchanged. */
        edtFit,                                 /*  Display the page, with its contents magnified just enough
                                                    to fit the entire page within the window both horizontally and vertically. If
                                                    the required horizontal and vertical magnification factors are different, use
                                                    the smaller of the two, centering the page within the window in the other dimension. */   
        edtFitH,                                /*  Display the page, with the vertical coordinate top positioned
                                                    at the <i>top</i> edge of the window and the contents of the page magnified
                                                    just enough to fit the entire width of the page within the window.                   */
        edtFitV,                                /*  Display the page, with the horizontal coordinate left positioned
                                                    at the <i>left</i> edge of the window and the contents of the page magnified
                                                    just enough to fit the entire height of the page within the window.                  */ 
        edtFitR,                                /*  Display the page, with its contents magnified just enough to fit the rectangle 
                                                    specified by the coordinates <i>left</i>, <i>bottom</i>, <i>right</i>, and <i>top</i> entirely 
                                                    within the window both horizontally and vertically. If the required horizontal and 
                                                    vertical magnification factors are different, use the smaller of the two, centering 
                                                    the rectangle within the window in the other dimension.                              */
        edtFitB,                                /*  Display the page, with its contents magnified just enough to fit its bounding box 
                                                    entirely within the window both horizontally and vertically. If the required horizontal 
                                                    and vertical magnification factors are different, use the smaller of the two, centering 
                                                    the bounding box within the window in the other dimension. */
        edtFitBH,                               /*  Display the page, with the vertical coordinate top positioned at the <i>top</i>
                                                    edge of the window and the contents of the page magnified just enough to fit the entire 
                                                    width of its bounding box within the window.                                         */
        edtFitBV                                /*  Display the page, with the horizontal coordinate left positioned at the <i>left</i> edge of 
                                                    the window and the contents of the page magnified just enough to fit the entire height 
                                                    of its bounding box within the window.                                               */   
    } PDFExplicitDestType;


    /* This struct for specifying a destination explicitly in a PDF file */
    typedef struct  {
        PDFExplicitDestType Type;           /*  Explicit destination type                           */
        ppUns32             Page;           /*  Index of the page in PDF document (Begin from 0 )   */
        ppReal              a;              /*  Value depended from type of the destination 
                                                left for edtXYZ, top for edtFitH and edtFitBH, left for edtFitV,edtFitBV, and edtFitR */
                                            
        ppReal              b;              /*  Value depended from type of the destination          
                                                top for edtXYZ, bottom for edtFitR                  */
        ppReal              c;              /*  Value depended from type of the destination          
                                                zoom for edtXYZ, right for edtFitR                  */
        ppReal              d;              /*  Value depended from type of the destination          
                                                top for edtFitR                                     */
    } PDFExplicitDest, *PPDFExplicitDest;


    /* Type of the PDF destination */
    typedef enum PDFDestinationType {
        pdfdtExplicit,                  /* Directly via explicit destination    */
        pdfdtNamed                      /* Indirectly via name destination      */
    } PDFDestinationType;

    /* Description
       Creates new destination name in name table and assigns to it
       explicit destination
       Parameters
       Doc :          [in] PDF Document handle
       String :       [in] Specifies the name which will be appended to
                      name table.
       Length :       [in] Length of the name
       Destination :  [in] Explicit destination which will be respected
                      to this name
       Returns
         None.        
   */
    LIB_API void PDFDestinationNamedNew ( PDFDocHandle Doc, char *String, ppInt32 Length, PDFExplicitDest Destination);

    /* Description
       Creates new destination handle from string.
       Parameters
        Doc :          [in] PDF Document handle
        String :       [in] Specifies the name for which will created destination handle.
        Length :       [in] Length of the name     
		IsIndirect:	   [in] Specifies store this destination in memory or append to PDFDocument.
       Returns
         Destination Handle.                                                            
	   Note
		 Memory Leak will present if IsIndirect is false and destination not appended to any Action
	*/
    LIB_API PDFDestinationHandle PDFDestinationFromString( PDFDocHandle Doc, char *String, ppInt32 Length, ppBool IsIndirect );

    /* Description
        Creates new destination handle from string.
       Parameters
        Doc :          [in] PDF Document handle
        Dest:          [in] Explicit destination 
        OtherDocument: [in] Specifies will destination is this document or other.
		IsIndirect:	   [in] Specifies store this destination in memory or append to PDFDocument.
       Returns
        Destination Handle.   
	   Note
		Memory Leak will present if IsIndirect is false and destination not appended to any Action
		*/
    LIB_API PDFDestinationHandle PDFDestinationFromExplit(PDFDocHandle Doc, PDFExplicitDest Dest, ppBool OtherDocument, ppBool IsIndirect);



    
    /* Description
       This function sets action which will be executed after
       current action.
       Parameters
       Action :  [in] Handle of the action object.
       Next :    [in] Handle of the next action object.
       Returns
       None.                                                  */
    LIB_API void PDFActionSetNext( PDFActionHandle Action, PDFActionHandle Next);




    /* GoTo Action */
    /* Description
       Creates new "GoTo" action and sets destination to explicit
       destination.
       Parameters
       Doc :          [in] PDF Document handle
       Destination :  [in] Destination handle.
       Returns
       The return value is a handle to a PDF action.              */
    LIB_API PDFActionHandle PDFActionNewGoToDestination(PDFDocHandle Doc, PDFDestinationHandle Destination);
  
    


    /* GoToRemote Action */



    /* Description
       Creates new "GoToRemote" action and sets destination to name
       destination.
       Parameters
       Doc :             [in] PDF Document handle
       FileName :        [in] Filename of the PDF document which need open
       FileNameLength :  [in] Length of the filename.
       Destination:      [in] Destination Handle.
       InNewWindow :     [in] Specifying whether to open the destination
                         document in a new window. If this flag is false,
                         the destination document will replace the current
                         document in the same window.
       Returns
       The return value is a handle to a PDF action.                       */
    LIB_API PDFActionHandle PDFActionNewGoToRemote(PDFDocHandle Doc, char *FileName,
        ppInt32 FileNameLength, PDFDestinationHandle Destination, ppBool InNewWindow);

    
    /* Launch Action */



    /********************************************************************
    Description:
        Creates new "Launch" action. 
    Parameters:
        Doc         - [in]  PDF Document handle
        Launch      - [in]  Point to a <b>PDFLaunch</b> structure that defines 
                            the characteristics of the launch action. 
        InNewWindow - [in]  Specifying whether to open the destination document
                            in a new window. If this flag is false, the destination document will
                            replace the current document in the same window.
    Returns: 
        The return value is a handle to a PDF action.
    *********************************************************************/
    LIB_API PDFActionHandle PDFActionNewLaunch(PDFDocHandle Doc, char * FileName, char * DefaultDir, 
		char * Operation, char * Params, ppBool InNewWindow);




    /* Thread Action */

    /* This enumeration defines type of the store thread info in
       thread action                                             */

    _EXP

    typedef enum PDFThreadActionType{
        taThreadHandle,                 /* Destination is stored in PDFThreadHandle */
        taThreadIndex,                  /* Destination is stored in index of the threads in PDF
                                           document. The first thread in the document has index 0. */
        taThreadTitle                   /* Destination is stored in the title of the thread. */
    } PDFThreadActionType;

    _EXP

    typedef enum PDFBeadActionType{
        taBeadHandle,                   /* Destination stored in PDFBeadHandle */
        taBeadIndex                     /* Destination stored in index of the bead within its thread. 
                                            The first bead in a thread has index 0. */
    } PDFBeadActionType;

    /* This structure used by PDFActionNewThread to create new
       Thread action                                           */
    typedef struct PDFThreadActionParam {
        PDFThreadActionType DestThreadType; /* Depends type of the thread */
        union {
            PDFThreadHandle ThreadHandle;
            ppInt32         ThreadIndex;
			struct {
				char*		String;
				ppUns32		Length;
			} ThreadTitle;
        } DestThread;                       /* The desired destination thread */
        PDFBeadActionType   DestBeadType;   /* Depends type of the bead */
        union {
            PDFBeadHandle   BeadHandle;
            ppInt32         BeadIndex;
        } DestBead;                         /* The desired bead in the destination thread */
    }PDFThreadActionParam, *PDFThreadActionParamP;


    /* Description
       Creates new "Thread" action.
       Parameters
       Doc :             [in] PDF Document handle
       Launch :          [in] Point to a <b>PDFThreadActionParam</b>
                         structure that defines the characteristics of the
                         thread action.
       FileName :        [in] Filename of the PDF documents where destination
                         thread may be the desired. 
       FileNameLength :  [in] Length of the filename.
       Note
       Destination thread is in current PDF document if filename is
       NULL. In other case PDFBeadHandle or PDFThreadHandle
       impossible to use in <b>PDFThreadActionParam</b> structure.
       Returns
       The return value is a handle to a PDF action.                          */
    LIB_API PDFActionHandle PDFActionNewThread(PDFDocHandle Doc, char *FileName, ppInt32 FileNameLength,
                                       PDFThreadActionParam Thread);
    
    
    
    /* URI Action */

    /********************************************************************
    Description:
        Creates new "URI" action. 
    Parameters:
        Doc         - [in]  PDF Document handle.
        URI         - [in]  The uniform resource identifier to resolve, encoded in 7-bit ASCII.
        Length      - [in]  Length of the URI.
        IsMap       - [in]  A flag specifying whether to track the mouse 
                            position when the URI is resolved.
    Returns: 
        The return value is a handle to a PDF action.
    *********************************************************************/
    LIB_API PDFActionHandle PDFActionNewURI(PDFDocHandle Doc, char *URI, ppInt32 Length, ppBool IsMap);






    /* Hide Action */


    /* Description
       Creates new "Hide" action.
       Parameters
       Doc :     [in] PDF Document handle.
       IsHide :  [in] Type of the action. Execution of this action will
                 hide selected annotations if value sets in "true". In
                 other case selected annotations will be shown.
       Returns
       The return value is a handle to a PDF action.                    */
    LIB_API PDFActionHandle PDFActionNewHide(PDFDocHandle Doc, ppBool IsHide);




    /* Description
       Appends annotation to list in the hide action.
       Parameters
       Doc :         [in] PDF Document handle.
       Action :      [in] Handle of the PDF action.
       Annotation :  [in] Handle of the annotation which is needed to
                     be appended to list.
       Returns
       None.
       Note
       Operation will be performed for all annotations in the PDF
       document if any annotation for this action is not selected.    */
    LIB_API void PDFActionHideAddAnnotation(PDFActionHandle Action,
                                    PDFAnnotationHandle Annotation);




    /* Description
       Appends annotation to list in the hide action.
       Parameters
       Doc :             [in] PDF Document handle.
       Action :          [in] Handle of the PDF action.
       AnnotationName :  [in] Name of the annotation which is needed to be
                         appended to the list.
       Length :          [in] Length of the name.
       Returns
       None.
       Note
       Operation will be performed for all annotations in the PDF
       document if any annotation for this action is not selected.         */
    LIB_API void PDFActionHideAddAnnotationName(PDFActionHandle Action,
                                        char *AnnotationName, ppInt32 Length);

  
    /* Named Action */

    /* This enumeration defines type of the operation for named
       action                                                   */
    _EXP
    typedef enum {
        naNextPage,     /* Go to the next page of the document.     */
        naPrevPage,     /* Go to the previous page of the document. */  
        naFirstPage,    /* Go to the first page of the document.    */
        naLastPage      /* Go to the last page of the document.     */
    } PDFNamedActionType;


    /********************************************************************
    Description:
        Creates new "Named" action. 
    Parameters:
        Doc         - [in]  PDF Document handle.
        NamedType   - [in]  Operation for the named action.
    Returns: 
        The return value is a handle to a PDF action.
    *********************************************************************/
    LIB_API PDFActionHandle PDFActionNewNamed(PDFDocHandle Doc, PDFNamedActionType NamedType);




    /* Submit-Form Action */
#define PDF_SUBMIT_FORM_FLAG_EXCLUDE                        1
#define PDF_SUBMIT_FORM_FLAG_INCLUDE_NO_VALUE_FIELDS        2
#define PDF_SUBMIT_FORM_FLAG_EXPORT_FORMAT                  4
#define PDF_SUBMIT_FORM_FLAG_GET_METHOD                     8
#define PDF_SUBMIT_FORM_FLAG_SUBMIT_COORDINATES             16
#define PDF_SUBMIT_FORM_FLAG_XML                            32
#define PDF_SUBMIT_FORM_FLAG_INCLUDE_ANNOTATIONS            128
#define PDF_SUBMIT_FORM_FLAG_SUBMIT_PDF                     256



    /********************************************************************
    Description:
        Creates new "SubmitForm" action. 
    Parameters:
        Doc         - [in]  PDF Document handle.
        URI         - [in]  A URL file specification giving the uniform resource locater of the script
                            at the Web server that will process the submission.
        Length      - [in]  Length of the URI string
        Flag        - [in]  A set of flags specifying various characteristics
                            of the action.
                            
    Returns: 
        The return value is a handle to a PDF action.
    Note: 
        Below you can find flag meaning table:
        <table>
        Flag                                            Meaning
        ----                                            -------
        PDF_SUBMIT_FORM_FLAG_EXCLUDE                    The list of the acroform objects exclude from the submission.
        PDF_SUBMIT_FORM_FLAG_INCLUDE_NO_VALUE_FIELDS    All acroform object will included in submission ( With empty values too )
        PDF_SUBMIT_FORM_FLAG_EXPORT_FORMAT              If this flag set, export will execute in HTML form format, else in FDF format
        PDF_SUBMIT_FORM_FLAG_GET_METHOD                 If this flag set, field names and values are submitted using an HTTP GET request; if clear, they are submitted using a POST request.
        PDF_SUBMIT_FORM_FLAG_SUBMIT_COORDINATES         If set, the coordinates of the mouse click that caused the submitform action are transmitted as part of the form data. The coordinate values are relative to the upper-left corner of the acroform object rectangle.
        PDF_SUBMIT_FORM_FLAG_XML                        If set, field names and values are submitted in XML format; if clear, they are submitted in HTML Form format or Forms Data Format (FDF), according to the value of the PDF_SUBMIT_FORM_FLAG_EXPORT_FORMAT flag.
        PDF_SUBMIT_FORM_FLAG_SUBMIT_PDF                 If set, the document is submitted in PDF format, using the MIME content type application/pdf (described in Internet RFC 2045, Multipurpose Internet Mail Extensions (MIME), Part One: Format of Internet Message Bodies; see the Bibliography). If this flag is set, all other flags are ignored except PDF_SUBMIT_FORM_FLAG_GET_METHOD.
        </table>

    *********************************************************************/
    LIB_API PDFActionHandle PDFActionNewSubmitForm(PDFDocHandle Doc, char *URI, ppInt32 Length,
        ppInt32 Flags);


    /* Description
       Appends annotation to list in the submitform action.
       Parameters
       Doc :         [in] PDF Document handle.
       Action :      [in] Handle of the PDF action.
       Annotation :  [in] Handle of the annotation which need append
                     to list.
       Returns
       None.
       Note
       Operation will be performed for all acroform object in the
       PDF document (flag PDF_SUBMIT_FORM_FLAG_EXCLUDE not used ) if
       its not selected any annotation for this action.              */
    LIB_API void PDFActionSubmitFormAddAnnotation(PDFActionHandle Action,
        PDFAnnotationHandle Annotation);



    /* Description
       Appends annotation to list in the submitform action.
       Parameters
       Doc :             [in] PDF Document handle.
       Action :          [in] Handle of the PDF action.
       AnnotationName :  [in] Name of the annotation which is needed to be
                         appended to the list.
       Length :          [in] Length of the name.
       Returns
       None.
       Note
       Operation will be performed for all acroform object in the
       PDF document (flag PDF_SUBMIT_FORM_FLAG_EXCLUDE not used ) if
       its not selected any annotation for this action.                    */
    LIB_API void PDFActionSubmitFormAddAnnotationName(PDFActionHandle Action,
        char *AnnotationName, ppInt32 Length);

    
/* Reset-Form Action */


    /* Description
       Creates new "Reset" action.
       Parameters
       Doc :      [in] PDF Document handle.
       Exclude :  [in] If false, the list specifies which fields to reset.
                  If true, the list of the acroform objects informs which
                  fields to be excluded from resetting; all fields in the
                  document’s interactive form are reset excepting those
                  listed.
       Returns
       The return value is a handle to a PDF action.                       */
    LIB_API PDFActionHandle PDFActionNewResetForm(PDFDocHandle Doc, ppBool Exclude);



    /* Description
       Appends annotation to list in the resetform action.
       Parameters
       Doc :         [in] PDF Document handle.
       Action :      [in] Handle of the PDF action.
       Annotation :  [in] Handle of the annotation which is needed to
                     be appended to list.
       Returns
       None.
       Note
       Reset action will be performed for all annotations in the PDF
       document if its not selected any annotation for this action.   */
    LIB_API void PDFActionResetFormAddAnnotation(PDFActionHandle Action,
        PDFAnnotationHandle Annotation);



    /* Description
       Appends annotation to list in the resetform action.
       Parameters
       Doc :             [in] PDF Document handle.
       Action :          [in] Handle of the PDF action.
       AnnotationName :  [in] Name of the annotation which is needed to be
                         appended to the list.
       Length :          [in] Length of the name.
       Returns
       None.
       Note
       Reset action will be performed for all annotations in the PDF
       document if its not selected any annotation for this action.        */
    LIB_API void PDFActionResetFormAddAnnotationName(PDFActionHandle Action,
        char *AnnotationName, ppInt32 Length);


    /* Import Data Action */


    /********************************************************************
    Description:
        Creates new "Import Data" action. 
    Parameters:
        Doc         - [in]  PDF Document handle.
        FileName    - [in]  The FDF filename from which to import the data.
        Length      - [in]  The length of the filename.
    Returns: 
        The return value is a handle to a PDF action.
    *********************************************************************/
    LIB_API PDFActionHandle PDFActionNewImportData(PDFDocHandle Doc, char *FileName, ppInt32 Length);



    /* JavaScript Action */

    /* Description
       Creates new "JavaScript" action from string.
       Parameters
       Doc :         [in] PDF Document handle
       JavaScript :  [in] JavaScript string which will be executed
       Length :      [in] Length of javascript string.
       Returns
       The return value is a handle to a PDF action.               */
    LIB_API PDFActionHandle PDFActionNewJavaScript(PDFDocHandle Doc, char *JavaScript, ppInt32 Length);



    /* Description
       Creates new "JavaScript" action from CosStream where this
       javascript is stored.
       Parameters
       Doc :         [in] PDF Document handle
       JavaScript :  [in] Cos Stream where this JavaScript is stored
       Returns
       The return value is a handle to a PDF action.                 */
    LIB_API PDFActionHandle PDFActionNewJavaScriptStream(PDFDocHandle Doc, PDFCosHandle JavaScript);


#ifdef __cplusplus
}
#endif

#endif /* #ifndef VSActionA_H */

