# JSON files used by DasherServer:
## JSON frame containing objects to be drawn:

This frame is sent by the server to tell the client what to draw on the canvas
```js
	 {
        "T" : "F",
        "G" : [{<Geometry_0>},...,],
```

where `{<Geometry_0>}` references one of the below types

### The compontens:

#### StringToDraw

String to be drawn on the canvas. `<StringToDraw>` serialized as follows:

```js
    {
		"G" : "S",
        "L" : {
            "S" : "<Label->m_strText>",
            "W" : <Label->m_iWrapSize>
        },
        "X" : <X>,
        "Y" : <Y>,
        "F" : <iFontSize>,
        "C" : <iColor>,
	 }
```
where:
`"<Label->m_strText>"`  string to be drawn,
 `<Label->m_iWrapSize>` from the Dasher code:
 ```c++
    ///If 0, Label is to be rendered on a single line.
    /// Any other value, Label need only be renderable at that size, but should 
    /// be _wrapped_ to fit the screen width. (It is up to platforms to decide
    /// whether to support DrawString/TextSize at any other size but this is
    /// NOT required.)
 ```
`<X>` and `<Y>`         coordinates where the text should be drawn (0,0) being the top left corner,
`<iFontSize>`           font size in points
`<iColor>`              text color that should be drawn as in integer index to a color map.

#### RectangleToDraw
Rectangle to be drawn on the canvas. `<RectangleToDraw>` serialized as follows:
```js
	{
		"G" : "R",
        "X1" : <X1>,
        "Y1" : <Y1>,
        "X2" : <X2>,
        "Y2" : <Y2>,
        "C" : <iColor>,
        "O" : <iOutlineColor>, //assume 0 if not sent
        "T" : <iThickness>     //assume 0 if not sent
	}
```
where:
`<X1>` and `<Y1>` coordinates for the top left corner of the rectangle
`<X2>` and `<Y2>` coordinates for the bottom corner of the rectangle
`<iColor>`        fill color as an integer index to a color map
The follwing are **optional**, assume no outline if not present in the frame:
`<iOutlineColor>` outline color as an integer index to a color map  
`<iThickness>`    outline thickness (integer)

#### CircleToDraw
Circle to be drawn on the canvas. `{<CircleToDraw>}` serialized as follows:

```js
	{
		"G" : "C",
        "X" : <iCX>,
        "Y" : <iCY>,
        "R" : <iR>,
        "F" : <iFillColor>,
        "L" : <iLineColor>,    //assume no border if not sent
        "W" : <iLineWidth>     //assume no border if not sent
	}
```
where:
`<iCX>` and `<iCY>` circle center coordinates
`<iR>`              circle radius (integer)
`<iFillColor>`      fill color as an integer index to a color map
The follwing are **optional**, assume no outline if not present in the frame:
`<iLineColor>`      outline color as an integer index to a color map
`<iLineWidth>`      outline thickness (integer)

#### PolylineToDraw
Polyline to be drawn on the canvas. `<PolylineToDraw>` serialized as follows:

```js
	{
		"G" : "L",
        "P" : [[<X_0>,<Y_0>],...,[<X_Number>,<Y_Number>]],
        "W" : <iWidth>,
        "C" : <iColor>
	}
```

`[[<X_0>,<Y_0>],...,[<X_Number>,<Y_Number>]]` array of arrays containing coordinates of to points to be connected
`<iWidth>`                                    width of the line (integer)
`<iColor>`                                    line color as in integer index to a color map.



#### PolygonToDraw

Polygon to be drawn on the canvas. `<PolygonToDraw>` serialized as follows:

```js
	{
		"G" : "P",
	 	"P" : [[<X_0>,<Y_0>],...,[<X_Number>,<Y_Number>]],
	 	"F" : <iFillColor>,
	 	"O" : <iOutlineColor>,      //assume 0 if not sent
	 	"W" : <LineWidth>           //assume 0 if not sent
	}
```

`[[<X_0>,<Y_0>],...,[<X_Number>,<Y_Number>]]` array of arrays containing coordinates of to be polygon's vertices
`<iFillColor>`                                fill color as an integer index to a color map
The follwing are **optional**, assume no outline if not present in the frame:
`<iOutlineColor>`                             outline color as an integer index to a color map
`<LineWidth>`                                 outline thickness (integer)


## Font measuring alphabet request JSON

This frame is sent by the server to request font size measurements by the client

```js
    {
		"T" : "A",
		"C" : ["<DisplayText_0>", ...]
		"S" : [<font_size_0>, ...]
	}
```

`["<DisplayText_0>", ...]` array of strings containing the text to be measured
`[<font_size_0>, ...]`     array of integer font sizes in pt to be measured

## Font measuring alphabet receive JSON

This frame is sent to the server to answer the alphabet request JSON above
```js
	{
		"T" : "S",
		"C" : [
			{
				"D" : "<DisplayText_0>",
				"W" : [<width_for_font_size_0>, ...]
			},
			...
		],
		"H" : [<line_height_for_font_size_0>,...]
	}
```


```js
"C" :   [
			{
				"D" : "<DisplayText_0>",
				"W" : [<width_for_font_size_0>, ...]
			},
			...
	    ],
```
Array of texts and their corresponding widths. Texts are strings and widths are an integer array, value in pixels.
```js
"H" : [<line_height_for_font_size_0>,...]
```
array of heights in pixels corresponding to the font size.

## Send Buffer JSON
Sends the text buffer contents to the client whenever the buffer is changed.

```js
	{
	 	"T" : "B",
	 	"B" : <NewBuffer>
	}
```
`<NewBuffer>` String containing the buffer

## Screen Resize JSON
JSON sent by the client to tell the server the new screen width and height

```js
    {
        "T": "R",
        "W": <iWidth>,
        "H": <iHeight>
    }
```
`<iWidth>` and `<iHeight>` Updated width and height of the screen in pixels (unsigned integers)

## Mouse Position JSON
Sent by the client to update the server on the mouse position

```js
    {
        "T": "C",
        "X": <iX>,
        "Y": <iY>
    }
```
`<iX>` and `<iY>` x and y position of the mouse (integer)


## Mouse Click JSON
Sent by the client to notify the server of a mouse click


```js
    {
        "T": "M",
        "D": <bMouseDown>
    }
```
`<bMouseDown>` boolean, True if mouse button was pressed, False if released.

## Set Parameters in Dasher
Sent by the client to set options in Dasher
```js
    {
        "T": "P",
        "N": "<Parameter_Name>"
        "V": <Parameter_Value>
    }
```
`<Parameter_Name>` string, defines which parameter is set (see ParameterDefinitions.h for possible values)
`<Parameter_Value>` either String/Bool/Long, defines the value of this parameter
