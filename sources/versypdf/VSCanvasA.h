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
VSCanvasA.h
*********************************************************************/

#ifndef VSCanvasA_H
#define VSCanvasA_H
#ifndef __GNUC__ /* suppress gcc warning */
#pragma once
#endif /* __GNUC__ */

#include "VSTypes.h"

#ifdef WINDOWS_PLATFORM
#include <windows.h>
#include <WinGDI.h>
#endif

#ifdef __cplusplus
extern "C"{
#endif
    /* */
    typedef enum  {
        hjLeft,             /* */
        hjCenter,           /* */
        hjRight             /* */
    } TPDFHorJust;
    /* */
    typedef enum {
        vjTop,              /* */
        vjCenter,           /* */
        vjBottom            /* */
    } TPDFVertJust;   


    /*  The line join style specifies the shape to be used at the corners 
        of paths that are stroked. */ 
    _EXP
    typedef enum {
        ljMiter,            /*  The outer edges of the strokes for the two segments are extended until 
                                they meet at an angle, as in a picture frame. If the segments meet at too
                                sharp an angle, a bevel join is used instead. */
        ljRound,            /*  A circle with a diameter equal to the line width is drawn around the point 
                                where the two segments meet and is filled in, producing a rounded corner. */
        ljBevel             /*  The two segments are finished with butt caps and the resulting notch beyond 
                                the ends of the segments is filled with a triangle */
    } TPDFLineJoin;



    /* The line cap style specifies the shape to be used at the ends
       \of opened subpaths (and dashes, if any) when they are
       stroked.                                                      */
    _EXP
    typedef enum {
        lcButtEnd,          /*  The stroke is squared off at the endpoint of the path. There is no projection 
                                beyond the end of the path. */
        lcRound,            /*  A semicircular arc with a diameter equal to the line width is drawn around 
                                the endpoint and filled in. */
        lcProjectingSquare  /*  The stroke continues beyond the endpoint of the path for a distance equal to 
                                half the line width and is then squared off. */
    } TPDFLineCap;

	/* Some misc */



    /********************************************************************
    Description: 
        Creates a TPDFColor structure from triple of the values.
    Parameters:
        r   - [in] Specifies the intensity of the red color.
        g   - [in] Specifies the intensity of the green color.
        b   - [in] Specifies the intensity of the blue color.
    Returns: 
        The return value is the resultant RGB color. 
    Remarks
        The intensity for each argument is in the range 0 through 1. 
        If all three intensities are zero, the result is black. 
        If all three intensities are 1, the result is white. 
    *********************************************************************/
	LIB_API TPDFColor ULRGBToColor(ppReal r, ppReal g, ppReal b);



    /********************************************************************
    Description: 
        Creates a TPDFColor structure from intensity of the gray.
    Parameters:
        g   - [in] Specifies the intensity of the gray color.
    Returns: 
        The return value is the resultant gray color. 
    Remarks
        The intensity for argument is in the range 0 through 1. 
        If intensity are zero, the result is black. 
        If intensity are 1, the result is white. 
    *********************************************************************/
	LIB_API TPDFColor ULGrayToColor(ppReal g);



    /********************************************************************
    Description: 
        Creates a TPDFColor structure from intensity of the CMYK.
    Parameters:
        c   - [in] Specifies the intensity of the cyan color.
        m   - [in] Specifies the intensity of the magenta color.
        y   - [in] Specifies the intensity of the yellow color.
        k   - [in] Specifies the intensity of the black color.
    Returns: 
        The return value is the resultant CMYK color. 
    Remarks
        The intensity for each argument is in the range 0 through 1. 
        If all four intensities are zero, the result is white. 
        If all four intensities are 1, the result is black. 
    *********************************************************************/
	LIB_API TPDFColor ULCMYKToColor(ppReal c, ppReal m, ppReal y, ppReal k);



    /* Description
       Disposes the instance of the PaintBox and packs content.
       Parameters
       PaintBox :  [in] PaintBox object will be disposed.
       Pack :      [in] Indicated whether content of the PaintBox will
                   be packed.
       Returns
       None.                                                           */
    LIB_API void PBXClose(PBXHandle PaintBox, ppBool Pack);



    /*Path operations*/



    /********************************************************************
    Description: 
        Clears the current path in the PaintBox. Current point becomes undefined.
    Parameters:
        PaintBox    - [in]  PaintBox object handle
    Returns: 
        None.
    *********************************************************************/
    LIB_API void PBXNewPath(PBXHandle PaintBox);



    /* Description
       This closes a path by connecting the first and the last point
       in the path currently constructed. Calling of this procedure
       is often needed to avoid a notch in a stroked path, and to
       make "line join" work correctly in joining the first and the
       last points.
       Parameters
       PaintBox :  [in] PaintBox object handle
       Returns
       None.                                                         */
    LIB_API void PBXClosePath(PBXHandle PaintBox);
    
    

    /* Description
       This function installs the current paths as the boundary for
       clipping of subsequent drawing. The use of the clip operator
       may require some care, because clip and eoclip operators do
       not consume the current path.
       Parameters
       PaintBox :  [in] PaintBox object handle
       Note
       There is no practical way of removing a clipping path, except
       by save and restore a graphical state before clipping is
       imposed.
       Returns
       None.                                                         */
    LIB_API void PBXClip(PBXHandle PaintBox);
    
    
    
    /* Description
       This function installs the current paths as the boundary for
       clipping subsequent drawing and uses the "even-odd" rule for
       defining the "inside" that shows through the clipping window.
       The use of the clip operator may require some care, because
       clip and eoclip operators do not consume the current path.
       Parameters
       PaintBox :  [in] PaintBox object handle
       Note
       There is not practical way of removing a clipping path,
       except by saving and restoring a graphical state before
       clipping is imposed.
       Returns
       None.                                                         */
    LIB_API void PBXEoClip(PBXHandle PaintBox);



    /********************************************************************
    Description: 
        This function uses the current path as the boundary for color filling 
        and uses the "evenodd" rule for defining an "inside" that is painted.
    Parameters:
        PaintBox    - [in]  PaintBox object handle
    Returns: 
        None.
    *********************************************************************/
    LIB_API void PBXEoFill(PBXHandle PaintBox);
    


    /********************************************************************
    Description: 
        This function uses the current path as the boundary for color filling 
        and uses the "non-zero winding number" rule.
    Parameters:
        PaintBox    - [in]  PaintBox object handle
    Returns: 
        None.
    *********************************************************************/
    LIB_API void PBXFill(PBXHandle PaintBox);
    


    /* Description
       This function is used for the first filling the inside with
       the current fill color ( uses the "non-zero winding number"
       rule ), and then stroking the path with the current stroke
       color. PDF's graphics state maintains separate colors to fill
       and stroke operations, thus these combined operators are
       available.
       Parameters
       PaintBox :  [in] PaintBox object handle
       Returns
       None.                                                         */
    LIB_API void PBXEoFillAndStroke(PBXHandle PaintBox);
    


    /* Description
       This function is used for the first filling the inside with
       the current fill color, ( uses the "evenodd" rule for
       defining an "inside" that is painted ) and then stroking the
       path with the current stroke color. PDF's graphics state
       maintains separate colors to fill and stroke operations, thus
       these combined operators are available.
       Parameters
       PaintBox :  [in] PaintBox object handle
       Returns
       None.                                                         */
    LIB_API void PBXFillAndStroke(PBXHandle PaintBox);
    


    /********************************************************************
    Description:
        This function strokes the current paths by the current stroke color 
        and current line width.
    Parameters:
        PaintBox    - [in]  PaintBox object handle
    Returns: 
        None.
    *********************************************************************/
    LIB_API void PBXStroke(PBXHandle PaintBox);
    

    /*Graphic primitives*/



    /********************************************************************
    Description:
        This procedure moves the current point to the location specified 
        by ( x, y ).
    Parameters:
        PaintBox    - [in]  PaintBox object handle
        X           - [in]  The logical x-coordinate of the new position. 
        Y           - [in]  The logical y-coordinate of the new position.   
    Returns: 
        None.
    *********************************************************************/
    LIB_API void PBXMoveTo(PBXHandle PaintBox, ppReal X, ppReal Y);



    /* Description
       This procedure adds a line segment to the path, starting at
       the current point and ending at point ( x, y ).
       
       Current point sets to ( x, y ).
       Parameters
       PaintBox :  [in] PaintBox object handle
       X        :  [in] The logical x-coordinate of the endpoint for
                     the line.
       Y        :  [in] The logical y-coordinate of the endpoint for
                   the line.
       Returns
       None.                                                               */
    LIB_API void PBXLineTo(PBXHandle PaintBox, ppReal X, ppReal Y);



    /********************************************************************
    Description:
        This procedure adds a Bezier cubic curve segment to the path starting at 
        the current point as ( x0, y0 ), using two points ( x1, y1 ) and ( x2, y2 ) 
        as control points, and terminating at point ( x3, y3 ). The new current 
        point will be ( x3, y3 ). If there is no current point, an error will result.
    Parameters:
        PaintBox    - [in]  PaintBox object handle
        x1          - [in]  The logical x-coordinate of the first control point position. 
        y1          - [in]  The logical y-coordinate of the first control point position. 
        x2          - [in]  The logical x-coordinate of the second control point position. 
        y2          - [in]  The logical y-coordinate of the second control point position. 
        x3          - [in]  The logical x-coordinate of the new position. 
        y3          - [in]  The logical y-coordinate of the new position. 
    Returns: 
        None.
    *********************************************************************/
    LIB_API void PBXCurveTo(PBXHandle PaintBox, ppReal x1, ppReal y1, ppReal x2, ppReal y2, ppReal x3,
        ppReal y3);



    /********************************************************************
    Description:
        This function draws a rectangle with one corner at ( x1, y1 ) and second at ( x2, y2 ).
    Parameters:
        PaintBox    - [in]  PaintBox object handle
        x1          - [in]  The x-coordinate of the upper-left corner of the rectangle. 
        y1          - [in]  The y-coordinate of the upper-left corner of the rectangle. 
        x2          - [in]  The x-coordinate of the lower-right corner of the rectangle. 
        y2          - [in]  The y-coordinate of the lower-right corner of the rectangle. 
    Returns: 
        None.
    *********************************************************************/
    LIB_API void PBXRectangle(PBXHandle PaintBox, ppReal x1, ppReal y1, ppReal x2, ppReal y2);




    /* Advanced graphic operations*/



    /* Description
       This procedure creates a circular path centered at (X, Y)
       with radius "Rin the counter-clock-wise direction.
       Parameters
       PaintBox :  [in] PaintBox object handle
       X        :  [in] The X-coordinate of the center of the circle.
       Y        :  [in] The Y-coordinate of the center of the circle.
       R :         [in] The radius of the circle.
       Note
       If you need a circle drawn in the clock-wise direction,
       please use PBXArc. This function performs a move to angle 0
       (right edge) of the circle. Current point also will be at the
       same location after the call.
       Returns
       None.                                                                */
    LIB_API void PBXCircle(PBXHandle PaintBox, ppReal X, ppReal Y, ppReal R);



    /* Description
       This procedure creates an ellipse path specified by top left
       point at pixel coordinates ( x1, y1 ) and the bottom right
       point at ( x2, y2 ) in the counter-clock-wise direction.
       Parameters
       PaintBox :  [in] PaintBox object handle.
       x1 :        [in] The x\-coordinate of the upper\-left corner of the
                   bounding rectangle.
       y1 :        [in] The y\-coordinate of the upper\-left corner of the
                   bounding rectangle.
       x2 :        [in] The x\-coordinate of the lower\-right corner of the
                   bounding rectangle.
       y2 :        [in] The y\-coordinate of the lower\-right corner of the
                   bounding rectangle.
       Note
       If you need an ellipse drawn in the clock-wise direction,
       please use PBXArc. This function performs a move to angle 0
       (right edge) of the ellipse. Current point also will be at
       the same location after the call.
       Returns
       None.                                                                */
    LIB_API void PBXEllipse(PBXHandle PaintBox, ppReal x1, ppReal y1, ppReal x2, ppReal y2);



    /* Description
       Use Arc to create an elliptically curved path. The arc
       traverses the perimeter of an PBXEllipse which is bounded by
       the points ( x1, y1 ) and ( x2, y2 ). The drawn arc is
       following the perimeter of the ellipse , counterclockwise,
       from the starting point to the ending point. The starting
       point is defined by the intersection of the ellipse and a
       line is defined by the center of the ellipse and ( x3, y3).
       The ending point is defined by the intersection of the
       ellipse and a line is defined by the center of the ellipse
       and ( x4, y4 ).
       Parameters
       PaintBox :  [in] PaintBox object handle.
       x1 :        [in] Specifies the x\-coordinate of the upper\-left
                   corner of the bounding rectangle.
       y1 :        [in] Specifies the y\-coordinate of the upper\-left
                   corner of the bounding rectangle.
       x2 :        [in] Specifies the x\-coordinate of the lower\-right
                   corner of the bounding rectangle.
       y2 :        [in] Specifies the y\-coordinate of the lower\-right
                   corner of the bounding rectangle.
       x3 :        [in] Specifies the x\-coordinate of the point that
                   defines the arc's starting point.
       y3 :        [in] Specifies the y\-coordinate of the point that
                   defines the arc's starting point .
       x4 :        [in] Specifies the x\-coordinate of the point that
                   defines the arc's endpoint.
       y4 :        [in] Specifies the y\-coordinate of the point that
                   defines the arc's endpoint.
       Returns
       None.                                                            */
    LIB_API void PBXArc(PBXHandle PaintBox, ppReal x1, ppReal y1, ppReal x2, ppReal y2, ppReal x3,
                ppReal y3, ppReal x4, ppReal y4);
    
    

    /* Description
       Use Arc to create an elliptically curved path. The arc
       traverses the perimeter of an ellipse which is bounded by the
       points ( x1, y1 ) and ( x2, y2 ). The drawn arc is following
       the perimeter of the ellipse, counterclockwise, from the
       starting point to the ending point. The starting point is
       defined by the intersection of the ellipse and a line is
       defined by BegAngle and EndAngle, specified in degrees.
       Parameters
       PaintBox :    [in] PaintBox object handle.
       x1 :          [in] Specifies the x\-coordinate of the upper\-left
                     corner of the bounding rectangle.
       y1 :          [in] Specifies the y\-coordinate of the upper\-left
                     corner of the bounding rectangle.
       x2 :          [in] Specifies the x\-coordinate of the lower\-right
                     corner of the bounding rectangle.
       y2 :          [in] Specifies the y\-coordinate of the lower\-right
                     corner of the bounding rectangle.
       BeginAngle :  [in] Specifies the starting angle in degrees relative
                     to the x\-axis.
       EndAngle :    [in] Specifies the ending angle in degrees relative to
                     the x\-axis.
       Returns
       None.                                                                */
    LIB_API void PBXArc2(PBXHandle PaintBox, ppReal x1, ppReal y1, ppReal x2, ppReal y2,
                 ppReal BeginAngle, ppReal EndAngle);



    /* Description
       Use Pie to append a pie-shaped wedge on the path. The wedge
       is defined by the ellipse bounded by the rectangle determined
       by the points ( x1, y1 ) and ( x2, y2). The drawn section is
       determined by two lines radiating from the center of the
       ellipse through the points ( x3, y3 ) and ( x4, y4 )
       Parameters
       PaintBox :  [in] PaintBox object handle
       x1 :        [in] Specifies the x\-coordinate of the upper\-left
                   corner of the bounding rectangle.
       y1 :        [in] Specifies the y\-coordinate of the upper\-left
                   corner of the bounding rectangle.
       x2 :        [in] Specifies the x\-coordinate of the lower\-right
                   corner of the bounding rectangle.
       y2 :        [in] Specifies the y\-coordinate of the lower\-right
                   corner of the bounding rectangle.
       x3 :        [in] Specifies the x\-coordinate of the point that
                   defines the arc's starting point.
       y3 :        [in] Specifies the y\-coordinate of the point that
                   defines the arc's starting point .
       x4 :        [in] Specifies the x\-coordinate of the point that
                   defines the arc's endpoint.
       y4 :        [in] Specifies the y\-coordinate of the point that
                   defines the arc's endpoint.
       Note
       Current point is center of the wedge.
       Returns
       None.                                                            */
    LIB_API void PBXPie(PBXHandle PaintBox, ppReal x1, ppReal y1, ppReal x2, ppReal y2, ppReal x3,
                ppReal y3, ppReal x4, ppReal y4);




    /* Description
       Use Pie to append a pie-shaped wedge on the path. The wedge
       is defined by the ellipse bounded by the rectangle determined
       by the points ( x1, y1 ) and ( x2, y2). The drawn section is
       determined by two lines ( BegAngle and EndAngle, specified in
       degrees).
       Parameters
       PaintBox :    [in] PaintBox object handle
       x1 :          [in] The x\-coordinate of the upper\-left corner of
                     the rectangle.
       y1 :          [in] The y\-coordinate of the upper\-left corner of
                     the rectangle.
       x2 :          [in] The x\-coordinate of the lower\-right corner of
                     the rectangle.
       y2 :          [in] The y\-coordinate of the lower\-right corner of
                     the rectangle.
       BeginAngle :  [in] Specifies the starting angle in degrees relative
                     to the x\-axis.
       EndAngle :    [in] Specifies the ending angle in degrees relative to
                     the x\-axis.
       Note
       Current point is center of the wedge.
       Returns
       None.                                                                */
    LIB_API void PBXPie2(PBXHandle PaintBox, ppReal x1, ppReal y1, ppReal x2, ppReal y2,
                 ppReal BeginAngle, ppReal EndAngle);




    /********************************************************************
    Description:
        This function draws a rectangle of size ( w, h ) with one corner at ( x, y ), 
        with an orientation argument, angle, specified in degrees.
    Parameters:
        PaintBox    - [in]  PaintBox object handle
        X           - [in]  The x-coordinate of the lower-left corner of the rectangle. 
        Y           - [in]  The y-coordinate of the lower-left corner of the rectangle. 
        W           - [in]  The width of the rectangle. 
        H           - [in]  The height of the rectangle. 
        Angle       - [in]  Specifies the angle, in degrees, between the escapement vector and the x-axis of the device. 
    Returns: 
        None.
    *********************************************************************/
    LIB_API void PBXRectRotated(PBXHandle PaintBox, ppReal X, ppReal Y, ppReal W, ppReal H, ppReal Angle);




    /* Description
       Adds a rectangle with rounded corners to path. The rectangle
       will have edges defined by the points ( x1, y1 ), ( x2, y1 ),
       ( x2, y2 ), ( x1, y2 ), but the corners will be shaved to
       create a rounded appearance. The curve of the rounded corners
       matches the curvature of an ellipse with width x3 and height
       y3
       Parameters
       PaintBox :  [in] PaintBox object handle
       x1 :        [in] The x\-coordinate of the upper\-left corner of the
                   rectangle.
       y1 :        [in] The y\-coordinate of the upper\-left corner of the
                   rectangle.
       x2 :        [in] The x\-coordinate of the lower\-right corner of the
                   rectangle.
       y2 :        [in] The y\-coordinate of the lower\-right corner of the
                   rectangle.
       x3 :        [in] Specifies the width of the ellipse used to draw the
                   rounded corners.
       y3 :        [in] Specifies the height of the ellipse used to draw
                   the rounded corners.
       Returns
       None.                                                                */
	LIB_API void PBXRoundRect(PBXHandle PaintBox, ppReal x1, ppReal y1, ppReal x2, 
        ppReal y2, ppReal x3, ppReal y3);




    /* Graphic state operators*/
    


    /********************************************************************
    Description:
        This function pushes a copy of the entire graphics state onto the stack.
    Parameters:
        PaintBox    - [in]  PaintBox object handle
    Returns: 
        None.
    *********************************************************************/
    LIB_API void PBXStateStore(PBXHandle PaintBox);



    /********************************************************************
    Description:
        This function restores the entire graphics state to its former value by 
        popping it from the stack.
    Parameters:
        PaintBox    - [in]  PaintBox object handle
    Returns: 
        None.
    *********************************************************************/
    LIB_API void PBXStateRestore(PBXHandle PaintBox);



    /********************************************************************
    Description:
        This function resets the dash pattern back to none, i.e., solid line.
    Parameters:
        PaintBox    - [in]  PaintBox object handle
    Returns: 
        None.
    *********************************************************************/
    LIB_API void PBXNoDash(PBXHandle PaintBox);



    /********************************************************************
    Description:
        The line dash pattern controls the pattern of dashes and gaps used to 
        stroke paths.
    Parameters:
        PaintBox    - [in]  PaintBox object handle.
        Dash        - [in]  Dash pattern style.
    Returns: 
        None.
    *********************************************************************/
    LIB_API void PBXSetDash(PBXHandle PaintBox, char *Dash);



    /* Description
       The flatness tolerance controls the maximum permitted
       distance in device pixels between the mathematically correct
       path and an approximation constructed with straight line
       segments.
       Parameters
       PaintBox :  [in] PaintBox object handle.
       Flatness :  [in] The flatness tolerance value.
       Returns
       None.                                                        */
    LIB_API void PBXSetFlatness(PBXHandle PaintBox, ppInt32 Flatness);



    /* Description
       This function sets both filling and stroking color to the
       specified values.
       Parameters
       PaintBox :  [in] PaintBox object handle
       Color :     [in] The color of the filling and stroking
       Returns
       None.                                                     */
    LIB_API void PBXSetColor(PBXHandle PaintBox, TPDFColor Color);



    /* Description
       This function sets filling color to the specified values .
       Parameters
       PaintBox :  [in] PaintBox object handle
       Color :     [in] The color of the filling
       Returns
       None.                                                      */
    LIB_API void PBXSetFillColor(PBXHandle PaintBox, TPDFColor Color);


    /* Description
       This function sets stroking color to the specified values.
       Parameters
       PaintBox :  [in] PaintBox object handle
       Color :     [in] The color of the stroke
       Returns
       None.                                                      */
    LIB_API void PBXSetStrokeColor(PBXHandle PaintBox, TPDFColor Color);


    /********************************************************************
    Description:
        This procedure sets the current line width to the value specified in points.
    Parameters:
        PaintBox    - [in]  PaintBox object handle.
        LineWidth   - [in]  Current line width.
    Returns: 
        None.
    *********************************************************************/
    LIB_API void PBXSetLineWidth(PBXHandle PaintBox, ppReal LineWidth);


    /* Description
       Specifies line cap style.
       Parameters
       PaintBox :  [in] PaintBox object handle.
       LineCap :   [in] Element of the TPDFLineCap enumeration that
                   specifies the line cap.
       Returns
       None.                                                        */
    LIB_API void PBXSetLineCap(PBXHandle PaintBox, TPDFLineCap LineCap);


    /********************************************************************
    Description:
        The SetLineJoin method sets the line join for this PaintBox.
    Parameters:
        PaintBox    - [in]  PaintBox object handle
        LineJoin    - [in]  Element of the TPDFLineJoin enumeration that specifies 
                            the join style used at the end of a line segment that 
                            meets another line segment.
    Returns: 
        None.
    *********************************************************************/
    LIB_API void PBXSetLineJoin(PBXHandle PaintBox, TPDFLineJoin LineJoin);


    /* Description
       The SetMiterLimit method sets the miter limit of this
       PaintBox object.
       Parameters
       PaintBox :    [in] PaintBox object handle
       MiterLimit :  [in] Real number that specifies the miter limit of
                     this PaintBox object.
       Remarks
       Returns
       None.                                                            */
    LIB_API void PBXSetMiterLimit(PBXHandle PaintBox, ppReal MiterLimit);

    /* Description
       This function sets extended graphic state which can be
       created with function PDFExtGraphicStateNew.
       Parameters
       PaintBox :  [in] PaintBox object handle
       Index :     [in] Index of the created extended graphic state
                   returned from function PDFExtGraphicStateNew
       Returns
       None.                                                        */
    LIB_API void PBXSetGState ( PBXHandle PaintBox, ppUns32 Index );
    

    /* Description
       This function returns width of the PaintBox.
       Parameters
       PaintBox :  [in] PaintBox object handle
       Returns
       Width of the PaintBox in logical units.      */
	LIB_API ppReal PBXGetWidth( PBXHandle PaintBox );



    /* Description
       This function returns height of the PaintBox.
       Parameters
       PaintBox :  [in] PaintBox object handle
       Returns
       Height of the PaintBox in logical units.      */
	LIB_API ppReal PBXGetHeight( PBXHandle PaintBox );

    /* Image operator */


    /* Description
       This function places the image data of size ( Width , Height
       ) with one corner at (x,y), and angle, specified in degrees
       (ImageIndex is returned by one of the function appending
       image to PDF document) into the current content stream for
       the page
       Parameters
       PaintBox :  [in] PaintBox object handle
       Index    :  [in] Index of the image appended to PDF document early.
       X        :  [in] The x-coordinate of the lower\-left corner of
                     the image.
       Y        :  [in] The y-coordinate of the lower\-left corner of
                   the image.
       Width    :  [in] The width of the image.
       Height   :  [in] The height of the image.
       Angle    :  [in] Specifies the angle, in degrees, between the
                   escapement vector and the x\-axis.
       Returns
       None.                                                                */
    LIB_API void PBXShowImage ( PBXHandle PaintBox, ppUns32 Index, ppReal X, ppReal Y, ppReal Width, ppReal Height, ppReal Angle);

    /* Text operations */


#ifdef WINDOWS_PLATFORM

	/********************************************************************
    Description:
        This function sets the active font for text operations.
    Parameters:
        PaintBox    - [in]  PaintBox object handle
        Index       - [in]  Index of the font appended to PDF document early.
        FontSize    - [in]  Size of output text, in units
        Charset     - [in]  Charset of the output text
        Underline   - [in]  Specifies an underlined font if set to true. 
        StrikeOut   - [in]  Specifies an strikeout font if set to true. 
    Returns: 
        None.
    Platform:
        Windows only.
    *********************************************************************/

    LIB_API void PBXSetActiveFontWithCharset(PBXHandle PaintBox, ppUns32 Index, ppReal FontSize,
                                     ppUns8 Charset, ppBool UnderLine, ppBool StrikeOut);
#endif


    /********************************************************************
    Description:
        This function sets the active font for text operations.
    Parameters:
        PaintBox    - [in]  PaintBox object handle
        Index       - [in]  Index of the font appended to PDF document early.
        FontSize    - [in]  Size of output text, in units
        Underline   - [in]  Specifies an underlined font if set to true. 
        StrikeOut   - [in]  Specifies an strikeout font if set to true. 
    Returns: 
        None.
    *********************************************************************/
    LIB_API void PBXSetActiveFont(PBXHandle PaintBox, ppUns32 Index, ppReal FontSize, ppBool UnderLine,
                          ppBool StrikeOut);


    /********************************************************************
    Description:
        This function sets the additional space that should be inserted between characters.
    Parameters:
        PaintBox    - [in]  PaintBox object handle
        Spacing     - [in]  Size between character in points
    Returns: 
        None.
    *********************************************************************/
    LIB_API void PBXSetCharacterSpacing(PBXHandle PaintBox, ppReal Spacing);



    /* Description
       This function sets the horizontal scaling factor in a
       percentage. This essentially expands or compresses the
       horizontal dimension of the string. The default value for
       this parameter is 100 (%).
       Parameters
       PaintBox :  [in] PaintBox object handle
       Scale :     [in] horizontal scaling factor in a percentage.
       Returns
       None.                                                       */
    LIB_API void PBXSetHorizontalScaling(PBXHandle PaintBox, ppReal Scale);



    /* Description
       This function sets the mode that determines how the outline
       character is used. By default, the outline character is used
       for filling operations by which inside the outline path is
       painted solidly with the current fill color. This may be
       changed by calling this function.
       Parameters
       PaintBox :  [in] PaintBox object handle
       Mode :      [in] Rendering mode
       Returns
       None.
       Note
       Available modes at current time:
       <table>
       Mode   Action
       -----  --------------------------------------------------------
       0      Fill text.
       1      Stroke text.
       2      Fill, then stroke, text.
       3      Neither fill nor stroke text (invisible).
       4      Fill text and add to path for clipping.
       5      Stroke text and add to path for clipping.
       6      Fill , then stroke , text and add to path for clipping.
       7      Add text to path for clipping.
       </table>                                                        */
    LIB_API void PBXSetTextRenderingMode(PBXHandle PaintBox, ppInt32 Mode);





    /********************************************************************
    Description:
        This procedure sets the additional space (in points) that should be 
        inserted between words, i.e., for every space character found in the text string.
    Parameters:
        PaintBox    - [in]  PaintBox object handle
        Spacing     - [in]  Additional space (in points) that should be 
                            inserted between words
    Returns: 
        None.
    *********************************************************************/
    LIB_API void PBXSetWordSpacing(PBXHandle PaintBox, ppReal Spacing);



    /********************************************************************
    Description:
        Writes a character string at the specified location using the currently 
        selected font.
    Parameters:
        PaintBox    - [in]  PaintBox object handle
        X           - [in]  Specifies the x-coordinate of the starting point of the text.
        Y           - [in]  Specifies the y-coordinate of the starting point of the text.
        Orientation - [in]  Specifies the angle, in degrees, between the escapement vector and the x-axis of the device. 
        TextStr     - [in]  Pointer to the string to be drawn. Must be zero terminated.
    Returns: 
        None.
    *********************************************************************/
    LIB_API void PBXTextOut(PBXHandle PaintBox, ppReal X, ppReal Y, ppReal Orientation, char *TextStr);
    
    

    /********************************************************************
    Description:
        Writes a character string at the specified location using the currently 
        selected font.
    Parameters:
        PaintBox    - [in]  PaintBox object handle
        X           - [in]  Specifies the x-coordinate of the starting point of the text.
        Y           - [in]  Specifies the y-coordinate of the starting point of the text.
        Orientation - [in]  Specifies the angle, in degrees, between the escapement vector and the x-axis of the device. 
        Text        - [in]  Pointer to string for drawing.
        Len         - [in]  Specifies the length of the string. It is a WORD count.
    Returns: 
        None.
    *********************************************************************/
    LIB_API void PBXUnicodeTextOut(PBXHandle PaintBox, ppReal X, ppReal Y, ppReal Orientation,
        PppUns16 Text, ppInt32 Len);


    /********************************************************************
    Description:
        Writes a character string at the specified text box using the currently 
        selected font.
    Parameters:
        PaintBox    - [in]  PaintBox object handle
        X           - [in]  Specifies the x-coordinate of the starting point of the text.
        Y           - [in]  Specifies the y-coordinate of the starting point of the text.
        Orientation - [in]  Specifies the angle, in degrees, between the escapement vector and the x-axis of the device. 
        TextStr     - [in]  Pointer to the string to be drawn. Must be zero terminated.
    Returns: 
        Count of the characters which was writed.
    *********************************************************************/

	LIB_API ppUns32 PBXTextOutBox(PBXHandle PaintBox, ppReal X, ppReal Y, ppReal Width, ppReal Height, ppReal Interval, char * TextStr);


	LIB_API ppUns32 PBXUnicodeOutBox(PBXHandle PaintBox, ppReal X, ppReal Y, ppReal Width, ppReal Height, ppReal Interval, PppUns16 Text, ppUns32 Len);

    
    
    
    /********************************************************************
    Description:
        Returns the width of a text string as it would be displayed in the current font.
    Parameters:
        PaintBox    - [in]  PaintBox object handle
        Text        - [in]  Specifies the character string for which the text width is determined.
                            Must be zero terminated.
    Returns: 
        Width of the text.
    *********************************************************************/
    LIB_API ppReal PBXGetTextWidth(PBXHandle PaintBox, char *Text);


    /********************************************************************
    Description:
        Returns the height of a text string as it would be displayed in the current font.
    Parameters:
        PaintBox    - [in]  PaintBox object handle   
    Returns: 
        Height of the text.
    *********************************************************************/

	LIB_API ppReal PBXGetTextHeight(PBXHandle PaintBox);




    /********************************************************************
    Description:
        Returns the width of a text string as it would be displayed in the current font.
    Parameters:
        PaintBox    - [in]  PaintBox object handle
        Text        - [in]  Specifies the character string for which the text width is determined.
        Len         - [in]  Specifies the length of the string. It is a WORD count.
    Returns: 
        Width of the text.
    *********************************************************************/
    LIB_API ppReal PBXGetUnicodeWidth(PBXHandle PaintBox, PppUns16 Text, ppInt32 Len);

/********************************************************************
    Description:
        Append misc line to content.
    Parameters:
        PaintBox    - [in]  PaintBox object handle
        LineCode        - [in]  Line of the code which need append to content.
    Returns: 
        None.
    *********************************************************************/
    LIB_API void PBXAppendLine(PBXHandle PaintBox, char * LineCode);



#ifdef WINDOWS_PLATFORM

    /* Description
       Function paints context of the metafile on the page.
       Parameters
       PaintBox :  [in] PaintBox object handle
       Metafile :  [in] Handle to the enhanced metafile
       X        :  [in] The X-coordinate of the upper-left corner
                     of the drawing rectangle.
       Y        :  [in] The Y-coordinate of the upper-left corner
                   of the drawing rectangle.
       XScale :    [in] X-scaling factor to play metafile.
       YScale :    [in] Y-scaling factor to play metafile.
       Returns
       None.
       Version
       Professional only.
       Platforms
       Windows only.                                                     */
    LIB_API void PBXPlayMetaFile(PBXHandle PaintBox, HGDIOBJ Metafile, ppReal X, ppReal Y, ppReal XScale,
        ppReal YScale);
#endif

#ifdef __cplusplus
}
#endif

#endif /* #ifndef VSCanvasA_H */
